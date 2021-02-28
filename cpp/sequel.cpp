#include <iostream>
#include <sstream>
#include <sqlite3.h>

using namespace std;

sqlite3 *db;

bool sequel_open(string const& dbName) {
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

  if(exit != SQLITE_OK) {
    cout << "react-native-sequel: Error opening database [" << dbName << "]: " << sqlite3_errmsg(db) << endl;
    return false;
  } else {
    cout << "Opened database successfully!" << endl;
  }

  sqlite3_close(db);
  return true;
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  return 0;
}

void sequel_init() {
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

  if(rc != SQLITE_OK) {
    cout << "react-native-sequel: Error opening database [sample.sqlite]: " << sqlite3_errmsg(db) << endl;
    return;
  }
   
  cout << "Opened database successfully!" << endl;
  
  string createTableSQL = "CREATE TABLE PEOPLE ("  \
      "ID INT PRIMARY KEY     NOT NULL," \
      "NAME           TEXT    NOT NULL);";

  rc = sqlite3_exec(db, createTableSQL.c_str(), callback, 0, &zErrMsg);
  
  cout << "Created table: PEOPLE" << endl;

  sqlite3_close(db);

  return;
}

void sequel_execute(string const& query) {
  
}