#import "react-native-sequel.h"

#include <iostream>
// #include <sstream>

using namespace facebook;

void installSequel(jsi::Runtime& runtime) {
  std::cout << "Initializing react-native-sequel" << "\n";

  auto multiply = jsi::Function::createFromHostFunction(
    runtime,
    jsi::PropNameID::forAscii(runtime, "multiply"),
    2, // number of arguments
    [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
      if(!arguments[0].isNumber() || !arguments[1].isNumber()) {
        jsi::detail::throwJSError(runtime, "Non number arguments passed to sequel");
      }

      double res = arguments[0].asNumber() * arguments[1].asNumber();

      // return jsi::Value(arguments[0].asNumber(runtime) * arguments[1].asNumber(runtime));
      return jsi::Value(res);
    }
  );

  runtime.global().setProperty(runtime, "multiply", std::move(multiply));
}

void cleanUpSequel() {
  // intentionally left blank
}