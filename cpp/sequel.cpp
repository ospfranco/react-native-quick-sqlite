/*
 * sequel.cpp
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the MIT license
 */

#include "sequel.h"
#include <sstream>
#include <iostream>
#include <sqlite3.h>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <map>

using namespace std;
using namespace facebook;

map<string, sqlite3 *> dbMap = map<string, sqlite3 *>();

bool folder_exists(const std::string &foldername)
{
  struct stat buffer;
  return (stat(foldername.c_str(), &buffer) == 0);
}

/**
     * Portable wrapper for mkdir. Internally used by mkdir()
     * @param[in] path the full path of the directory to create.
     * @return zero on success, otherwise -1.
     */
int _mkdir(const char *path)
{
#if _POSIX_C_SOURCE
  return mkdir(path);
#else
  return mkdir(path, 0755); // not sure if this works on mac
#endif
}

/**
     * Recursive, portable wrapper for mkdir.
     * @param[in] path the full path of the directory to create.
     * @return zero on success, otherwise -1.
     */
int mkdir(const char *path)
{
  string current_level = "/";
  string level;
  stringstream ss(path);
  // First line is empty because it starts with /User
  getline(ss, level, '/');
  // split path using slash as a separator
  while (getline(ss, level, '/'))
  {
    current_level += level; // append folder to the current level
    // create current level
    if (!folder_exists(current_level) && _mkdir(current_level.c_str()) != 0)
      return -1;

    current_level += "/"; // don't forget to append a slash
  }

  return 0;
}

inline bool file_exists(const string &path)
{
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

string get_db_path(string const dbName, string const docPath)
{
  mkdir(docPath.c_str());
  return docPath + "/" + dbName;
}

SequelResult sequel_open(string const dbName, string const docPath)
{
  string dbPath = get_db_path(dbName, docPath);

  int sqlOpenFlags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

  sqlite3 *db;
  int exit = 0;
  exit = sqlite3_open_v2(dbPath.c_str(), &db, sqlOpenFlags, nullptr);

  if (exit != SQLITE_OK)
  {
    return SequelResult{
        SequelResultError,
        sqlite3_errmsg(db),
        jsi::Value::undefined()};
  }
  else
  {
    dbMap[dbName] = db;
  }

  return SequelResult{
      SequelResultOk,
      "",
      jsi::Value::undefined()};
}

SequelResult sequel_close(string const dbName)
{

  if (dbMap.count(dbName) == 0)
  {
    // cout << "[react-native-quick-sqlite]: DB " << dbName << " not open" << endl;
    return SequelResult{
        SequelResultError,
        dbName + " is not open",
        jsi::Value::undefined()};
  }

  sqlite3 *db = dbMap[dbName];

  sqlite3_close(db);

  dbMap.erase(dbName);

  return SequelResult{
      SequelResultOk,
      "",
      jsi::Value::undefined()};
}

//SequelResult sequel_attach(string const &dbName)
//{
//    if(dbMap.count(dbName) == 0){
//        cout << "[react-native-quick-sqlite]: DB " << dbName << " not open" << endl;
//        return SequelResult{
//            SequelResultError,
//            dbName + " is not open",
//            jsi::Value::undefined()
//        };
//    }

// TODO: What does "Attach" do? is it really necessary?
//https://github.com/andpor/react-native-sqlite-storage/blob/master/platforms/ios/SQLite.m#L362

//    NSString* sql = [NSString stringWithFormat:@"ATTACH DATABASE '%@' AS %@", dbPathToAttach, dbAlias];
//
//            if(sqlite3_exec(db, [sql UTF8String], NULL, NULL, NULL) == SQLITE_OK) {
//              pluginResult = [SQLiteResult resultWithStatus:SQLiteStatus_OK messageAsString:@"Database attached successfully."];
//            } else {
//              pluginResult = [SQLiteResult resultWithStatus:SQLiteStatus_ERROR messageAsString:@"Unable to attach DB"];
//            }

//}

SequelResult sequel_remove(string const dbName, string const docPath)
{
  if (dbMap.count(dbName) == 1)
  {
    SequelResult closeResult = sequel_close(dbName);
    if (closeResult.type == SequelResultError)
    {
      return closeResult;
    }
  }

  string dbPath = get_db_path(dbName, docPath);

  if (!file_exists(dbPath))
  {
    return SequelResult{
        SequelResultError,
        "[react-native-quick-sqlite]: Database file not found" + dbPath};
  }

  remove(dbPath.c_str());

  return SequelResult{
      SequelResultOk,
      "",
      jsi::Value::undefined()};
}

void bindStatement(sqlite3_stmt *statement, jsi::Runtime &rt, jsi::Value const &params)
{
  if (params.isNull() || params.isUndefined())
  {
    return;
  }

  jsi::Array values = params.asObject(rt).asArray(rt);

  for (int ii = 0; ii < values.length(rt); ii++)
  {

    jsi::Value value = values.getValueAtIndex(rt, ii);
    if (value.isNull())
    {
      sqlite3_bind_null(statement, ii + 1);
    }
    else if (value.isBool())
    {
      int intVal = int(value.getBool());
      sqlite3_bind_int(statement, ii + 1, intVal);
    }
    else if (value.isNumber())
    {
      double doubleVal = value.asNumber();
      int intVal = (int)doubleVal;
      long long longVal = (long)doubleVal;
      if (intVal == doubleVal)
      {
        sqlite3_bind_int(statement, ii + 1, intVal);
      }
      else if (longVal == doubleVal)
      {
        sqlite3_bind_int64(statement, ii + 1, longVal);
      }
      else
      {
        sqlite3_bind_double(statement, ii + 1, doubleVal);
      }
    }
    else if (value.isString())
    {
      string strVal = value.asString(rt).utf8(rt);

      sqlite3_bind_text(statement, ii + 1, strVal.c_str(), strVal.length(), SQLITE_TRANSIENT);
    }
  }
}

SequelResult sequel_execute(jsi::Runtime &rt, string const dbName, string const &query, jsi::Value const &params)
{
  // Check if db connection is opened
  if (dbMap.count(dbName) == 0)
  {
    return SequelResult{
        SequelResultError,
        "[react-native-quick-sqlite]: Database " + dbName + " is not open"};
  }

  sqlite3 *db = dbMap[dbName];

  vector<jsi::Object> results;

  // SQLite statements need to be compiled before executed
  sqlite3_stmt *statement;

  // Compile and move result into statement memory spot
  int statementStatus = sqlite3_prepare_v2(db, query.c_str(), -1, &statement, NULL);

  if (statementStatus == SQLITE_OK) // statemnet is correct, bind the passed parameters
  {
    bindStatement(statement, rt, params);
  }
  else
  {
    const char *message = sqlite3_errmsg(db);

    return {
        SequelResultError,
        "[react-native-quick-sqlite] SQL execution error: " + string(message),
        jsi::Value::undefined()};
  }

  bool isConsuming = true;
  bool isFailed = false;

  int result, i, count, column_type;
  string column_name;

  while (isConsuming)
  {
    result = sqlite3_step(statement);
    jsi::Object entry = jsi::Object(rt);

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
          entry.setProperty(rt, column_name.c_str(), jsi::Value(column_value));
          break;
        }

        case SQLITE_FLOAT:
        {
          double column_value = sqlite3_column_double(statement, i);
          entry.setProperty(rt, column_name.c_str(), jsi::Value(column_value));
          break;
        }

        case SQLITE_TEXT:
        {
          const char *column_value = reinterpret_cast<const char *>(sqlite3_column_text(statement, i));
          entry.setProperty(rt, column_name.c_str(), jsi::String::createFromUtf8(rt, column_value));
          break;
        }

        case SQLITE_NULL:
        // Intentionally left blank to switch to default case
        default:
          entry.setProperty(rt, column_name.c_str(), jsi::Value(nullptr));
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
      isFailed = true;
      isConsuming = false;
    }
  }

  sqlite3_finalize(statement);

  if (isFailed)
  {
    const char *message = sqlite3_errmsg(db);
    return {
        SequelResultError,
        "[react-native-quick-sqlite] SQL execution error: " + string(message),
        jsi::Value::undefined()};
  }

  // Move everything into a JSI object
  auto array = jsi::Array(rt, results.size());
  for (int i = 0; i < results.size(); i++)
  {
    array.setValueAtIndex(rt, i, move(results[i]));
  }

  jsi::Object rows = jsi::Object(rt);
  rows.setProperty(rt, "length", jsi::Value((int)results.size()));
  rows.setProperty(rt, "_array", move(array));

  //    For any future endaevors, I tried to create the accesor function directly on via JSI
  //      But this is too complex for my punny brain, so this function is created on the index.ts file
  //  // Create accessor function
  //  auto itemAccesser = jsi::Function::createFromHostFunction(
  //                          rt,
  //                          jsi::PropNameID::forAscii(rt, "item"),
  //                          1,
  //                          [&array](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
  //                          {
  //      if(args[0].isNumber()) {
  //          double rowNumber = args[0].asNumber();
  //          cout << "trying to access value at" << rowNumber << endl;
  //          return array.getValueAtIndex(rt, (int)rowNumber);
  //      }
  //
  //      return {};
  //  });

  //  rows.setProperty(rt, "item", move(itemAccesser));

  jsi::Object res = jsi::Object(rt);
  res.setProperty(rt, "rows", move(rows));

  int changedRowCount = sqlite3_total_changes(db);
  res.setProperty(rt, "rowsAffected", jsi::Value(changedRowCount));

  // row id has nothing to do with the actual uuid/id of the object, but internal row count
  long long latestInsertRowId = sqlite3_last_insert_rowid(db);
  if (changedRowCount > 0 && latestInsertRowId != 0)
  {
    res.setProperty(rt, "insertId", jsi::Value((int)latestInsertRowId));
  }

  return {
      SequelResultOk,
      "",
      move(res)};
}
