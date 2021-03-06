#include "react-native-sequel.h"
#include "sequel.h"
#include <iostream>

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
            }

            string dbName = args[0].asString(rt).utf8(rt);

            return sequel_open(dbName);
        });

    rt.global().setProperty(rt, "sequel_open", move(openDb));
        
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
            }

            string dbName = args[0].asString(rt).utf8(rt);

            return sequel_remove(dbName);
        });

    rt.global().setProperty(rt, "sequel_delete", move(deleteDb));


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
            }

            string dbName = args[0].asString(rt).utf8(rt);

            return sequel_close(dbName);
        });

    rt.global().setProperty(rt, "sequel_close", move(closeDb));

    /**
            EXECUTE SQL (SYNC)
     */
    auto execSQL = jsi::Function::createFromHostFunction(
        rt,
        jsi::PropNameID::forAscii(rt, "sequel_execSQL"),
        1,
        [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
              vector<jsi::Object> results = sequel_execute(rt, args[0].asString(rt).utf8(rt));

              auto res = jsi::Array(rt, results.size());
              for(int i = 0; i < results.size(); i++) {
                  res.setValueAtIndex(rt, i, move(results[i]));
              }

              return res;
        });

    rt.global().setProperty(rt, "sequel_execSQL", move(execSQL));

    auto asyncExecSQL = jsi::Function::createFromHostFunction(
      rt,
      jsi::PropNameID::forAscii(rt, "sequel_asyncExecSQL"),
      1,
      [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t count) -> jsi::Value {
        jsi::Value promise = rt.global().getPropertyAsFunction(rt, "Promise").callAsConstructor(
          rt,
          jsi::Function::createFromHostFunction(
                                                rt,
                                                jsi::PropNameID::forAscii(rt, "executor"),
                                                2,
                                                [](jsi::Runtime &rt, const jsi::Value &thisValue, const jsi::Value *args, size_t) -> jsi::Value {

            args[0].asObject(rt).asFunction(rt).call(rt, jsi::Value(42));
            return {};
        }));

        return promise;
    });

    rt.global().setProperty(rt, "sequel_asyncExecSQL", move(asyncExecSQL));
}

void cleanUpSequel()
{
    // intentionally left blank
}
