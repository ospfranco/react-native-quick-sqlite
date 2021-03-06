#include <jsi/jsilib.h>
#include <jsi/jsi.h>
#import <ReactCommon/CallInvoker.h>

using namespace facebook;

void installSequel(jsi::Runtime& rt, std::shared_ptr<react::CallInvoker> callInvoker);

void cleanUpSequel();
