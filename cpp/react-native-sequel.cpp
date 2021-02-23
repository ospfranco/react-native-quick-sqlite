#import "react-native-sequel.h"

#include <iostream>
#include <sstream>

using namespace facebook;

void installSequel(jsi::Runtime& jsiRuntime) {
  std::cout << "Initializing react-native-sequel" << "\n";

  auto multiply = jsi::Function::createFromHostFunction(
    jsiRuntime,
    jsi::PropNameID::forAscii(jsiRuntime, "multiply"),
    1, // WHAT IS THIS??!!
    [](jsi::Runtime& runtime, const jsi::Value& thisValue, const jsi::Value* arguments, size_t count) -> jsi::Value {
      if(!arguments[0].isNumber() || !arguments[1].isNumber()) {
        jsi::detail::throwJSError(runtime, "Non number arguments passed to sequel");
      }

      double res = 42;

      // return jsi::Value(arguments[0].asNumber(runtime) * arguments[1].asNumber(runtime));
      return jsi::Value(res);
    }
  );

  jsiRuntime.global().setProperty(jsiRuntime, "multiply", std::move(multiply));
}

void cleanUpSequel() {
  // intentionally left blank
}