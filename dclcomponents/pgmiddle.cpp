#include "processor/dclprocessor_old.h"
#include <storage/ObjectDescriptor.h>

#include <glog/logging.h>

#include <iostream>
#include <string>
#include <pqxx/pqxx>

using namespace tmk;
using namespace tmk::storage;

pqxx::result insert_query(char *obj_name) {
    std::string connection_string("host=79.173.96.138 port=5432 dbname=test_db user=tmk password=tmkdb");
    pqxx::connection conn(connection_string.c_str());
    pqxx::work transaction(conn);

    pqxx::result r {
        transaction.exec("INSERT INTO objects VALUES (DEFAULT, (SELECT object_types.id FROM object_types WHERE object_types.name = 'mandrel'), '----'," + transaction.quote(obj_name) + ");")
    };

    transaction.commit();
    return r;
}

pqxx::result select_query() {
    std::string connection_string("host=79.173.96.138 port=5432 dbname=test_db user=tmk password=tmkdb");
    pqxx::connection conn(connection_string.c_str());
    pqxx::work transaction(conn);

    pqxx::result r {
        transaction.exec("SELECT * FROM objects;")
    };
    if (r.size() < 1) {
        std::cout << "No data" << std::endl;
    }

    std::cout << "id\tobj_number\t\tobj_generated_id" << std::endl;
    for (auto row: r) {
        std::cout << row[0].c_str() << "\t" << row[2].c_str() << "\t\t" << row[3].c_str() << std::endl;
    }
    fflush(stdout);

    transaction.commit();
    return r;
}

pqxx::result select_count_query() {
    std::string connection_string("host=79.173.96.138 port=5432 dbname=test_db user=tmk password=tmkdb");
    pqxx::connection conn(connection_string.c_str());
    pqxx::work transaction(conn);

    pqxx::result r {
        transaction.exec("SELECT count(*) FROM objects;")
    };
    if (r.size() < 1) {
        std::cout << "No data" << std::endl;
    }

    for (auto row: r) {
        std::cout << row[0] << std::endl;
    }
    fflush(stdout);

    transaction.commit();
    return r;
}


void WorkWithObject(ObjectDescriptor &obj, char *boost_path, char *obj_name) {
    std::cout << "[pg]: Hello, object " << obj_name << " from " << boost_path << std::endl;
    fflush(stdout);
    try {
        pqxx::result ri{insert_query(obj_name)};
        pqxx::result rs{select_count_query()};
#if 0
        for (auto row: rs) {
            std::cout << "[pg]: Row: ";
            for (auto field: row) std::cout << field.c_str() << " ";
            std::cout << std::endl;
            fflush(stdout);
        }
#endif
    }
    catch (pqxx::sql_error const &e) {
        std::cerr << "[pg]: SQL error: " << e.what() << std::endl;
        std::cerr << "[pg]: Query was: " << e.query() << std::endl;
        fflush(stderr);
        return;
    }
    catch (std::exception const &e) {
        std::cerr << "[pg]: Error: " << e.what() << std::endl;
        fflush(stderr);
        return;
    }
}

int main(int argc, char *argv[]){
    connect_to_pipeline(PC_MIDDLE, NULL, WorkWithObject);
    return 0;
}

