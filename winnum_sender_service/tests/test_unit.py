import pytest

from sent_to_winnum import ObjectOfInterest, ObjectOfInterestException, WinnumLogger, Mapper, Config


class TestUnit:
    def test_enum_object_of_interests(self):
        result_not_present = ObjectOfInterest.has_not_name('Blabla')
        assert result_not_present
        result_present = ObjectOfInterest.has_not_name('mandrel')
        assert not result_present

    def test_object_of_interest_exception(self):
        with pytest.raises(ObjectOfInterestException) as excinfo:
            raise ObjectOfInterestException
        assert str(excinfo.value) == ''

    def test_winnum_logger(self):
        WinnumLogger.init_logger()
        WinnumLogger.get_logger()
        assert str(type(WinnumLogger.logger)) == "<class 'logging.Logger'>"

    def test_mapper(self):
        Mapper.load('test_data/mapping.json')
        assert (Mapper.billet, Mapper.pipe, Mapper.billet) != (None, None, None)

    def test_config(self):
        Config.load('test_data/config.json')
        assert (
        Config.name,
        Config.postgresql_host,
        Config.dbname,
        Config.postgrest_address,
        Config.winnum_address,
        Config.user,
        Config.password,
        Config.channel,
        Config.device,
        Config.key,
        ) != (
            None,
            None,
            None,
            None,
            None,
            None,
            None,
            None,
            None,
            None,
        )
