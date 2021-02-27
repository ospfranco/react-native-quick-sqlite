#import "react-native-sequel.h"
#import "sequel.h"

#include <iostream>

using namespace facebook;

void installSequel(jsi::Runtime& runtime) {
  std::cout << "react-native-sequel: registering JSI bindings" << "\n";

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

  runtime.global().setProperty(runtime, "sequel_open", std::move(openDb));
}

void cleanUpSequel() {
  // intentionally left blank
}