/*
 * Sequel.mm
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the MIT license
 */

#import "Sequel.h"
#import "installer.h"

#import <React/RCTBridge+Private.h>
#import <jsi/jsi.h>
#import <ReactCommon/RCTTurboModuleManager.h>

#import <React/RCTUtils.h>
#import <ReactCommon/CallInvoker.h>

#import <memory>

@implementation Sequel

@synthesize bridge = _bridge;
@synthesize methodQueue = _methodQueue;

RCT_EXPORT_MODULE()

+ (BOOL)requiresMainQueueSetup {
  return YES;
}

- (void)setBridge:(RCTBridge *)bridge {
  _bridge = bridge;
  _setBridgeOnMainQueue = RCTIsMainQueue();

  RCTCxxBridge *cxxBridge = (RCTCxxBridge *)self.bridge;
  if (!cxxBridge.runtime) {
    return;
  }
  
  auto callInvoker = bridge.jsCallInvoker;

  // Get iOS app's document directory (to safely store database .sqlite3 file)
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, true);
  NSString *documentPath = [paths objectAtIndex:0];

  install(*(facebook::jsi::Runtime *)cxxBridge.runtime, callInvoker,[documentPath UTF8String]);
}

@end
