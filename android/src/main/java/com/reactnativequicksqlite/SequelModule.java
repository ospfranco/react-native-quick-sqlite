package com.reactnativequicksqlite;

import androidx.annotation.NonNull;
import android.util.Log;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;

@ReactModule(name = SequelModule.NAME)
class SequelModule extends NativeQuickSQLiteModuleSpec {
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
      QuickSQLiteBridge.instance.install(getReactApplicationContext());
      return true;
    } catch (Exception exception) {
      Log.e(NAME, "Failed to install JSI Bindings!", exception);
      return false;
    }
  }
}