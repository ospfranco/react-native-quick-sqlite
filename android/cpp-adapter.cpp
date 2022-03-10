#include <jni.h>
#include <CallInvokerHolder.h>
#include <fbjni/fbjni.h>
#include "react-native-quick-sqlite.h"

extern "C" JNIEXPORT void JNICALL
Java_com_reactnativequicksqlite_SequelModule_initialize(JNIEnv *env,
                                                        jclass clazz,
                                                        jlong jsiPtr,
                                                        jni::alias_ref<react::CallInvokerHolder::javaobject> jsCallInvokerHolder,
                                                        jstring docPath)
{
  jboolean isCopy;
  const char *docPathString = (env)->GetStringUTFChars(docPath, &isCopy);
  auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();

  installSequel(*reinterpret_cast<facebook::jsi::Runtime *>(jsiPtr), jsCallInvoker, docPathString);
}

extern "C" JNIEXPORT void JNICALL
Java_com_reactnativequicksqlite_SequelModule_destruct(JNIEnv *env, jclass clazz)
{
  cleanUpSequel();
}