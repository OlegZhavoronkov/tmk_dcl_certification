#!/bin/bash

################################################
#  Create database and tables for TMK project
################################################

database=tmk_db

psql -c "CREATE DATABASE ${database};"
psql -d $database -f tmk_db.sql

psql -d $database -v db=$database -v user='"tmk_client_app"'  -f client_privileges.sql

psql -d $database -f notify_trigger.sql
psql -d $database -v tableName="pipe_defects" -v triggerName="pipe_defects_trigger" -f table_trigger.sql
psql -d $database -v tableName="mandrel_defects" -v triggerName="mandrel_defects_trigger" -f table_trigger.sql
psql -d $database -v tableName="billets" -v triggerName="billets_trigger" -f table_trigger.sql



#psql -d $database -c "INSERT INTO mandrels VALUES ( DEFAULT, '123', 50, current_timestamp, false, 'good condition')"


