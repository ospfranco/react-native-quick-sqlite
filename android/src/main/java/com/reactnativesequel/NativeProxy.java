package com.reactnativesequel;

import android.util.Log;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;

public class NativeProxy {


  private native void installNativeJsi(long jsContextNativePointer, CallInvokerHolderImpl jsCallInvokerHolder, String docDir);
  public static final NativeProxy instance = new NativeProxy();

  public void installJsi(ReactContext context) {
    Log.d("rn-native-quick-sqlite", "Installing native...");
    CallInvokerHolderImpl holder = (CallInvokerHolderImpl)context.getCatalystInstance().getJSCallInvokerHolder();
    long contextPointer = context.getJavaScriptContextHolder().get();
    final String path = context.getFilesDir().getAbsolutePath();
    installNativeJsi(contextPointer, holder, path);
  }
}
