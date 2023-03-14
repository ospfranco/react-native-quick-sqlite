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

SQLiteOPResult sqliteOpenDb(string const dbName, string const docPath);

SQLiteOPResult sqliteCloseDb(string const dbName);

SQLiteOPResult sqliteRemoveDb(string const dbName, string const docPath);

SQLiteOPResult sqliteAttachDb(string const mainDBName, string const docPath, string const databaseToAttach, string const alias);

SQLiteOPResult sqliteDetachDb(string const mainDBName, string const alias);

SQLiteOPResult sqliteExecute(string const dbName, string const &query, vector<QuickValue> *values, vector<map<string, QuickValue>> *result, vector<QuickColumnMetadata> *metadata);

SequelLiteralUpdateResult sqliteExecuteLiteral(string const dbName, string const &query);

SQLiteFunctionResult sqliteCustomFunction(
                                          jsi::Runtime& rt,
                                          const string dbName,
                                          const string name,
                                          const int nArgs,
                                          const bool DETERMINISTIC,
                                          const bool DIRECTONLY,
                                          const bool INNOCUOUS,
                                          const bool SUBTYPE,
                                          const std::shared_ptr<jsi::Function> callback);

SQLiteFunctionResult sqliteCustomAggregate(
                                           jsi::Runtime& rt,
                                           const string dbName,
                                           const string name,
                                           const int nArgs,
                                           const bool DETERMINISTIC,
                                           const bool DIRECTONLY,
                                           const bool INNOCUOUS,
                                           const bool SUBTYPE,
                                           const std::shared_ptr<jsi::Function> step,
                                           const bool startIsFunction,
                                           const bool inverseIsFunction,
                                           const bool resultIsFunction,
                                           const std::shared_ptr<jsi::Function> start,
                                           const std::shared_ptr<jsi::Function> inverse,
                                           const std::shared_ptr<jsi::Function> result
                                        );
