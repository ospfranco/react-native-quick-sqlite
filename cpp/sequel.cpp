#include "sequel.h"
#include <iostream>
#include <sstream>
#include <sqlite3.h>
#include <ctime>
#include <unistd.h>

using namespace std;
using namespace facebook;

string gen_random(const int len)
{

    string tmp_s;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    srand((unsigned)time(NULL) * getpid());

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];

    return tmp_s;
}

sqlite3 *db;

bool sequel_open(string const &dbName)
{
    cout << "react-native-sequel: Trying to open db connection" << endl;
    char *home = getenv("HOME");
    char *subdir = "/Documents/";

    stringstream ss;
    ss << home << subdir << dbName;
    string dbPath = ss.str();

    // s.erase(0, strlen("/private")); // Erase “/Private” from the final string

    int sqlOpenFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

    int exit = 0;
    exit = sqlite3_open_v2(dbPath.c_str(), &db, sqlOpenFlags, nullptr);

    if (exit != SQLITE_OK)
    {
        cout << "react-native-sequel: Error opening database [" << dbName << "]: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    else
    {
        cout << "Opened database successfully!" << endl;
    }

    sqlite3_close(db);
    return true;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    for (int ii = 0; ii < argc; ii++)
    {
        cout << azColName[ii] << ": " << argv[ii] << endl;
    }

    cout << endl;

    return 0;
}

std::vector<jsi::Object> sequel_init(jsi::Runtime &runtime)
{
    cout << "----sequel---: INIT" << endl;

    char *home = getenv("HOME");
    char *subdir = "/Documents/";

    // Save any error messages
    char *zErrMsg = 0;

    stringstream ss;
    ss << home << subdir << "sample.sqlite";
    string dbPath = ss.str();

    int sqlOpenFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

    int rc;
    rc = sqlite3_open_v2(dbPath.c_str(), &db, sqlOpenFlags, nullptr);

    // if(rc != SQLITE_OK) {
    //   cout << "react-native-sequel: Error opening database [sample.sqlite]: " << sqlite3_errmsg(db) << endl;
    //   return;
    // }

    cout << "Opened database successfully!" << endl;

    string sql = "DROP TABLE PEOPLE;";
    //
    //  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    //
    //  cout << "dropped table PEOPLE" << endl;

    //  sql = "CREATE TABLE PEOPLE ("  \
//      "ID TEXT PRIMARY KEY     NOT NULL," \
//      "NAME           TEXT    NOT NULL);";
    //
    //  rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    //
    //  cout << "Created table: PEOPLE" << endl;

    //  for(int ii = 0; ii < 10000; ii++) {
    //    sql = "INSERT INTO PEOPLE ('ID', 'NAME') VALUES ('" + to_string(ii) + "', '" + gen_random(12) + "')";
    //
    //    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    //  }

    sql = "SELECT * FROM 'PEOPLE';";

    // this time we will first compile the SQL
    sqlite3_stmt *statement;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, NULL) != SQLITE_OK)
    {
        cout << "INVALID SQL QUERY" << endl;
    }

    bool isConsuming = true;
    int result, i, count, column_type;
    string column_name;
    
    vector<jsi::Object> results;

    while (isConsuming)
    {
        result = sqlite3_step(statement);
        jsi::Object entry = jsi::Object(runtime);

        switch (result)
        {
        case SQLITE_ROW:
            i = 0;
            count = sqlite3_column_count(statement);

            while (i < count)
            {
                column_type = sqlite3_column_type(statement, i);
                column_name = sqlite3_column_name(statement, i);

                switch (column_type)
                {
                
                case SQLITE_INTEGER:
                {
                    int column_value = sqlite3_column_int(statement, i);
                    entry.setProperty(runtime, column_name.c_str(), jsi::Value(column_value));
                    break;
                }

                case SQLITE_FLOAT:
                {
                    double column_value = sqlite3_column_double(statement, i);
                    entry.setProperty(runtime, column_name.c_str(), jsi::Value(column_value));
                    break;
                }

                case SQLITE_TEXT:
                {
                    // TODO: not all the stored text is ASCII, replace this for UTF 8
                    const char *column_value = reinterpret_cast<const char *>(sqlite3_column_text(statement, i));
                    entry.setProperty(runtime, column_name.c_str(), jsi::String::createFromAscii(runtime, column_value));
                    break;
                }

                case SQLITE_NULL:
                // Intentionally left blank to switch to default case
                default:
                    entry.setProperty(runtime, column_name.c_str(), jsi::Value(nullptr));
                    break;
                }

                i++;
            }

            results.push_back(move(entry));
            break;

        case SQLITE_DONE:
            isConsuming = false;
            break;

        default:
            cout << "react-native-sequel: Error executing query" << endl;
            isConsuming = false;
        }
    }

    sqlite3_finalize(statement);

    sqlite3_close(db);

    return results;
}

void sequel_execute(string const &query)
{
    // TODO
}
