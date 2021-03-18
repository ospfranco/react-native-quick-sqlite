package com.reactnativesequel;

import androidx.annotation.NonNull;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;

class SequelModule extends ReactContextBaseJavaModule {
  static {
    System.loadLibrary("sequel");
  }

  private static native void initialize(long jsiPtr, String docDir);
  private static native void destruct();

  public SequelModule(ReactApplicationContext reactContext) {
    super(reactContext);
  }

  @NonNull
  @Override
  public String getName() {
    return "Sequel";
  }


  @NonNull
  @Override
  public void initialize() {
    super.initialize();

    SequelModule.initialize(
      this.getReactApplicationContext().getJavaScriptContextHolder().get(),
      this.getReactApplicationContext().getFilesDir().getAbsolutePath()
    );
  }

  @Override
  public void onCatalystInstanceDestroy() {
    SequelModule.destruct();
  }
}