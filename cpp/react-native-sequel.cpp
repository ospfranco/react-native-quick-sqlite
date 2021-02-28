#import "react-native-sequel.h"
#import "sequel.h"

#include <iostream>

using namespace std;
using namespace facebook;

void installSequel(jsi::Runtime& runtime) {
  cout << "react-native-sequel: registering JSI bindings" << endl;

  // auto multiply = jsi::Function::createFromHostFunction(
  //   runtime,
  //   jsi::PropNameID::forAscii(runtime, "multiply"),
  //   2, // number of arguments
  //   [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
  //     if(!arguments[0].isNumber() || !arguments[1].isNumber()) {
  //       jsi::detail::throwJSError(runtime, "Non number arguments passed to sequel");
  //     }

  //     double res = arguments[0].asNumber() * arguments[1].asNumber();

  //     return jsi::Value(res);
  //   }
  // );

  // runtime.global().setProperty(runtime, "multiply", std::move(multiply));

  auto openDb = jsi::Function::createFromHostFunction(
    runtime,
    jsi::PropNameID::forAscii(runtime, "sequel_open"),
    1,
    [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* args, size_t count) -> jsi::Value {
      if(!args[0].isString()) {
        jsi::detail::throwJSError(runtime, "dbName must be a string");
      }

      std::string dbName = args[0].asString(runtime).utf8(runtime);

      return sequel_open(dbName);
    }
  );

  runtime.global().setProperty(runtime, "sequel_open", move(openDb));

  auto init = jsi::Function::createFromHostFunction(
    runtime,
    jsi::PropNameID::forAscii(runtime, "sequel_init"),
    0,
    [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* args, size_t count) -> jsi::Value {
      vector<std::map<string, string> > results = sequel_init();
      auto arrayRes = jsi::Array(runtime, results.size());
      for(std::vector<int>::size_type i = 0; i != results.size(); i++) {
          auto row = results[i];
          jsi::Object rowRes = jsi::Object(runtime);

          for(auto it = row.begin(); it != row.end(); it++) {
            string column = it->first;
            string columnValue = it->second;
            rowRes.setProperty(runtime, column.c_str(), jsi::String::createFromAscii(runtime, columnValue.c_str()));
          }

          arrayRes.setValueAtIndex(runtime, i, rowRes);
      }

      return arrayRes;
    }
  );

  runtime.global().setProperty(runtime, "sequel_init", move(init));
}

void cleanUpSequel() {
  // intentionally left blank
}
