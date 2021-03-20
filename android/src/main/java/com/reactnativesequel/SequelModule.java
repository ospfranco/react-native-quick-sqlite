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

    // LEFT HERE:
    // Convert the second arg into a std::string in the cpp-adapter file
    // https://stackoverflow.com/questions/41820039/jstringjni-to-stdstringc-with-utf8-characters
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