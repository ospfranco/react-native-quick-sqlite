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

#include <iostream>
#include <thread>

using namespace std;
using namespace facebook;

void installSequel(jsi::Runtime &rt, std::shared_ptr<react::CallInvoker> callInvoker)
{

    /**
            OPEN DB INSTANCE
     */
    auto openDb = jsi::Function::createFromHostFunction(
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

            if(result.type == SequelResultError) {
                jsi::detail::throwJSError(rt, result.message.c_str());
                return {};
            }


            return move(result.value);
        });

    rt.global().setProperty(rt, "sequel_open", move(openDb));

    /**
            CLOSE DB INSTANCE
     */
    auto closeDb = jsi::Function::createFromHostFunction(
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

            if(result.type == SequelResultError) {
                jsi::detail::throwJSError(rt, result.message.c_str());
                return {};
            }

            return move(result.value);
        });

    rt.global().setProperty(rt, "sequel_close", move(closeDb));
        
    /**
            DELETE DB INSTANCE
     */
    auto deleteDb = jsi::Function::createFromHostFunction(
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

            if(result.type == SequelResultError) {
                jsi::detail::throwJSError(rt, result.message.c_str());
                return {};
            }

            return jsi::Value::undefined();
        });

    rt.global().setProperty(rt, "sequel_delete", move(deleteDb));

    /**
            EXECUTE SQL
     */
    auto execSQL = jsi::Function::createFromHostFunction(
        rt,
        jsi::PropNameID::forAscii(rt, "sequel_execSQL"),
        2,
        [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
            const string dbName = args[0].asString(rt).utf8(rt);
            const string query = args[1].asString(rt).utf8(rt);
            vector<jsi::Object> results = sequel_execute(rt, dbName, query);

            auto res = jsi::Array(rt, results.size());
            for(int i = 0; i < results.size(); i++) {
              res.setValueAtIndex(rt, i, move(results[i]));
            }

            return res;
        });

    rt.global().setProperty(rt, "sequel_execSQL", move(execSQL));

    /**
            ASYNC EXECUTE SQL
     */
    auto asyncExecSQL = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_asyncExecSQL"),
      2,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
        const string dbName = args[0].asString(rt).utf8(rt);
        const string query = args[1].asString(rt).utf8(rt);

        jsi::Value promise = rt.global().getPropertyAsFunction(rt, "Promise").callAsConstructor(
          rt,
          jsi::Function::createFromHostFunction(
            rt,
            jsi::PropNameID::forAscii(rt, "executor"),
            2,
            [dbName, query](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t) -> jsi::Value {
                thread t1([&rt, &dbName, &query, resolve{ std::make_shared<jsi::Value>(rt, args[0]) }] {
                    vector<jsi::Object> results = sequel_execute(rt, dbName, query);

                    auto res = jsi::Array(rt, results.size());
                    for(int i = 0; i < results.size(); i++) {
                        res.setValueAtIndex(rt, i, move(results[i]));
                    }

                    resolve->asObject(rt).asFunction(rt).call(rt, res);
                });

                t1.detach();


                return {};
            })
        );

        return promise;
    });

    rt.global().setProperty(rt, "sequel_asyncExecSQL", move(asyncExecSQL));
}

void cleanUpSequel()
{
    // intentionally left blank
}
