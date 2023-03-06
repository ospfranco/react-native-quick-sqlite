package com.reactnativequicksqlite;

import androidx.annotation.NonNull;

import com.facebook.react.ReactPackage;
import com.facebook.react.TurboReactPackage;
import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.module.annotations.ReactModule;
import com.facebook.react.module.annotations.ReactModuleList;
import com.facebook.react.module.model.ReactModuleInfo;
import com.facebook.react.module.model.ReactModuleInfoProvider;
import com.facebook.react.turbomodule.core.interfaces.TurboModule;
import com.facebook.react.uimanager.ViewManager;

import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.annotation.Nonnull;


@ReactModuleList(
        nativeModules = {
                SequelModule.class,
        })
public class SequelPackage extends TurboReactPackage {
  @NonNull
  @Override
  public List<NativeModule> createNativeModules(@NonNull ReactApplicationContext reactContext) {
    return Collections.singletonList(new SequelModule(reactContext));
  }

  @Override
  public NativeModule getModule(String name, @Nonnull ReactApplicationContext reactContext) {
    switch (name) {
      case SequelModule.NAME:
        return new SequelModule(reactContext);
      default:
        return null;
    }
  }

  @Override
  public ReactModuleInfoProvider getReactModuleInfoProvider() {
    try {
      Class<?> reactModuleInfoProviderClass =
              Class.forName("com.reactnativequicksqlite.SequelPackage$$ReactModuleInfoProvider");
      return (ReactModuleInfoProvider) reactModuleInfoProviderClass.newInstance();
    } catch (ClassNotFoundException e) {
      // ReactModuleSpecProcessor does not run at build-time. Create this ReactModuleInfoProvider by
      // hand.
      return new ReactModuleInfoProvider() {
        @Override
        public Map<String, ReactModuleInfo> getReactModuleInfos() {
          final Map<String, ReactModuleInfo> reactModuleInfoMap = new HashMap<>();

          Class<? extends NativeModule>[] moduleList =
                  new Class[] {
                          SequelModule.class,
                  };

          for (Class<? extends NativeModule> moduleClass : moduleList) {
            ReactModule reactModule = moduleClass.getAnnotation(ReactModule.class);

            reactModuleInfoMap.put(
                    reactModule.name(),
                    new ReactModuleInfo(
                            reactModule.name(),
                            moduleClass.getName(),
                            reactModule.canOverrideExistingModule(),
                            reactModule.needsEagerInit(),
                            reactModule.hasConstants(),
                            reactModule.isCxxModule(),
                            TurboModule.class.isAssignableFrom(moduleClass)));
          }

          return reactModuleInfoMap;
        }
      };
    } catch (InstantiationException | IllegalAccessException e) {
      throw new RuntimeException(
              "No ReactModuleInfoProvider for com.reactnativequicksqlite.SequelPackage$$ReactModuleInfoProvider", e);
    }
  }

  @NonNull
  @Override
  public List<ViewManager> createViewManagers(@NonNull ReactApplicationContext reactContext) {
    return Collections.emptyList();
  }
}