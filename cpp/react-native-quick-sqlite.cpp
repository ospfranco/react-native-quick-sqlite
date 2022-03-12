/*
 * react-native-quick-sqlite.cpp
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the MIT license
 */

#include "react-native-quick-sqlite.h"
#include "sequel.h"
#include "logs.h"
#include "SequelResult.h"
#include <vector>
#include <iostream>
#include <thread>
#include <chrono> 
#include <fstream>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <exception>

using namespace std;
using namespace facebook;

class ThreadPool
{
public:
  ThreadPool() : done(false)
  {
    // This returns the number of threads supported by the system. If the
    // function can't figure out this information, it returns 0. 0 is not good,
    // so we create at least 1
    auto numberOfThreads = std::thread::hardware_concurrency();
    if (numberOfThreads == 0)
    {
      numberOfThreads = 1;
    }

    for (unsigned i = 0; i < numberOfThreads; ++i)
    {
      // The threads will execute the private member `doWork`. Note that we need
      // to pass a reference to the function (namespaced with the class name) as
      // the first argument, and the current object as second argument
      threads.push_back(std::thread(&ThreadPool::doWork, this));
    }
  }

  // The destructor joins all the threads so the program can exit gracefully.
  // This will be executed if there is any exception (e.g. creating the threads)
  ~ThreadPool()
  {
    // So threads know it's time to shut down
    done = true;

    // Wake up all the threads, so they can finish and be joined
    workQueueConditionVariable.notify_all();
    for (auto &thread : threads)
    {
      if (thread.joinable())
      {
        thread.join();
      }
    }
  }

  // This function will be called by the server every time there is a request
  // that needs to be processed by the thread pool
  void queueWork(std::function<void(void)> task)
  {
    // Grab the mutex
    std::lock_guard<std::mutex> g(workQueueMutex);

    // Push the request to the queue
    workQueue.push(task);

    // Notify one thread that there are requests to process
    workQueueConditionVariable.notify_one();
  }

private:
  // This condition variable is used for the threads to wait until there is work
  // to do
  std::condition_variable_any workQueueConditionVariable;

  // We store the threads in a vector, so we can later stop them gracefully
  std::vector<std::thread> threads;

  // Mutex to protect workQueue
  std::mutex workQueueMutex;

  // Queue of requests waiting to be processed
  std::queue<std::function<void(void)>> workQueue;

  // This will be set to true when the thread pool is shutting down. This tells
  // the threads to stop looping and finish
  bool done;

  // Function used by the threads to grab work from the queue
  void doWork()
  {
    // Loop while the queue is not destructing
    while (!done)
    {
      std::function<void(void)> task;

      // Create a scope, so we don't lock the queue for longer than necessary
      {
        std::unique_lock<std::mutex> g(workQueueMutex);
        workQueueConditionVariable.wait(g, [&]
                                        {
          // Only wake up if there are elements in the queue or the program is
          // shutting down
          return !workQueue.empty() || done; });

        // If we are shutting down exit witout trying to process more work
        if (done)
        {
          break;
        }

        task = workQueue.front();
        workQueue.pop();
      }

      task();
    }
  }
};

const vector<string> mapParams(jsi::Runtime &rt, jsi::Array &params)
{
  int jsiParamsLength = params.length(rt);
  vector<string> res;
  for (int ii = 0; ii < jsiParamsLength; ii++)
  {
    res.push_back(params.getValueAtIndex(rt, ii).asString(rt).utf8(rt));
  }
  return res;
}

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

/**
 * Local function to handle SQL File Import in order to reuse with Sync and Async operations
 */
SequelBatchOperationResult importSQLFile(string dbName, string fileLocation) {
  string line;
  ifstream sqFile(fileLocation);
  if (sqFile.is_open())
  {
    try
    {
      int affectedRows = 0;
      int commands = 0;
      sequel_execute_literal_update(dbName, "BEGIN EXCLUSIVE TRANSACTION");
      while (std::getline(sqFile, line, '\n'))
      {
        if (!line.empty())
        {
          SequelLiteralUpdateResult result = sequel_execute_literal_update(dbName, line);
          if (result.type == SequelResultError)
          {
            sequel_execute_literal_update(dbName, "ROLLBACK");
            sqFile.close();
            return {SequelResultError, result.message, 0, commands};
          }
          else
          {
            affectedRows += result.affectedRows;
            commands++;
          }
        }
      }
      sqFile.close();
      sequel_execute_literal_update(dbName, "COMMIT");
      return {SequelResultOk, "", affectedRows,commands};
    }
    catch (...)
    {
      sqFile.close();
      sequel_execute_literal_update(dbName, "ROLLBACK");
      return {SequelResultError, "[react-native-quick-sqlite][loadSQLFile] Unexpected error, transaction was rolledback", 0, 0};
    }
  }
  else
  {
    return {SequelResultError, "[react-native-quick-sqlite][loadSQLFile] Could not open file", 0, 0};
  }
}

void installSequel(jsi::Runtime &rt, std::shared_ptr<react::CallInvoker> jsCallInvoker, const char *docPath)
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

        SequelResult result = sequel_open(dbName, tempDocPath);

        if (result.type == SequelResultError)
        {
          return createError(rt, result.message.c_str());
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
  //            if (result.type == SequelResultError)
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

        SequelResult result = sequel_close(dbName);

        if (result.type == SequelResultError)
        {
          return createError(rt, result.message.c_str());
        }

        return move(result.value);
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

        SequelResult result = sequel_remove(dbName, docPathStr);

        if (result.type == SequelResultError)
        {
          return createError(rt, result.message.c_str());
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
        const jsi::Value &params = args[2];
        SequelResult result = sequel_execute(rt, dbName, query, params);

        if (result.type == SequelResultError)
        {
          return createError(rt, result.message.c_str());
        }

        return move(result.value);
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

        const string dbName = args[0].asString(rt).utf8(rt);
        const jsi::Value &params = args[1];
        if (params.isNull() || params.isUndefined())
        {
          return createError(rt, "[react-native-quick-sqlite][execSQLBatch] - An array of SQL commands or parameters is needed");
        }

        int rowsAffected = 0;
        const jsi::Array &batchParams = params.asObject(rt).asArray(rt);
        try
        {
          sequel_execute(rt, dbName, "BEGIN TRANSACTION", jsi::Value::undefined());
          for (int i = 0; i < batchParams.length(rt); i++)
          {
            const jsi::Array &command = batchParams.getValueAtIndex(rt, i).asObject(rt).asArray(rt);
            if (command.length(rt) == 0)
            {
              sequel_execute(rt, dbName, "ROLLBACK", jsi::Value::undefined());
              return createError(rt, "[react-native-quick-sqlite][execSQLBatch] - No SQL Commands found on batch index " + std::to_string(i));
            }
            const string query = command.getValueAtIndex(rt, 0).asString(rt).utf8(rt);
            const jsi::Value &commandParams = command.length(rt) > 1 ? command.getValueAtIndex(rt, 1) : jsi::Value::undefined();
            if (!commandParams.isUndefined() && commandParams.asObject(rt).isArray(rt) && commandParams.asObject(rt).asArray(rt).length(rt) > 0 && commandParams.asObject(rt).asArray(rt).getValueAtIndex(rt, 0).isObject())
            {
              // This arguments are an array of arrays, like a batch update of a single sql command.
              const jsi::Array &batchUpdateParams = commandParams.asObject(rt).asArray(rt);
              for (int x = 0; x < batchUpdateParams.length(rt); x++)
              {
                const jsi::Value &p = batchUpdateParams.getValueAtIndex(rt, x);
                SequelResult result = sequel_execute(rt, dbName, query, p);
                if (result.type == SequelResultError)
                {
                  sequel_execute(rt, dbName, "ROLLBACK", jsi::Value::undefined());
                  return createError(rt, result.message.c_str());
                }
                else
                {
                  if (result.value.getObject(rt).hasProperty(rt, jsi::PropNameID::forAscii(rt, "rowsAffected")))
                  {
                    rowsAffected += result.value.getObject(rt).getProperty(rt, jsi::PropNameID::forAscii(rt, "rowsAffected")).asNumber();
                  }
                }
              }
            }
            else
            {
              SequelResult result = sequel_execute(rt, dbName, query, commandParams);
              if (result.type == SequelResultError)
              {
                sequel_execute(rt, dbName, "ROLLBACK", jsi::Value::undefined());

                return createError(rt, result.message.c_str());
              }
              else
              {
                if (result.value.getObject(rt).hasProperty(rt, jsi::PropNameID::forAscii(rt, "rowsAffected")))
                {
                  rowsAffected += result.value.getObject(rt).getProperty(rt, jsi::PropNameID::forAscii(rt, "rowsAffected")).asNumber();
                }
              }
            }
          }
          sequel_execute(rt, dbName, "COMMIT", jsi::Value::undefined());
        }
        catch (...)
        {
          sequel_execute(rt, dbName, "ROLLBACK", jsi::Value::undefined());
          return createError(rt, "[react-native-quick-sqlite][execSQLBatch] - Unexpected error");
        }

        auto res = jsi::Object(rt);
        res.setProperty(rt, "status", jsi::Value(0));
        res.setProperty(rt, "rowsAffected", jsi::Value(rowsAffected));
        return move(res);
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
        if(importResult.type == SequelResultOk)
        {
          auto res = jsi::Object(rt);
          res.setProperty(rt, "status", jsi::Value(0));
          res.setProperty(rt, "rowsAffected", jsi::Value(importResult.affectedRows));
          res.setProperty(rt, "commands", jsi::Value(importResult.commands));
          return move(res);
        } else {
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
        const string dbName = args[0].asString(rt).utf8(rt);
        const string sqlFileName = args[1].asString(rt).utf8(rt);
        auto callback = make_shared<jsi::Value>((args[2].asObject(rt)));

        auto task =
            [&rt, dbName, sqlFileName, callback]()
        {
          try
          {
            // Running the import operation in another thread
            const auto importResult = importSQLFile(dbName, sqlFileName);

            // Executing the callback invoke inside the JavaScript thread in order to safe build JSI objects that depends on jsi::Runtime and must be synchronized.
            invoker->invokeAsync([&rt, result = move(importResult), callback] {
              if(result.type == SequelResultOk)
              {
                auto res = jsi::Object(rt);
                res.setProperty(rt, "status", jsi::Value(0));
                res.setProperty(rt, "rowsAffected", jsi::Value(result.affectedRows));
                res.setProperty(rt, "commands", jsi::Value(result.commands));
                callback->asObject(rt).asFunction(rt).call(rt, move(res));
              } else {
                callback->asObject(rt).asFunction(rt).call(rt, createError(rt, result.message));
              }
            });
          }
          catch (std::exception &exc)
          {
            LOGW("Catched exception: %s", exc.what());
            invoker->invokeAsync([&rt, err = exc.what(), callback] {
              callback->asObject(rt).asFunction(rt).call(rt, createError(rt, "Unknow error"));
            });
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
          LOGW("NOT ENOUGH PARAMS PASSED");
        }
        const string dbName = args[0].asString(rt).utf8(rt);
        const string query = args[1].asString(rt).utf8(rt);
        auto params = make_shared<jsi::Value>(args[2].asObject(rt));
        auto callback = make_shared<jsi::Value>((args[3].asObject(rt)));

        auto task =
            [&rt, dbName, query, params, callback]()
        {
          try
          {
            SequelResult result = sequel_execute(rt, dbName, query, *params);
            LOGW("ROPO FINISHED COMPUTING");
            if (result.type == SequelResultError)
            {
              LOGW("RETURNING ERROR");
              invoker->invokeAsync([&rt, callback, &result] {
                callback->asObject(rt).asFunction(rt).call(rt, createError(rt, result.message.c_str()));
              });
            }
            else
            {
              LOGW("RETURNING SUCCESS");
              invoker->invokeAsync([&rt, callback, &result] {
                callback->asObject(rt).asFunction(rt).call(rt, result.value);
              });
              LOGW("SUCCESS CALLED");
            }
          }
          catch (std::exception &exc)
          {
            LOGW("Catched exception: %s", exc.what());
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
  module.setProperty(rt, "executeSqlBatch", move(execSQLBatch));
  module.setProperty(rt, "loadSqlFile", move(loadSQLFile));
  module.setProperty(rt, "asyncLoadSqlFile", move(loadSQLFileAsync));
  module.setProperty(rt, "asyncExecuteSql", move(asyncExecSQL));
  
  rt.global().setProperty(rt, "sqlite", move(module));
}

void cleanUpSequel()
{
  // intentionally left blank
}
