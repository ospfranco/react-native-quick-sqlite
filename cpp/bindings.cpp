#include "bindings.h"
#include "sqliteBridge.h"
#include "logs.h"
#include "JSIHelper.h"
#include "ThreadPool.h"
#include "sqlfileloader.h"
#include "sqlbatchexecutor.h"
#include <vector>
#include <string>
#include "macros.h"

using namespace std;
using namespace facebook;

namespace osp {
string docPathStr;
std::shared_ptr<react::CallInvoker> invoker;

// Utility FNS
jsi::Object error(jsi::Runtime &rt, string message)
{
  auto res = jsi::Object(rt);
  res.setProperty(rt, "status", jsi::Value(1));
  res.setProperty(rt, "message", jsi::String::createFromUtf8(rt, message));
  return res;
}

jsi::Object ok(jsi::Runtime &rt)
{
  auto res = jsi::Object(rt);
  res.setProperty(rt, "status", jsi::Value(0));
  return res;
}

// END Utility FNS

void install(jsi::Runtime &rt, std::shared_ptr<react::CallInvoker> jsCallInvoker, const char *docPath)
{
  docPathStr = std::string(docPath);
  auto pool = std::make_shared<ThreadPool>();
  invoker = jsCallInvoker;

  auto open = HOSTFN("open", 2) {
    if (count == 0)
    {
      return error(rt, "[react-native-quick-sqlite][open] database name is required");
    }

    if (!args[0].isString())
    {
      return error(rt, "[react-native-quick-sqlite][open] database name must be a string");
    }

    string dbName = args[0].asString(rt).utf8(rt);
    string tempDocPath = string(docPathStr);
    if (count > 1 && !args[1].isUndefined() && !args[1].isNull())
    {
      if (!args[1].isString())
      {
        return error(rt, "[react-native-quick-sqlite][open] database location must be a string");
      }

      tempDocPath = tempDocPath + "/" + args[1].asString(rt).utf8(rt);
    }

    SQLiteOPResult result = sqliteOpenDb(dbName, tempDocPath);

    if (result.type == SQLiteError)
    {
      return error(rt, result.errorMessage.c_str());
    }

    return ok(rt);
  });
  
  auto attach = HOSTFN("attach", 4) {
    if(count < 3) {
      return error(rt, "[react-native-quick-sqlite][attach] Incorrect number of arguments");
    }
    if (!args[0].isString() || !args[1].isString() || !args[2].isString())
    {
      throw jsi::JSError(rt, "dbName, databaseToAttach and alias must be a strings");
      return {};
    }

    string tempDocPath = string(docPathStr);
    if (count > 3 && !args[3].isUndefined() && !args[3].isNull())
    {
      if (!args[3].isString())
      {
        return error(rt, "[react-native-quick-sqlite][attach] database location must be a string");
      }

      tempDocPath = tempDocPath + "/" + args[3].asString(rt).utf8(rt);
    }

    string dbName = args[0].asString(rt).utf8(rt);
    string databaseToAttach = args[1].asString(rt).utf8(rt);
    string alias = args[2].asString(rt).utf8(rt);
    SQLiteOPResult result = sqliteAttachDb(dbName, tempDocPath, databaseToAttach, alias);

    if (result.type == SQLiteError)
    {
      return error(rt, result.errorMessage.c_str());
    }

    return ok(rt);
  });
  
  auto detach = HOSTFN("detach", 2) {
    if(count < 2) {
      return error(rt, "[react-native-quick-sqlite][detach] Incorrect number of arguments");
    }
    if (!args[0].isString() || !args[1].isString())
    {
      throw jsi::JSError(rt, "dbName, databaseToAttach and alias must be a strings");
      return {};
    }

    string dbName = args[0].asString(rt).utf8(rt);
    string alias = args[1].asString(rt).utf8(rt);
    SQLiteOPResult result = sqliteDetachDb(dbName, alias);

    if (result.type == SQLiteError)
    {
      return error(rt, result.errorMessage.c_str());
    }

    return ok(rt);
  });

  auto close = HOSTFN("close", 1)
  {
    if (count == 0)
    {
      return error(rt, "[react-native-quick-sqlite][close] database name is required");
    }

    if (!args[0].isString())
    {
      return error(rt, "[react-native-quick-sqlite][close] database name must be a string");
    }

    string dbName = args[0].asString(rt).utf8(rt);

    SQLiteOPResult result = sqliteCloseDb(dbName);

    if (result.type == SQLiteError)
    {
      return error(rt, result.errorMessage.c_str());
    }

    return ok(rt);
  });

  auto remove = HOSTFN("delete", 2)
  {
    if (count == 0)
    {
      return error(rt, "[react-native-quick-sqlite][open] database name is required");
    }

    if (!args[0].isString())
    {
      return error(rt, "[react-native-quick-sqlite][open] database name must be a string");
    }

    string dbName = args[0].asString(rt).utf8(rt);

    string tempDocPath = string(docPathStr);
    if (count > 1 && !args[1].isUndefined() && !args[1].isNull())
    {
      if (!args[1].isString())
      {
        return error(rt, "[react-native-quick-sqlite][open] database location must be a string");
      }

      tempDocPath = tempDocPath + "/" + args[1].asString(rt).utf8(rt);
    }


    SQLiteOPResult result = sqliteRemoveDb(dbName, tempDocPath);

    if (result.type == SQLiteError)
    {
      return error(rt, result.errorMessage.c_str());
    }

    return ok(rt);
  });

  auto execute = HOSTFN("execute", 3)
  {
    const string dbName = args[0].asString(rt).utf8(rt);
    const string query = args[1].asString(rt).utf8(rt);
    vector<QuickValue> params;
    if(count == 3) {
      const jsi::Value &originalParams = args[2];
      jsiQueryArgumentsToSequelParam(rt, originalParams, &params);
    }

    vector<map<string, QuickValue>> results;
    vector<QuickColumnMetadata> metadata;
    auto status = sqliteExecute(dbName, query, &params, &results, &metadata);

    // Converting results into a JSI Response
    auto jsiResult = createSequelQueryExecutionResult(rt, status, &results, &metadata);
    return jsiResult;
  });

  auto transaction = HOSTFN("transaction", 3)
  {
    const string dbName = args[0].asString(rt).utf8(rt);
    const string query = args[1].asString(rt).utf8(rt);
    vector<QuickValue> params;
    if(count == 3) {
      const jsi::Value &originalParams = args[2];
      jsiQueryArgumentsToSequelParam(rt, originalParams, &params);
    }

    sqliteExecuteLiteral(dbName, "BEGIN EXCLUSIVE TRANSACTION");

    vector<map<string, QuickValue>> results;
    vector<QuickColumnMetadata> metadata;
    auto status = sqliteExecute(dbName, query, &params, &results, &metadata);

    if(status.type == SQLiteError) {
      sqliteExecuteLiteral(dbName, "ROLLBACK");
      return error(rt, status.errorMessage);
    }

    // Converting results into a JSI Response
    auto jsiResult = createSequelQueryExecutionResult(rt, status, &results, &metadata);
    return jsiResult;
  });

  auto executeAsync = HOSTFN("executeAsync", 3)
  {
    if (count < 3)
    {
      throw jsi::JSError(rt, "[react-native-quick-sqlite][executeAsync] Incorrect arguments for executeAsync");
    }

    const string dbName = args[0].asString(rt).utf8(rt);
    const string query = args[1].asString(rt).utf8(rt);
    const jsi::Value &originalParams = args[2];

    // Converting query parameters inside the javascript caller thread
    vector<QuickValue> params;
    jsiQueryArgumentsToSequelParam(rt, originalParams, &params);

    auto promiseCtr = rt.global().getPropertyAsFunction(rt, "Promise");
    auto promise = promiseCtr.callAsConstructor(rt, HOSTFN("executor", 2) {
      auto resolve = std::make_shared<jsi::Value>(rt, args[0]);
      auto reject = std::make_shared<jsi::Value>(rt, args[1]);

      auto task =
      [&rt, dbName, query, params = make_shared<vector<QuickValue>>(params), resolve, reject]()
      {
        try
        {
          // Inside the new worker thread, we can now call sqlite operations
          vector<map<string, QuickValue>> results;
          vector<QuickColumnMetadata> metadata;
          auto status = sqliteExecute(dbName, query, params.get(), &results, &metadata);
          invoker->invokeAsync([&rt, results = make_shared<vector<map<string, QuickValue>>>(results), metadata = make_shared<vector<QuickColumnMetadata>>(metadata), status_copy = move(status), resolve]
                               {
            // Now, back into the JavaScript thread, we can translate the results
            // back to a JSI Object to pass on the callback
            auto jsiResult = createSequelQueryExecutionResult(rt, status_copy, results.get(), metadata.get());
            resolve->asObject(rt).asFunction(rt).call(rt, move(jsiResult)); });
        }
        catch (std::exception &exc)
        {
          invoker->invokeAsync([&rt, reject, &exc]
                               { reject->asObject(rt).asFunction(rt).call(rt, error(rt, exc.what())); });
        }
      };

      pool->queueWork(task);

      return {};
    }));

    return promise;
  });
  
  // Execute a batch of SQL queries in a transaction
  // Parameters can be: [[sql: string, arguments: any[] | arguments: any[][] ]]
  auto executeBatch = HOSTFN("executeBatch", 2)
  {
    if (sizeof(args) < 2)
    {
      return error(rt, "[react-native-quick-sqlite][executeBatch] - Incorrect parameter count");
    }

    const jsi::Value &params = args[1];
    if (params.isNull() || params.isUndefined())
    {
      return error(rt, "[react-native-quick-sqlite][executeBatch] - An array of SQL commands or parameters is needed");
    }
    const string dbName = args[0].asString(rt).utf8(rt);
    const jsi::Array &batchParams = params.asObject(rt).asArray(rt);
    vector<QuickQueryArguments> commands;
    jsiBatchParametersToQuickArguments(rt, batchParams, &commands);

    auto batchResult = sqliteExecuteBatch(dbName, &commands);
    if (batchResult.type == SQLiteOk)
    {
      auto res = ok(rt);
      res.setProperty(rt, "rowsAffected", jsi::Value(batchResult.affectedRows));
      return move(res);
    }
    else
    {
      return error(rt, batchResult.message);
    }
  });
  
  auto executeBatchAsync = HOSTFN("executeBatchAsync", 3)
  {
    if (sizeof(args) < 3)
    {
      throw jsi::JSError(rt, "[react-native-quick-sqlite][executeAsyncBatch] Incorrect parameter count");
      return {};
    }

    const jsi::Value &params = args[1];
    const jsi::Value &callbackHolder = args[2];
    if (!callbackHolder.isObject() || !callbackHolder.asObject(rt).isFunction(rt))
    {
      throw jsi::JSError(rt, "[react-native-quick-sqlite][executeAsyncBatch] The callback argument must be a function");
      return {};
    }

    if (params.isNull() || params.isUndefined())
    {
      throw jsi::JSError(rt, "[react-native-quick-sqlite][executeAsyncBatch] - An array of SQL commands or parameters is needed");
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
        auto batchResult = sqliteExecuteBatch(dbName, commands.get());
        invoker->invokeAsync([&rt, batchResult = move(batchResult), callback]
                             {
          if(batchResult.type == SQLiteOk)
          {
            auto res = ok(rt);
            res.setProperty(rt, "rowsAffected", jsi::Value(batchResult.affectedRows));
            callback->asObject(rt).asFunction(rt).call(rt, move(res));
          } else
          {
            callback->asObject(rt).asFunction(rt).call(rt, error(rt, batchResult.message));
          } });
      }
      catch (std::exception &exc)
      {
        invoker->invokeAsync([&rt, callback, &exc]
                             { callback->asObject(rt).asFunction(rt).call(rt, error(rt, exc.what())); });
      }
    };
    pool->queueWork(task);
    return {};
  });

  auto loadFile = HOSTFN("loadFile", 2)
  {
    const string dbName = args[0].asString(rt).utf8(rt);
    const string sqlFileName = args[1].asString(rt).utf8(rt);

    const auto importResult = importSQLFile(dbName, sqlFileName);
    if (importResult.type == SQLiteOk)
    {
      auto res = ok(rt);
      res.setProperty(rt, "rowsAffected", jsi::Value(importResult.affectedRows));
      res.setProperty(rt, "commands", jsi::Value(importResult.commands));
      return move(res);
    }
    else
    {
      return error(rt, "[react-native-quick-sqlite][loadFile] Could not open file");
    }
  });
  
  // Load SQL File from disk in another thread
  auto loadFileAsync = HOSTFN("loadFileAsync", 3)
  {
    if (sizeof(args) < 3)
    {
      throw jsi::JSError(rt, "[react-native-quick-sqlite][asyncloadFile] Incorrect parameter count");
      return {};
    }

    const jsi::Value &callbackHolder = args[2];
    if (!callbackHolder.isObject() || !callbackHolder.asObject(rt).isFunction(rt))
    {
      throw jsi::JSError(rt, "[react-native-quick-sqlite][asyncloadFile] The callback argument must be a function");
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
            auto res = ok(rt);
            res.setProperty(rt, "rowsAffected", jsi::Value(result.affectedRows));
            res.setProperty(rt, "commands", jsi::Value(result.commands));
            callback->asObject(rt).asFunction(rt).call(rt, move(res));
          } else {
            callback->asObject(rt).asFunction(rt).call(rt, error(rt, result.message));
          } });
      }
      catch (std::exception &exc)
      {
        LOGW("Catched exception: %s", exc.what());
        invoker->invokeAsync([&rt, err = exc.what(), callback]
                             { callback->asObject(rt).asFunction(rt).call(rt, error(rt, "Unknown error")); });
      }
    };
    pool->queueWork(task);
    return {};
  });



  jsi::Object module = jsi::Object(rt);

  module.setProperty(rt, "open", move(open));
  module.setProperty(rt, "close", move(close));
  module.setProperty(rt, "attach", move(attach));
  module.setProperty(rt, "detach", move(detach));
  module.setProperty(rt, "delete", move(remove));
  module.setProperty(rt, "execute", move(execute));
  module.setProperty(rt, "transaction", move(transaction));
  module.setProperty(rt, "executeAsync", move(executeAsync));
  module.setProperty(rt, "executeBatch", move(executeBatch));
  module.setProperty(rt, "executeBatchAsync", move(executeBatchAsync));
  module.setProperty(rt, "loadFile", move(loadFile));
  module.setProperty(rt, "loadFileAsync", move(loadFileAsync));
  
  rt.global().setProperty(rt, "__QuickSQLiteProxy", move(module));
}

}
