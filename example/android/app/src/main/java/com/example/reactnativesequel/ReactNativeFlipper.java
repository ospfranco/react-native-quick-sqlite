package com.example.reactnativequicksqlite;

import android.content.Context;
import com.facebook.flipper.android.AndroidFlipperClient;
import com.facebook.flipper.android.utils.FlipperUtils;
import com.facebook.flipper.core.FlipperClient;
import com.facebook.flipper.plugins.inspector.DescriptorMapping;
import com.facebook.flipper.plugins.inspector.InspectorFlipperPlugin;
import com.facebook.react.ReactInstanceManager;
import okhttp3.OkHttpClient;
import com.facebook.flipper.plugins.databases.DatabasesFlipperPlugin;
import com.facebook.flipper.plugins.inspector.DescriptorMapping;
import com.facebook.flipper.plugins.inspector.InspectorFlipperPlugin;

public class ReactNativeFlipper {
    public static void initializeFlipper(Context context, ReactInstanceManager reactInstanceManager) {
        if (FlipperUtils.shouldEnableFlipper(context)) {
            final FlipperClient client = AndroidFlipperClient.getInstance(context);

            client.addPlugin(new InspectorFlipperPlugin(context, DescriptorMapping.withDefaults()));
            client.addPlugin(new DatabasesFlipperPlugin(context));

            final DescriptorMapping descriptorMapping = DescriptorMapping.withDefaults();
            client.addPlugin(new InspectorFlipperPlugin(context, descriptorMapping));
            client.start();
        }
    }
}