#include "react-native-sequel.h"
#include "sequel.h"
#include <iostream>

using namespace std;
using namespace facebook;

void installSequel(jsi::Runtime &runtime)
{

    /**
            OPEN DB INSTANCE
     */
    auto openDb = jsi::Function::createFromHostFunction(
        runtime,
        jsi::PropNameID::forAscii(runtime, "sequel_open"),
        1,
        [](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
            if (!args[0].isString())
            {
                jsi::detail::throwJSError(runtime, "dbName must be a string");
            }

            string dbName = args[0].asString(runtime).utf8(runtime);

            return sequel_open(dbName);
        });

    runtime.global().setProperty(runtime, "sequel_open", move(openDb));
        
    /**
            DELETE DB INSTANCE
     */
    auto deleteDb = jsi::Function::createFromHostFunction(
        runtime,
        jsi::PropNameID::forAscii(runtime, "sequel_delete"),
        1,
        [](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
            if (!args[0].isString())
            {
                jsi::detail::throwJSError(runtime, "dbName must be a string");
            }

            string dbName = args[0].asString(runtime).utf8(runtime);

            return sequel_delete(dbName);
        });

    runtime.global().setProperty(runtime, "sequel_delete", move(deleteDb));


    /**
            CLOSE DB INSTANCE
     */
    auto closeDb = jsi::Function::createFromHostFunction(
        runtime,
        jsi::PropNameID::forAscii(runtime, "sequel_close"),
        1,
        [](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
            if (!args[0].isString())
            {
                jsi::detail::throwJSError(runtime, "dbName must be a string");
            }

            string dbName = args[0].asString(runtime).utf8(runtime);

            return sequel_close(dbName);
        });

    runtime.global().setProperty(runtime, "sequel_close", move(closeDb));

    /**
            EXECUTE SQL (SYNC)
     */
    auto execSQL = jsi::Function::createFromHostFunction(
        runtime,
        jsi::PropNameID::forAscii(runtime, "sequel_execSQL"),
        1,
        [](jsi::Runtime &runtime, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
              vector<jsi::Object> results = sequel_execute(runtime, args[0].asString(runtime).utf8(runtime));

              auto res = jsi::Array(runtime, results.size());
              for(int i = 0; i < results.size(); i++) {
                  res.setValueAtIndex(runtime, i, move(results[i]));
              }

              return res;
        });

    runtime.global().setProperty(runtime, "sequel_execSQL", move(execSQL));
}

void cleanUpSequel()
{
    // intentionally left blank
}
