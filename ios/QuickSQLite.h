#ifdef RCT_NEW_ARCH_ENABLED
#import <reactnativequicksqlite/reactnativequicksqlite.h>
#else
#import <React/RCTBridge.h>
#endif

@interface QuickSQLite : NSObject
#ifdef RCT_NEW_ARCH_ENABLED
                                   <NativeQuickSQLiteModuleSpec>
#else
                                   <RCTBridgeModule>
#endif

@property(nonatomic, assign) BOOL setBridgeOnMainQueue;

@end
