#import "QuickSQLite.h"

#import <React/RCTBridge+Private.h>

#import <React/RCTUtils.h>
#import <ReactCommon/RCTTurboModule.h>
#import <jsi/jsi.h>

#import "../cpp/bindings.h"

@implementation QuickSQLite

RCT_EXPORT_MODULE()


RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(install) {
  NSLog(@"Installing QuickSQLite module...");

  RCTBridge *bridge = [RCTBridge currentBridge];
  RCTCxxBridge *cxxBridge = (RCTCxxBridge *)bridge;
  if (cxxBridge == nil) {
    return @false;
  }

  using namespace facebook;

  auto jsiRuntime = (jsi::Runtime *)cxxBridge.runtime;
  if (jsiRuntime == nil) {
    return @false;
  }
  auto &runtime = *jsiRuntime;
  auto callInvoker = bridge.jsCallInvoker;

  // Get iOS app's document directory (to safely store database .sqlite3 file)
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, true);
  NSString *documentPath = [paths objectAtIndex:0];

  osp::install(runtime, callInvoker,[documentPath UTF8String]);
  return @true;
}

#if RCT_NEW_ARCH_ENABLED
- (std::shared_ptr<facebook::react::TurboModule>)getTurboModule:
    (const facebook::react::ObjCTurboModule::InitParams &)params
{
  return std::make_shared<facebook::react::NativeQuickSQLiteModuleSpecJSI>(params);
}
#endif

@end
