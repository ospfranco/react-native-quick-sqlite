/*
 * react-native-sequel.cpp
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the SSPL license
 * https://www.mongodb.com/licensing/server-side-public-license
 */

#include "react-native-sequel.h"
#include "sequel.h"
#include "SequelResult.h"
#include <vector>
#include <iostream>
#include <thread>

using namespace std;
using namespace facebook;

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

// void installSequel(jsi::Runtime &rt, shared_ptr<react::CallInvoker> callInvoker)
void installSequel(jsi::Runtime &rt)
{
  /**
            OPEN DB INSTANCE
     */
  auto open = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_open"),
      1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
        if (!args[0].isString())
        {
          jsi::detail::throwJSError(rt, "dbName must be a string");
          return {};
        }

        string dbName = args[0].asString(rt).utf8(rt);
        SequelResult result = sequel_open(dbName);

        if (result.type == SequelResultError)
        {
          jsi::detail::throwJSError(rt, result.message.c_str());
          return {};
        }

        return move(result.value);
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

  /**
            CLOSE DB INSTANCE
     */
  auto close = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_close"),
      1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
        if (!args[0].isString())
        {
          jsi::detail::throwJSError(rt, "dbName must be a string");
          return {};
        }

        string dbName = args[0].asString(rt).utf8(rt);

        SequelResult result = sequel_close(dbName);

        if (result.type == SequelResultError)
        {
          jsi::detail::throwJSError(rt, result.message.c_str());
          return {};
        }

        return move(result.value);
      });

  /**
            DELETE DB INSTANCE
     */
  auto remove = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_delete"),
      1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
        if (!args[0].isString())
        {
          jsi::detail::throwJSError(rt, "dbName must be a string");
          return {};
        }

        string dbName = args[0].asString(rt).utf8(rt);

        SequelResult result = sequel_remove(dbName);

        if (result.type == SequelResultError)
        {
          jsi::detail::throwJSError(rt, result.message.c_str());
          return {};
        }

        return jsi::Value::undefined();
      });

  /**
            EXECUTE SQL
     */
  auto execSQL = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_execSQL"),
      3,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
        const string dbName = args[0].asString(rt).utf8(rt);
        const string query = args[1].asString(rt).utf8(rt);
        const jsi::Value &params = args[2];
        SequelResult result = sequel_execute(rt, dbName, query, params);

        if (result.type == SequelResultError)
        {
          jsi::detail::throwJSError(rt, result.message.c_str());
          return {};
        }

        return move(result.value);
      });

  /**
            ASYNC EXECUTE SQL
     */
  // auto asyncExecSQL = jsi::Function::createFromHostFunction(
  //     rt,
  //     jsi::PropNameID::forAscii(rt, "sequel_asyncExecSQL"),
  //     3,
  //     [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
  //       const string dbName = args[0].asString(rt).utf8(rt);
  //       const string query = args[1].asString(rt).utf8(rt);
  //       const jsi::Value &params = args[2];

  //       jsi::Value promise = rt.global().getPropertyAsFunction(rt, "Promise").callAsConstructor(rt, jsi::Function::createFromHostFunction(rt, jsi::PropNameID::forAscii(rt, "executor"), 2, [dbName, query, &params](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t) -> jsi::Value {
  //                                                                                                 // Spawn c++ thread
  //                                                                                                 thread t1([&rt, &dbName, &query, &params, resolve{make_shared<jsi::Value>(rt, args[0])}] {
  //                                                                                                   SequelResult result = sequel_execute(rt, dbName, query, params);

  //                                                                                                   if (result.type == SequelResultError)
  //                                                                                                   {
  //                                                                                                     jsi::detail::throwJSError(rt, result.message.c_str());
  //                                                                                                   }
  //                                                                                                   else
  //                                                                                                   {
  //                                                                                                     resolve->asObject(rt).asFunction(rt).call(rt, move(result.value));
  //                                                                                                   }
  //                                                                                                 });

  //                                                                                                 t1.detach();

  //                                                                                                 return {};
  //                                                                                               }));

  //       return promise;
  //     });

// Create final object that will be injected into the global object
  jsi::Object module = jsi::Object(rt);

  // Open/Close
  module.setProperty(rt, "open", move(open));
  module.setProperty(rt, "close", move(close));
  //    module.setProperty(rt, "attach", move(attach));
  module.setProperty(rt, "delete", move(remove));

  module.setProperty(rt, "executeSql", move(execSQL));
  // module.setProperty(rt, "backgroundExecuteSql", move(asyncExecSQL));

  rt.global().setProperty(rt, "sqlite", move(module));
}

void cleanUpSequel()
{
  // intentionally left blank
}
