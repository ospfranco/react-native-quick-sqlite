#import <React/RCTBridgeModule.h>
#import "react-native-sequel.h"

@interface Sequel : NSObject <RCTBridgeModule>

@property (nonatomic, assign) BOOL setBridgeOnMainQueue;

@end
