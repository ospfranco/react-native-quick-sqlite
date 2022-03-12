package com.reactnativequicksqlite;

import android.os.Handler;
import android.os.Looper;

import androidx.collection.ArrayMap;

import com.facebook.jni.HybridData;
import com.facebook.jni.annotations.DoNotStrip;
import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;

@SuppressWarnings("JavaJniMissingFunction")
public class QuickSQLiteBridge {
  @DoNotStrip
  @SuppressWarnings("unused")
  private HybridData mHybridData;

  public static final QuickSQLiteBridge instance = new QuickSQLiteBridge();

  public boolean install(ReactApplicationContext context) {
    try {
      JavaScriptContextHolder jsContext = context.getJavaScriptContextHolder();
      CallInvokerHolder jsCallInvokerHolder = context.getCatalystInstance().getJSCallInvokerHolder();
      mHybridData = initHybrid(jsContext.get(), (CallInvokerHolderImpl)jsCallInvokerHolder);
      
      installJSIBindings(
        jsContext.get(),
        (CallInvokerHolderImpl)jsCallInvokerHolder,
        context.getFilesDir().getAbsolutePath()
      );
      return true;
    } catch (Exception exception) {
      return false;
    }
  }

  private native void installJSIBindings(long jsContext, CallInvokerHolderImpl jsCallInvokerHolder, String docPath);
  private native HybridData initHybrid(long jsContext, CallInvokerHolderImpl jsCallInvokerHolder);

}