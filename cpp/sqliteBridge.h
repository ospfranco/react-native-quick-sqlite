/*
 * sequel.h
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the MIT license
 */

#include "JSIHelper.h"
#include <vector>

using namespace std;
using namespace facebook;
using namespace jsi;

SQLiteOPResult sqliteOpenDb(string const dbName, string const docPath);

SQLiteOPResult sqliteCloseDb(string const dbName);

SQLiteOPResult sqliteRemoveDb(string const dbName, string const docPath);

SQLiteOPResult sqliteAttachDb(string const mainDBName, string const docPath, string const databaseToAttach, string const alias);

SQLiteOPResult sqliteDetachDb(string const mainDBName, string const alias);

SQLiteOPResult sqliteExecute(string const dbName, string const &query, vector<QuickValue> *values, vector<map<string, QuickValue>> *result, vector<QuickColumnMetadata> *metadata);

SequelLiteralUpdateResult sqliteExecuteLiteral(string const dbName, string const &query);

SQLiteFunctionResult sqliteCustomFunction(
                                          Runtime& rt,
                                          const string dbName,
                                          const string name,
                                          const int nArgs,
                                          const bool DETERMINISTIC,
                                          const bool DIRECTONLY,
                                          const bool INNOCUOUS,
                                          const bool SUBTYPE,
                                          const shared_ptr<Function> callback);

SQLiteFunctionResult sqliteCustomAggregate(
                                           Runtime& rt,
                                           const string dbName,
                                           const string name,
                                           const int nArgs,
                                           const bool DETERMINISTIC,
                                           const bool DIRECTONLY,
                                           const bool INNOCUOUS,
                                           const bool SUBTYPE,
                                           const shared_ptr<Function> step,
                                           const bool startIsFunction,
                                           const bool inverseIsFunction,
                                           const bool resultIsFunction,
                                           const shared_ptr<Function> start,
                                           const shared_ptr<Function> inverse,
                                           const shared_ptr<Function> result
                                        );
