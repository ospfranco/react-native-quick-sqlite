/*
 * Sequel.mm
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the SSPL license
 * https://www.mongodb.com/licensing/server-side-public-license
 */

#import "Sequel.h"

#import <React/RCTBridge+Private.h>
#import <jsi/jsi.h>

#import <React/RCTUtils.h>
#import <ReactCommon/CallInvoker.h>

#import <memory>

// @interface RCTBridge (RCTTurboModule)
// - (std::shared_ptr<facebook::react::CallInvoker>)jsCallInvoker;
// @end

@implementation Sequel

@synthesize bridge=_bridge;
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

  // installSequel(*(facebook::jsi::Runtime *)cxxBridge.runtime, cxxBridge.jsCallInvoker);
  installSequel(*(facebook::jsi::Runtime *)cxxBridge.runtime);
}

- (void)invalidate {
  cleanUpSequel();
}

@end
