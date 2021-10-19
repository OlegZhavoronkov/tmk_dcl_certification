import asyncio
import functools
import json
import logging
import traceback
from enum import Enum
from json.decoder import JSONDecodeError
from time import sleep

import psycopg2
import requests

# dbname should be the same for the notifying process
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT
from requests import HTTPError

TIMER_CONST = 12
CFG_PATH = 'config/config.json'
MAPPER_PATH = 'config/mapping.json'


class ObjectOfInterest(Enum):
    billet = 1
    pipe = 2
    mandrel = 3

    @classmethod
    def has_not_name(cls, value):
        return value not in cls._member_names_


class ObjectOfInterestException(Exception):
    pass


class WinnumLogger:
    logger = None

    @classmethod
    def init_logger(cls):
        logging.basicConfig(level=logging.INFO,
                            format="[%(levelname)s] %(asctime)s %(module)s - %(funcName)s: %(message)s",
                            datefmt='%Y-%m-%d %H:%M:%S')

    @classmethod
    def get_logger(cls):
        cls.logger = logging.getLogger(__name__)


class Mapper:
    mandrel = None
    pipe = None
    billet = None

    @classmethod
    def load(cls, cfg_path):
        try:
            with open(cfg_path) as config_file:
                cfg_json = json.load(config_file)
                cls.mandrel = cfg_json['mandrel']
                cls.pipe = cfg_json['pipe']
                cls.billet = cfg_json['billet']
        except JSONDecodeError:
            WinnumLogger.logger.error("Mapper config format error")
            exit(1)
        except KeyError:
            WinnumLogger.logger.error("Mapper config reading error")
            exit(1)
        except FileNotFoundError:
            WinnumLogger.logger.error(f"File {MAPPER_PATH} not found")
            exit(1)
        except ObjectOfInterestException:
            WinnumLogger.logger.error("Invalid name for object of interests in config file. ['system']['name']")
            exit(1)

class Config:
    name = None
    postgresql_host = None
    dbname = None
    postgrest_address = None
    user = None
    password = None
    channel = None
    winnum_address = None
    device = None
    key = None

    @classmethod
    def load(cls, cfg_path):
        try:
            with open(cfg_path) as config_file:
                cfg_json = json.load(config_file)
                cls.name = cfg_json['system']['name']
                if ObjectOfInterest.has_not_name(cls.name):
                    raise ObjectOfInterestException
                cls.postgresql_host = cfg_json['system']['database']['postgresql_host']
                cls.dbname = cfg_json['system']['database']['dbname']
                cls.user = cfg_json['system']['database']['user']
                cls.password = cfg_json['system']['database']['password']
                cls.channel = cfg_json['system']['database']['channel']
                cls.postgrest_address = cfg_json['system']['postgrest']['postgrest_address']
                cls.winnum_address = cfg_json['system']['winnum']['winnum_address']
                cls.device = cfg_json['system']['winnum']['device']
                cls.key = cfg_json['system']['winnum']['key']

        except JSONDecodeError:
            WinnumLogger.logger.error("Config format error")
            exit(1)
        except KeyError:
            WinnumLogger.logger.error("Config reading error")
            exit(1)
        except FileNotFoundError:
            WinnumLogger.logger.error(f"File {CFG_PATH} not found")
            exit(1)
        except ObjectOfInterestException:
            WinnumLogger.logger.error("Invalid name for object of interests in config file. ['system']['name']")
            exit(1)

class EntityProcessor:
    def __init__(self, data_payload, payload, device):
        self.payload = payload
        self.data_payload = data_payload
        self.device = device

    def __is_match_conditions(self, oi_json):
        if oi_json['condition']:
            return True
        return False

    def __return_mapper(self, object_of_interest):
        mapper = None
        if object_of_interest == ObjectOfInterest.mandrel:
            mapper = Mapper.mandrel
        elif object_of_interest == ObjectOfInterest.pipe:
            mapper = Mapper.pipe
        elif object_of_interest == ObjectOfInterest.billet:
            mapper = Mapper.billet
        return mapper

    def process(self, object_of_interest):
        mapper = self.__return_mapper(object_of_interest)
        WinnumLogger.logger.info(f"{Config.name} postgREST result: {self.payload}")
        #
        # Billet has no table for deffects
        #
        if not self.__is_match_conditions(self.payload[0]) and object_of_interest != ObjectOfInterest.billet:
            WinnumLogger.logger.info("Mandrel is not match conditions. Getting additional data...")
            defects = get_object_id(self.data_payload, object_of_interest, defected=True)
            WinnumLogger.logger.info(f"Defects count: {len(defects)}")
            for deffect in defects:
                WinnumLogger.logger.info(f"Defect:")
                for winnum_k_v in convert_to_winnum(deffect):
                    try:
                        send_winnum_request(mapper[winnum_k_v['ID']], winnum_k_v['VALUE'], self.device)
                    except KeyError:
                        WinnumLogger.logger.warning(f"Key '{winnum_k_v['ID']}' is absent in {MAPPER_PATH} ")
        else:
            for winnum_k_v in convert_to_winnum(self.payload[0]):
                try:
                    send_winnum_request(mapper[winnum_k_v['ID']], winnum_k_v['VALUE'], self.device)
                except KeyError:
                    WinnumLogger.logger.warning(f"Key '{winnum_k_v['ID']}' is absent in {MAPPER_PATH}")


def handle_notify(c, q):
    c.poll()
    q.put_nowait(conn.notifies.pop(0))


def send_winnum_request(signal_id, signal_value, device):
    valid = False
    timer = 0

    endpoint = "/WinnumCloud/rest/unauth/put/url/"
    device = device
    url = Config.winnum_address+endpoint+device

    payload = {'id': signal_id,
               'value': signal_value}
    WinnumLogger.logger.info(f"REQUEST: {url} params: {payload}")

    while (valid == False) and (timer != TIMER_CONST):
        try:
            response = requests.request("GET", url, params=payload)
            response.raise_for_status()
            valid = True
        except HTTPError as e:
            WinnumLogger.logger.warning(f"Get {e.response.status_code} statuscode. Retry...")
            stacktrace = traceback.format_exc()
            sleep(5)
            timer += 1
    if timer == TIMER_CONST:
        WinnumLogger.logger.error("The error can not be eliminated. Return to main cycle.")
        WinnumLogger.logger.error(f"RESPONSE: status: {response.status_code} body: {response.json()}")
        if stacktrace is not None:
            WinnumLogger.logger.error(stacktrace)
        return


def convert_to_winnum(object_json):
    """
    Convert TMK objects record to winnum signals format
    :param object_json: must be dict, not str for correct work
    :return: winnum dict ready to be as json body
    """
    js_array = []
    for key in object_json:
        js_array.append({
            "ID": key,
            "VALUE": object_json[key]
        })
    return js_array


def get_object_id(obj_id, object_of_interest: ObjectOfInterest, defected: bool):
    """
    Gets TMK objects record by object_id
    :param defected:
    :param object_of_interest:
    :param obj_id:
    :return:
    """
    valid = False
    timer = 0

    if not isinstance(object_of_interest, ObjectOfInterest):
        raise Exception(f"{object_of_interest} is not a valid value from ObjectOfInterest")
    url = None
    if not defected:
        if object_of_interest == ObjectOfInterest.mandrel:
            url = f"{Config.postgrest_address}/mandrels"
        if object_of_interest == ObjectOfInterest.pipe:
            url = f"{Config.postgrest_address}/pipes"
        if object_of_interest == ObjectOfInterest.billet:
            url = f"{Config.postgrest_address}/billets"
    else:
        if object_of_interest == ObjectOfInterest.mandrel:
            url = f"{Config.postgrest_address}/mandrels_with_defects"
        if object_of_interest == ObjectOfInterest.pipe:
            url = f"{Config.postgrest_address}/pipes_with_defects"
        if object_of_interest == ObjectOfInterest.billet:
            url = f"{Config.postgrest_address}/billets_with_defects"

    payload = {'object_id': f"eq.{obj_id}"}

    while (valid == False) and (timer != TIMER_CONST):
        try:
            response = requests.get(url, params=payload)
            response.raise_for_status()
            result = response.json()
            valid = True
        except HTTPError as e:
            WinnumLogger.logger.warning(f"Get {e.response.status_code} statuscode. Retry...")
            stacktrace = traceback.format_exc()
            sleep(5)
            timer += 1
        except IndexError:
            return []
        except Exception:
            WinnumLogger.logger.error("Something wrong with PostgREST service...")
            return []

    if timer == TIMER_CONST:
        WinnumLogger.logger.error("The error can not be eliminated. Return to main cycle.")
        WinnumLogger.logger.error(f"RESPONSE: status: {response.status_code} body: {response.json()}")
        if stacktrace is not None:
            WinnumLogger.logger.error(stacktrace)
        return []
    return result


def handle_object_of_interest(data_payload, device, object_type):
    if object_type == ObjectOfInterest.mandrel.name:
        mandrels = get_object_id(data_payload, ObjectOfInterest.mandrel, defected=False)
        if not mandrels:
            WinnumLogger.logger.info(
                f"Skipping object: {data_payload} is not valid {Config.name} or can not get data from PostgRest server")
            return
        else:
            EntityProcessor(data_payload=data_payload, payload=mandrels, device=device).process(ObjectOfInterest.mandrel)

    if object_type == ObjectOfInterest.pipe.name:
        pipes = get_object_id(data_payload, ObjectOfInterest.pipe, defected=False)
        if not pipes:
            WinnumLogger.logger.info(
                f"Skipping object: {data_payload} is not valid {Config.name} or can not get data from PostgRest server")
            return
        else:
            EntityProcessor(data_payload=data_payload, payload=pipes, device=device).process(ObjectOfInterest.pipe)

    if object_type == ObjectOfInterest.billet.name:
        billets = get_object_id(data_payload, ObjectOfInterest.billet, defected=False)
        if not billets:
            WinnumLogger.logger.info(
                f"Skipping object: {data_payload} is not valid {Config.name} or can not get data from PostgRest server")
            return
        else:
            EntityProcessor(data_payload=data_payload, payload=billets, device=device).process(ObjectOfInterest.billet)


async def handle_queue(device, queue):
    print()
    print(f"Waiting for {Config.name}s...")
    while True:
        data = await queue.get()
        # None is the quit signal
        if data is not None:
            WinnumLogger.logger.info(f"NOTIFICATION RECEIVED: {data}")
            data_payload = json.loads(data.payload.strip("'"))
            #this place is hardcode of message structure
            #TODO: make class Message
            oi_name = data_payload['Data']['obj_generated_id']
            handle_object_of_interest(oi_name, device, Config.name)
        else:
            return


def listen_for_objects(conn, channel):
    with conn.cursor() as cursor:
        cursor.execute(f"LISTEN {channel};")
    cursor.close()


if __name__ == '__main__':
    WinnumLogger.init_logger()
    WinnumLogger.get_logger()
    Mapper.load(MAPPER_PATH)
    Config.load(CFG_PATH)
    conn = None
    loop = None

    try:
        conn = psycopg2.connect(host=Config.postgresql_host, dbname=Config.dbname, user=Config.user, password=Config.password)
        conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)
        print("Winnum sender".center(80, '='))
        listen_for_objects(conn, Config.channel)

        q = asyncio.Queue()
        loop = asyncio.get_event_loop()
        loop.add_reader(conn, callback=functools.partial(handle_notify, conn, q))
        loop.run_until_complete(handle_queue(Config.device, q))

    except psycopg2.DatabaseError as e:
        WinnumLogger.logger.error(e)
    # Need for ctrl-c correct stop
    except KeyboardInterrupt:
        WinnumLogger.logger.info("Stopping queue...")
        if q is not None:
            q.put_nowait(None)
        WinnumLogger.logger.info("Application closed by keyboard!")
    finally:
        if loop is not None:
            loop.close()

        if conn is not None:
            conn.close()
