from datetime import datetime

import psycopg2
import pytest

from sent_to_winnum import WinnumLogger, Config

CFG_PATH = 'config/config.json'


@pytest.fixture(scope='session')
def pg_connect():
    conn = None
    try:
        conn = psycopg2.connect(host=Config.postgresql_host, dbname=Config.dbname, user=Config.user,
                                password=Config.password)
        with conn.cursor() as cursor:
            yield cursor
        cursor.close()
    except psycopg2.DatabaseError as e:
        print(e)
    finally:
        if conn is not None:
            conn.close()


@pytest.fixture(scope='session', autouse=True)
def init():
    WinnumLogger.init_logger()
    WinnumLogger.get_logger()
    Config.load(CFG_PATH)


@pytest.fixture
def build_bad_mandrel_query():
    obj_id = datetime.now().strftime("object_%Y_%m_%d_%H_%M_%S_%f")
    return (
        f"BEGIN TRANSACTION;\n"

        f"INSERT INTO objects\n"
        f"VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'mandrel'), 13, '{obj_id}');\n"

        f"COMMIT;\n"
    )


@pytest.fixture
def build_bad_pipe_query():
    obj_id = datetime.now().strftime("object_%Y_%m_%d_%H_%M_%S_%f")
    return (
        f"BEGIN TRANSACTION;\n"

        f"INSERT INTO objects\n"
        f"VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'mandrel'), 13, '{obj_id}');\n"

        f"COMMIT;\n"
    )


@pytest.fixture
def build_valid_mandrel_query():
    obj_id = datetime.now().strftime("object_%Y_%m_%d_%H_%M_%S_%f")
    return (
        f"BEGIN TRANSACTION;\n"

        f"INSERT INTO objects\n"
        f"VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'mandrel'), 13, '{obj_id}');\n"
        
        f"INSERT INTO mandrels(id, object_id, last_detection, diameter, CONDITION)\n"
        f"VALUES (DEFAULT, (SELECT max(id) AS max_object_id FROM objects o), '2021-08-10 12:14:16.928', 556.000000, TRUE);\n"

        f"COMMIT;\n"
    )


@pytest.fixture
def build_valid_pipe_query():
    obj_id = datetime.now().strftime("object_%Y_%m_%d_%H_%M_%S_%f")
    return (
        f"BEGIN TRANSACTION;\n"

        f"INSERT INTO objects\n"
        f"VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'pipe'), 13, '{obj_id}');\n"

        f"INSERT INTO pipes(id, object_id, last_detection, diameter, CONDITION)\n"
        f"VALUES (DEFAULT, (SELECT max(id) AS max_object_id FROM objects o), '2021-08-10 12:14:16.928', 556.000000, TRUE);\n"

        f"COMMIT;\n"
    )


@pytest.fixture
def build_defect_pipe_query_with_2_defects():
    obj_id = datetime.now().strftime("object_%Y_%m_%d_%H_%M_%S_%f")
    return (
        f"BEGIN TRANSACTION;\n"

        f"INSERT INTO objects\n"
        f"VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'pipe'), 13, '{obj_id}');\n"

        f"INSERT INTO pipes(id, object_id, last_detection, diameter, CONDITION)\n"
        f"VALUES (DEFAULT, (SELECT max(id) AS max_object_id FROM objects o), '2021-08-10 12:14:16.928', 556.000000, FALSE);\n"

        f"INSERT INTO pipes_defects(id, pipe_id, defect_id, POSITION, SIZE, depth, PRECISION, image_url)\n"
        f"VALUES (DEFAULT, (SELECT max(id) AS max_mandrel_id FROM pipes m), 2, 6273.000000, 470, 7.000000, -1.000000, '/home/vault/picture_arc');\n"

        f"INSERT INTO pipes_defects(id, pipe_id, defect_id, POSITION, SIZE, depth, PRECISION, image_url)\n"
        f"VALUES (DEFAULT, (SELECT max(id) AS max_mandrel_id FROM pipes m), 2, 6273.000000, 470, 7.000000, -1.000000, '/home/vault/picture_arc');\n"

        f"COMMIT;\n"
    )


@pytest.fixture
def build_defect_mandrel_query_with_2_defects():
    obj_id = datetime.now().strftime("object_%Y_%m_%d_%H_%M_%S_%f")
    return (
        f"BEGIN TRANSACTION;\n"

        f"INSERT INTO objects\n"
        f"VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'mandrel'), 13, '{obj_id}');\n"

        f"INSERT INTO mandrels(id, object_id, last_detection, diameter, CONDITION)\n"
        f"VALUES (DEFAULT, (SELECT max(id) AS max_object_id FROM objects o), '2021-08-10 12:14:16.928', 556.000000, FALSE);\n"

        f"INSERT INTO mandrels_defects(id, detected_timestamp, mandrel_id, defect_id, POSITION, SIZE, depth, PRECISION, image_url)\n"
        f"VALUES (DEFAULT, '2021-08-10 12:14:16.928', (SELECT max(id) AS max_mandrel_id FROM mandrels m), 2, 6273.000000, 470, 7.000000, -1.000000, '/home/vault/picture_arc');\n"

        f"INSERT INTO mandrels_defects(id, detected_timestamp, mandrel_id, defect_id, POSITION, SIZE, depth, PRECISION, image_url)\n"
        f"VALUES (DEFAULT, '2021-08-10 12:14:16.928', (SELECT max(id) AS max_mandrel_id FROM mandrels m), 2, 6273.000000, 470, 7.000000, -1.000000, '/home/vault/picture_arc');\n"

        f"COMMIT;\n"
    )


@pytest.fixture
def build_valid_billet_query():
    obj_id = datetime.now().strftime("object_%Y_%m_%d_%H_%M_%S_%f")
    return (
        f"BEGIN TRANSACTION;\n"

        f"INSERT INTO objects\n"
        f"VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'billet'), 13, '{obj_id}');\n"

        f"INSERT INTO billets(id, object_id, last_detection, diameter1, diameter2, diameter3, length, CONDITION)\n"
        f"VALUES (DEFAULT, (SELECT max(id) AS max_object_id FROM objects o), '2021-08-10 12:14:16.928', 556.000000, 556.000000, 556.000000, 4444234, TRUE);\n"

        f"COMMIT;\n"
    )


@pytest.fixture
def build_bad_billet_query():
    obj_id = datetime.now().strftime("object_%Y_%m_%d_%H_%M_%S_%f")
    return (
        f"BEGIN TRANSACTION;\n"

        f"INSERT INTO objects\n"
        f"VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'billet'), 13, '{obj_id}');\n"

        f"COMMIT;\n"
    )


@pytest.fixture
def build_defect_billet_query():
    obj_id = datetime.now().strftime("object_%Y_%m_%d_%H_%M_%S_%f")
    return (
        f"BEGIN TRANSACTION;\n"

        f"INSERT INTO objects\n"
        f"VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'billet'), 13, '{obj_id}');\n"

        f"INSERT INTO billets(id, object_id, last_detection, diameter1, diameter2, diameter3, length, CONDITION)\n"
        f"VALUES (DEFAULT, (SELECT max(id) AS max_object_id FROM objects o), '2021-08-10 12:14:16.928', 556.000000, 556.000000, 556.000000, 4444234, FALSE);\n"

        f"COMMIT;\n"
    )