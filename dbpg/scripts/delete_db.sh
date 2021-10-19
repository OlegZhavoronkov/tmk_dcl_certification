#!/bin/bash

################################################
#  Delete database and tables for TMK project
################################################

database=tmk_db

psql -c "DROP DATABASE ${database};"
