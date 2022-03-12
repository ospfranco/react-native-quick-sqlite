
//
// Created by Sergei Golishnikov on 08/03/2022.
//
#include <fbjni/fbjni.h>
#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <map>

class QuickSQLiteBridge : public facebook::jni::HybridClass<QuickSQLiteBridge>
{

public:
  static constexpr auto kJavaDescriptor = "Lcom/reactnativequicksqlite/QuickSQLiteBridge;";
  static facebook::jni::local_ref<jhybriddata> initHybrid(
      facebook::jni::alias_ref<jhybridobject> jThis,
      jlong jsContext,
      facebook::jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder);

  static void registerNatives();

  void installJSIBindings(
      jlong jsContext,
      facebook::jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder,
      jstring docPath);
  // void emitJs(jstring name, jstring data);

private:
  friend HybridBase;
  facebook::jni::global_ref<QuickSQLiteBridge::javaobject> javaPart_;
  facebook::jsi::Runtime *runtime_;
  std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker_;
  std::map<std::string, std::shared_ptr<facebook::jsi::Function>> webSocketCallbacks_;
  explicit QuickSQLiteBridge(
      facebook::jni::alias_ref<QuickSQLiteBridge::jhybridobject> jThis,
      facebook::jsi::Runtime *rt,
      std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker);
};
