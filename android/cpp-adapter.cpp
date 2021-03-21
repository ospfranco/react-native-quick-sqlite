#include <jni.h>
#include "react-native-quick-sqlite.h"

extern "C" JNIEXPORT void JNICALL
Java_com_reactnativequicksqlite_SequelModule_initialize(JNIEnv *env, jclass clazz, jlong jsiPtr, jstring docPath)
{
  jboolean isCopy;
  const char *docPathString = (env)->GetStringUTFChars(docPath, &isCopy);

  installSequel(*reinterpret_cast<facebook::jsi::Runtime *>(jsiPtr), docPathString);
}

extern "C" JNIEXPORT void JNICALL
Java_com_reactnativequicksqlite_SequelModule_destruct(JNIEnv *env, jclass clazz)
{
  cleanUpSequel();
}