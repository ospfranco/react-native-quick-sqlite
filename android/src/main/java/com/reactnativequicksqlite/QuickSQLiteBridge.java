package com.reactnativequicksqlite;

import android.util.Log;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContext;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;


public class QuickSQLiteBridge {
  private native void installNativeJsi(long jsContextNativePointer, CallInvokerHolderImpl jsCallInvokerHolder, String docPath);
  public static final QuickSQLiteBridge instance = new QuickSQLiteBridge();

  public void install(ReactContext context) {
    // try {
      Log.d("react-native-quick-sqlite", "Installing native...");
      
      long jsContextPointer = context.getJavaScriptContextHolder().get();
      Log.d("react-native-quick-sqlite", "Installing native 2 ...");

      CallInvokerHolderImpl jsCallInvokerHolder = (CallInvokerHolderImpl)context.getCatalystInstance().getJSCallInvokerHolder();
      Log.d("react-native-quick-sqlite", "Installing native 3...");

      final String path = context.getFilesDir().getAbsolutePath();
      
      Log.d("react-native-quick-sqlite", "Installing native4...");

      installNativeJsi(
        jsContextPointer,
        jsCallInvokerHolder,
        path
      );
      
    // } catch (Exception exception) {
    //   return false;
    // }
  }
}