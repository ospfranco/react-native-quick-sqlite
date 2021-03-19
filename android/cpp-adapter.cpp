#include <jni.h>
#include "react-native-sequel.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativesequel_SequelModule_initialize(JNIEnv* env, jclass clazz, jlong jsiPtr) {
    installSequel(*reinterpret_cast<facebook::jsi::Runtime*>(jsiPtr));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_reactnativesequel_SequelModule_destruct(JNIEnv* env, jclass clazz) {
  cleanUpSequel();
}