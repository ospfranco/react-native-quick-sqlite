//
// Created by Sergei Golishnikov on 08/03/2022.
//

#include "QuickSQLiteBridge.h"

#include <utility>
#include "iostream"
#include "react-native-quick-sqlite.h"
#include <typeinfo>

using namespace facebook;
using namespace facebook::jni;

using TSelf = local_ref<HybridClass<QuickSQLiteBridge>::jhybriddata>;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *)
{
  return facebook::jni::initialize(vm, []
                                   { QuickSQLiteBridge::registerNatives(); });
}

// JNI binding
void QuickSQLiteBridge::registerNatives()
{
  __android_log_print(ANDROID_LOG_VERBOSE, "😇", "registerNatives");
  registerHybrid({
      makeNativeMethod("initHybrid",
                       QuickSQLiteBridge::initHybrid),
      makeNativeMethod("installJSIBindings",
                       QuickSQLiteBridge::installJSIBindings),
  });
}

QuickSQLiteBridge::QuickSQLiteBridge(
    jni::alias_ref<QuickSQLiteBridge::javaobject> jThis,
    jsi::Runtime *rt,
    std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker)
    : javaPart_(jni::make_global(jThis)),
      runtime_(rt),
      jsCallInvoker_(std::move(jsCallInvoker)) {}

// JNI init
TSelf QuickSQLiteBridge::initHybrid(
    alias_ref<jhybridobject> jThis,
    jlong jsContext,
    jni::alias_ref<facebook::react::CallInvokerHolder::javaobject>
        jsCallInvokerHolder)
{

  __android_log_write(ANDROID_LOG_INFO, "🥲", "initHybrid...");
  auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
  return makeCxxInstance(jThis, (jsi::Runtime *)jsContext, jsCallInvoker);
}

void QuickSQLiteBridge::installJSIBindings(
    jlong jsContext,
    // jni::alias_ref<facebook::react::CallInvokerHolder::javaobject> jsCallInvokerHolder,
    jni::alias_ref<jni::JString> docPath)
{
  __android_log_print(ANDROID_LOG_VERBOSE, "😇", "installJSIBindings");
  // auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
  std::string docPathString = docPath->toStdString();
  // LOGW("INSTALL SEQUEL BEING CALLED %s", docPathString);
  installSequel(*reinterpret_cast<facebook::jsi::Runtime *>(jsContext), jsCallInvoker_, docPathString.c_str());
}