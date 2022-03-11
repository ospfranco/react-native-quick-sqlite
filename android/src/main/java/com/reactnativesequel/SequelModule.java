package com.reactnativequicksqlite;

import androidx.annotation.NonNull;
import android.util.Log;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;

class SequelModule extends ReactContextBaseJavaModule {
  public static final String NAME = "QuickSQLite";
  private static native void initialize(long jsiPtr, CallInvokerHolderImpl jsCallInvokerHolder, String docDir);

  public SequelModule(ReactApplicationContext context) {
    super(context);
  }

  @NonNull
  @Override
  public String getName() {
    return NAME;
  }

  @ReactMethod(isBlockingSynchronousMethod = true)
  public boolean install() {
    try {
      System.loadLibrary("react-native-quick-sqlite");
      
      ReactApplicationContext context = getReactApplicationContext();
      CallInvokerHolderImpl holder = (CallInvokerHolderImpl)context.getCatalystInstance().getJSCallInvokerHolder();

      initialize(
        context.getJavaScriptContextHolder().get(),
        holder,
        context.getFilesDir().getAbsolutePath()
      );
      return true;
    } catch (Exception exception) {
      return false;
    }
  }
}