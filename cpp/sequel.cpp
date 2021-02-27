#include <iostream>
#include <sstream>
#include <sqlite3.h>

using namespace std;

bool sequel_open(string const& dbName) {
  cout << "react-native-sequel: Trying to open db connection" << endl;
  char *home = getenv("HOME");
  char *subdir = "/Documents/";
  
  stringstream ss;
  ss << home << subdir << dbName;
  string dbPath = ss.str();

  // s.erase(0, strlen("/private")); // Erase “/Private” from the final string

  int sqlOpenFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

  sqlite3 *db;

  int exit = 0;
  exit = sqlite3_open_v2(dbPath.c_str(), &db, sqlOpenFlags, nullptr);

  if(exit != SQLITE_OK) {
    cout << "react-native-sequel: Error opening database [" << dbName << "]: " << sqlite3_errmsg(db) << endl;
    return false;
  } else {
    cout << "Opened database successfully!" << endl;
  }

  sqlite3_close(db);
  return true;
}