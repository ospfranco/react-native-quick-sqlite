#include <iostream>
#include <sqlite3.h>

using namespace std;

bool sequel_open(string const& dbName) {
  cout << "Trying to open db connection" << endl;
  int sqlOpenFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

  sqlite3 *db;

  int exit = 0;
  exit = sqlite3_open_v2(dbName.c_str(), &db, sqlOpenFlags, nullptr);

  if(exit != SQLITE_OK) {
    cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
    return false;
  } else {
    cout << "Opened database successfully!" << endl;
  }

  sqlite3_close(db);
  return true;
}