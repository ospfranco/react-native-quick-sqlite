package com.reactnativequicksqlite;

import androidx.annotation.NonNull;
import android.util.Log;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.reactnativesequel.NativeProxy;

class SequelModule extends ReactContextBaseJavaModule {
  public static final String NAME = "QuickSQLite";

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
      NativeProxy.instance.installJsi(getReactApplicationContext());
      return true;
    } catch (Exception exception) {
      return false;
    }
  }
}
