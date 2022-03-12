#include <jni.h>
#include <fbjni/fbjni.h>
#include <jsi/jsi.h>
#include <ReactCommon/CallInvokerHolder.h>
#include "react-native-quick-sqlite.h"
#include "logs.h"
#include <typeinfo>

struct NativeProxy : jni::JavaClass<NativeProxy> {
    static constexpr auto kJavaDescriptor = "Lcom/reactnativesequel/NativeProxy;";

    static void registerNatives() {
        javaClassStatic()->registerNatives({
                //initialization for JSI
                makeNativeMethod("installNativeJsi", NativeProxy::installNativeJsi)
        });
    }

private:
    static void installNativeJsi(jni::alias_ref<jni::JObject> thiz,
                                 jlong jsiRuntimePtr,
                                 jni::alias_ref<react::CallInvokerHolder::javaobject> jsCallInvokerHolder,
                                 jni::alias_ref<jni::JString> docPath) {

        auto jsiRuntime = reinterpret_cast<jsi::Runtime*>(jsiRuntimePtr);
        auto jsCallInvoker = jsCallInvokerHolder->cthis()->getCallInvoker();
        std::string docPathString = docPath->toStdString();

        installSequel(*jsiRuntime, jsCallInvoker, docPathString.c_str());
    }
};

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
    return jni::initialize(vm, [] {
        NativeProxy::registerNatives();
    });
}
