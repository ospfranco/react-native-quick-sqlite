#include "sequel.h"
#include <iostream>
#include <sstream>
#include <sqlite3.h>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;
using namespace facebook;

inline bool file_exists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

string get_db_path(const string &dbName)
{
    char *home = getenv("HOME");
    char const *subdir = "/Documents/";

    stringstream ss;
    ss << home << subdir << dbName;
    string dbPath = ss.str();

    return dbPath;
}

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
    cout << "[react-native-sequel] Opening DB" << endl;

    string dbPath = get_db_path(dbName);
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

    return true;
}

bool sequel_close(string const &dbName)
{
    cout << "[react-native-sequel] Closing DB" << endl;

    if(db == NULL) {
        cout << "[react-native-sequel] No DB open" << endl;
        return true;
    }

    sqlite3_close(db);

    return true;
}

bool sequel_delete(string const &dbName)
{
    cout << "[react-native-sequel] Deleting DB" << endl;

    string dbPath = get_db_path(dbName);

    if(file_exists(dbPath)) {
        remove(dbPath.c_str());
        cout << "[react-native-sequel] DB at " << dbPath << "has been deleted." << endl;
    } else {
        cout << "[react-native-sequel] File not found" << endl;
        return false;
    }


    return true;
}

std::vector<jsi::Object> sequel_execute(jsi::Runtime &runtime, string const &query)
{
    // this time we will first compile the SQL
    sqlite3_stmt *statement;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &statement, NULL) != SQLITE_OK)
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

    return results;
}
