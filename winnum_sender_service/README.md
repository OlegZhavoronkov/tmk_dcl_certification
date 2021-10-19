### About
Service for listen new records in DB and send it to winnum as signals
### Install dependencies and venv
```
# If venv package not installed:
$ sudo apt install python3-venv

# In directory with sources enter
$ python3 -m venv venv
$ source venv/bin/activate

# Install dependencies
$ pip install -r requirements.txt
# or for test running
$ pip install -r requirements_dev.txt
```
### Deactivating venv
```
$ source venv/bin/deactivate
```
# ALL NEXT THINGS MUST BE RUN IN VENV!!!
### Run service
```
$ python sent_to_winnum.py
```
### Run unit tests
Must be run from winnum_sender_service/tests directory
```
$ cd tests
$ python -v test_unit.py
```
### How to check service?
In tests/test_db_inserter.py located queries for inserting fake data to DB in many variants
When sent_to_winnum.py service running try to run some of this commands from winnum_sender_service directory:
```
python -m pytest -v tests/test_db_inserter.py::TestDbMandrelInserter::test_insert_mandrel_with_2_defects
python -m pytest -v tests/test_db_inserter.py::TestDbMandrelInserter::test_insert_bad_mandrel
python -m pytest -v tests/test_db_inserter.py::TestDbMandrelInserter::test_insert_mandrel_with_no_defects
python -m pytest -v tests/test_db_inserter.py::TestDbPipeInserter::test_insert_bad_pipe
python -m pytest -v tests/test_db_inserter.py::TestDbPipeInserter::test_insert_pipe_with_2_defects
python -m pytest -v tests/test_db_inserter.py::TestDbPipeInserter::test_insert_pipe_with_no_defects
python -m pytest -v tests/test_db_inserter.py::TestDbBilletInserter::test_insert_bad_billet
python -m pytest -v tests/test_db_inserter.py::TestDbBilletInserter::test_insert_billet_with_defect
python -m pytest -v tests/test_db_inserter.py::TestDbBilletInserter::test_insert_billet_with_no_defects
```
