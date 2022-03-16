/*
 * react-native-quick-sqlite.cpp
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the MIT license
 */

#include "installer.h"
#include "sqliteBridge.h"
#include "logs.h"
#include "JSIHelper.h"
#include "ThreadPool.h"
#include "sqlfileloader.h"
#include "sqlbatchexecutor.h"
#include <vector>
#include <string>

using namespace std;
using namespace facebook;

string docPathStr;
std::shared_ptr<react::CallInvoker> invoker;

jsi::Object createError(jsi::Runtime &rt, string message)
{
  auto res = jsi::Object(rt);
  res.setProperty(rt, "status", jsi::Value(1));
  res.setProperty(rt, "message", jsi::String::createFromUtf8(rt, message));
  return res;
}

jsi::Object createOk(jsi::Runtime &rt)
{
  auto res = jsi::Object(rt);
  res.setProperty(rt, "status", jsi::Value(0));
  return res;
}

void install(jsi::Runtime &rt, std::shared_ptr<react::CallInvoker> jsCallInvoker, const char *docPath)
{

  // Transfer from pointer to variable to prevent de-allocation once calling function has finished
  docPathStr = std::string(docPath);
  auto pool = std::make_shared<ThreadPool>();
  invoker = jsCallInvoker;

  // Open/create db
  auto open = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_open"),
      1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
      {
        if (count == 0)
        {
          return createError(rt, "[react-native-quick-sqlite][open] database name is required");
        }

        if (!args[0].isString())
        {
          return createError(rt, "[react-native-quick-sqlite][open] database name must be a string");
        }

        string dbName = args[0].asString(rt).utf8(rt);
        string tempDocPath = string(docPathStr);
        if (count > 1)
        {
          if (!args[1].isString())
          {
            return createError(rt, "[react-native-quick-sqlite][open] database location must be a string");
          }

          tempDocPath = tempDocPath + "/" + args[1].asString(rt).utf8(rt);
        }

        SQLiteOPResult result = sqliteOpenDb(dbName, tempDocPath);

        if (result.type == SQLiteError)
        {
          return createError(rt, result.errorMessage.c_str());
        }

        return createOk(rt);
      });

  //    auto attach = jsi::Function::createFromHostFunction(
  //        rt,
  //        jsi::PropNameID::forAscii(rt, "sequel_attach"),
  //        1,
  //        [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
  //            if (!args[0].isString())
  //            {
  //                jsi::detail::throwJSError(rt, "dbName must be a string");
  //                return {};
  //            }
  //
  //            string dbName = args[0].asString(rt).utf8(rt);
  //            SequelResult result = sequel_attach(dbName);
  //
  //            if (result.type == SQLiteError)
  //            {
  //                jsi::detail::throwJSError(rt, result.message.c_str());
  //                return {};
  //            }
  //
  //            return move(result.value);
  //        });

  // Close db
  auto close = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_close"),
      1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
      {
        if (count == 0)
        {
          return createError(rt, "[react-native-quick-sqlite][close] database name is required");
        }

        if (!args[0].isString())
        {
          return createError(rt, "[react-native-quick-sqlite][close] database name must be a string");
        }

        string dbName = args[0].asString(rt).utf8(rt);

        SQLiteOPResult result = sqliteCloseDb(dbName);

        if (result.type == SQLiteError)
        {
          return createError(rt, result.errorMessage.c_str());
        }

        return createOk(rt);
      });

  // Delete db
  auto remove = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_delete"),
      1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
      {
        if (count == 0)
        {
          return createError(rt, "[react-native-quick-sqlite][open] database name is required");
        }

        if (!args[0].isString())
        {
          return createError(rt, "[react-native-quick-sqlite][open] database name must be a string");
        }

        string dbName = args[0].asString(rt).utf8(rt);

        SQLiteOPResult result = sqliteRemoveDb(dbName, docPathStr);

        if (result.type == SQLiteError)
        {
          return createError(rt, result.errorMessage.c_str());
        }

        return jsi::Value::undefined();
      });

  // Execute SQL query
  auto execSQL = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_execSQL"),
      3,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
      {
        const string dbName = args[0].asString(rt).utf8(rt);
        const string query = args[1].asString(rt).utf8(rt);
        const jsi::Value &originalParams = args[2];
        // Converting parameters
        vector<QuickValue> params;
        jsiQueryArgumentsToSequelParam(rt, originalParams, &params);

        // Filling the results
        vector<map<string, QuickValue>> results;
        auto status = sqliteExecute(dbName, query, &params, &results);

        // Converting results into a JSI Response
        auto jsiResult = createSequelQueryExecutionResult(rt, status, &results);
        return move(jsiResult);
      });

  // Execute a batch of SQL queries in a transaction
  // Parameters can be: [[sql: string, arguments: any[] | arguments: any[][] ]]
  auto execSQLBatch = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_execSQLBatch"),
      2,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
      {
        if (sizeof(args) < 2)
        {
          return createError(rt, "[react-native-quick-sqlite][execSQLBatch] - Incorrect parameter count");
        }

        const jsi::Value &params = args[1];
        if (params.isNull() || params.isUndefined())
        {
          return createError(rt, "[react-native-quick-sqlite][execSQLBatch] - An array of SQL commands or parameters is needed");
        }
        const string dbName = args[0].asString(rt).utf8(rt);
        const jsi::Array &batchParams = params.asObject(rt).asArray(rt);
        vector<QuickQueryArguments> commands;
        jsiBatchParametersToQuickArguments(rt, batchParams, &commands);

        auto batchResult = executeBatch(dbName, &commands);
        if (batchResult.type == SQLiteOk)
        {
          auto res = jsi::Object(rt);
          res.setProperty(rt, "status", jsi::Value(0));
          res.setProperty(rt, "rowsAffected", jsi::Value(batchResult.affectedRows));
          return move(res);
        }
        else
        {
          return createError(rt, batchResult.message);
        }
      });

  auto execSQLBatchAsync = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_execSQLBatchAsync"),
      3,
      [pool](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
      {
        if (sizeof(args) < 3)
        {
          jsi::detail::throwJSError(rt, "[react-native-quick-sqlite][asyncExecuteSqlBatch] Incorrect parameter count");
          return {};
        }

        const jsi::Value &params = args[1];
        const jsi::Value &callbackHolder = args[2];
        if (!callbackHolder.isObject() || !callbackHolder.asObject(rt).isFunction(rt))
        {
          jsi::detail::throwJSError(rt, "[react-native-quick-sqlite][asyncExecuteSqlBatch] The callback argument must be a function");
          return {};
        }

        if (params.isNull() || params.isUndefined())
        {
          jsi::detail::throwJSError(rt, "[react-native-quick-sqlite][asyncExecuteSqlBatch] - An array of SQL commands or parameters is needed");
          return {};
        }

        const string dbName = args[0].asString(rt).utf8(rt);
        const jsi::Array &batchParams = params.asObject(rt).asArray(rt);
        auto callback = make_shared<jsi::Value>(callbackHolder.asObject(rt));

        vector<QuickQueryArguments> commands;
        jsiBatchParametersToQuickArguments(rt, batchParams, &commands);

        auto task =
            [&rt, dbName, commands = make_shared<vector<QuickQueryArguments>>(commands), callback]()
        {
          try
          {
            // Inside the new worker thread, we can now call sqlite operations
            auto batchResult = executeBatch(dbName, commands.get());
            invoker->invokeAsync([&rt, batchResult = move(batchResult), callback]
                                 {
              if(batchResult.type == SQLiteOk)
              {
                auto res = jsi::Object(rt);
                res.setProperty(rt, "status", jsi::Value(0));
                res.setProperty(rt, "rowsAffected", jsi::Value(batchResult.affectedRows));
                callback->asObject(rt).asFunction(rt).call(rt, move(res));
              } else
              {
                callback->asObject(rt).asFunction(rt).call(rt, createError(rt, batchResult.message));
              } });
          }
          catch (std::exception &exc)
          {
            invoker->invokeAsync([&rt, callback, &exc]
                                 { callback->asObject(rt).asFunction(rt).call(rt, createError(rt, exc.what())); });
          }
        };
        pool->queueWork(task);
        return {};
      });

  // Load SQL File from disk
  auto loadSQLFile = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_loadSQLFile"),
      2,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
      {
        const string dbName = args[0].asString(rt).utf8(rt);
        const string sqlFileName = args[1].asString(rt).utf8(rt);

        const auto importResult = importSQLFile(dbName, sqlFileName);
        if (importResult.type == SQLiteOk)
        {
          auto res = jsi::Object(rt);
          res.setProperty(rt, "status", jsi::Value(0));
          res.setProperty(rt, "rowsAffected", jsi::Value(importResult.affectedRows));
          res.setProperty(rt, "commands", jsi::Value(importResult.commands));
          return move(res);
        }
        else
        {
          return createError(rt, "[react-native-quick-sqlite][loadSQLFile] Could not open file");
        }
      });

  // Load SQL File from disk in another thread
  auto loadSQLFileAsync = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_loadSQLFileAsync"),
      3,
      [pool](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
      {
        if (sizeof(args) < 3)
        {
          jsi::detail::throwJSError(rt, "[react-native-quick-sqlite][asyncLoadSqlFile] Incorrect parameter count");
          return {};
        }

        const jsi::Value &callbackHolder = args[2];
        if (!callbackHolder.isObject() || !callbackHolder.asObject(rt).isFunction(rt))
        {
          jsi::detail::throwJSError(rt, "[react-native-quick-sqlite][asyncLoadSqlFile] The callback argument must be a function");
          return {};
        }

        const string dbName = args[0].asString(rt).utf8(rt);
        const string sqlFileName = args[1].asString(rt).utf8(rt);
        auto callback = make_shared<jsi::Value>(callbackHolder.asObject(rt));

        auto task =
            [&rt, dbName, sqlFileName, callback]()
        {
          try
          {
            // Running the import operation in another thread
            const auto importResult = importSQLFile(dbName, sqlFileName);

            // Executing the callback invoke inside the JavaScript thread in order to safe build JSI objects that depends on jsi::Runtime and must be synchronized.
            invoker->invokeAsync([&rt, result = move(importResult), callback]
                                 {
              if(result.type == SQLiteOk)
              {
                auto res = jsi::Object(rt);
                res.setProperty(rt, "status", jsi::Value(0));
                res.setProperty(rt, "rowsAffected", jsi::Value(result.affectedRows));
                res.setProperty(rt, "commands", jsi::Value(result.commands));
                callback->asObject(rt).asFunction(rt).call(rt, move(res));
              } else {
                callback->asObject(rt).asFunction(rt).call(rt, createError(rt, result.message));
              } });
          }
          catch (std::exception &exc)
          {
            LOGW("Catched exception: %s", exc.what());
            invoker->invokeAsync([&rt, err = exc.what(), callback]
                                 { callback->asObject(rt).asFunction(rt).call(rt, createError(rt, "Unknown error")); });
          }
        };
        pool->queueWork(task);
        return {};
      });

  // Async Execute SQL
  auto asyncExecSQL = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_asyncExecSQL"),
      4,
      [pool](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value
      {
        if (count < 4)
        {
          jsi::detail::throwJSError(rt, "[react-native-quick-sqlite][asyncExecuteSql] Incorrect arguments for asyncExecuteSQL");
          return {};
        }

        const jsi::Value &callbackHolder = args[3];
        if (!callbackHolder.isObject() || !callbackHolder.asObject(rt).isFunction(rt))
        {
          jsi::detail::throwJSError(rt, "[react-native-quick-sqlite][asyncExecuteSql] The callback argument must be a function");
          return {};
        }

        const string dbName = args[0].asString(rt).utf8(rt);
        const string query = args[1].asString(rt).utf8(rt);
        const jsi::Value &originalParams = args[2];
        auto callback = make_shared<jsi::Value>(callbackHolder.asObject(rt));

        // Converting query parameters inside the javascript caller thread
        vector<QuickValue> params;
        jsiQueryArgumentsToSequelParam(rt, originalParams, &params);

        auto task =
            [&rt, dbName, query, params = make_shared<vector<QuickValue>>(params), callback]()
        {
          try
          {
            // Inside the new worker thread, we can now call sqlite operations
            vector<map<string, QuickValue>> results;
            auto status = sqliteExecute(dbName, query, params.get(), &results);
            invoker->invokeAsync([&rt, results = make_shared<vector<map<string, QuickValue>>>(results), status_copy = move(status), callback]
                                 {
              // Now, back into the JavaScript thread, we can translate the results
              // back to a JSI Object to pass on the callback
              auto jsiResult = createSequelQueryExecutionResult(rt, status_copy, results.get());
              callback->asObject(rt).asFunction(rt).call(rt, move(jsiResult)); });
          }
          catch (std::exception &exc)
          {
            invoker->invokeAsync([&rt, callback, &exc]
                                 { callback->asObject(rt).asFunction(rt).call(rt, createError(rt, exc.what())); });
          }
        };

        pool->queueWork(task);

        return {};
      });

  // Global object
  jsi::Object module = jsi::Object(rt);

  // Callable properties
  module.setProperty(rt, "open", move(open));
  module.setProperty(rt, "close", move(close));
  //    module.setProperty(rt, "attach", move(attach));
  module.setProperty(rt, "delete", move(remove));
  module.setProperty(rt, "executeSql", move(execSQL));
  module.setProperty(rt, "asyncExecuteSql", move(asyncExecSQL));
  module.setProperty(rt, "executeSqlBatch", move(execSQLBatch));
  module.setProperty(rt, "asyncExecuteSqlBatch", move(execSQLBatchAsync));
  module.setProperty(rt, "loadSqlFile", move(loadSQLFile));
  module.setProperty(rt, "asyncLoadSqlFile", move(loadSQLFileAsync));

  rt.global().setProperty(rt, "sqlite", move(module));
}
