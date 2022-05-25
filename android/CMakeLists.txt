cmake_minimum_required(VERSION 3.9.0)

#set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 14)

set (PACKAGE_NAME "react-native-quick-sqlite")
set (BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set (RN_SO_DIR ${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/jni/first-party/react/jni)

if(${REACT_NATIVE_VERSION} LESS 66)
        set (
                INCLUDE_JSI_CPP
                "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi/jsi/jsi.cpp"
        )
        set (
                INCLUDE_JSIDYNAMIC_CPP
                "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi/jsi/JSIDynamic.cpp"
        )
endif()

file (GLOB LIBFBJNI_INCLUDE_DIR "${BUILD_DIR}/fbjni-*-headers.jar/")

include_directories(
        ../cpp
        "${NODE_MODULES_DIR}/react-native/React"
        "${NODE_MODULES_DIR}/react-native/React/Base"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/callinvoker"
        "${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/java/com/facebook/react/turbomodule/core/jni"
        "${NODE_MODULES_DIR}/react-native/ReactCommon"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/callinvoker"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
        "${NODE_MODULES_DIR}/hermes-engine/android/include/"
        ${INCLUDE_JSI_CPP} # only on older RN versions
        ${INCLUDE_JSIDYNAMIC_CPP} # only on older RN versions
)

if($ENV{QUICK_SQLITE_USE_PHONE_VERSION})
        message(STATUS "Using phone's sqlite")
        add_library(
                ${PACKAGE_NAME}
                SHARED
                ../cpp/sqliteBridge.cpp
                ../cpp/sqliteBridge.h
                ../cpp/installer.cpp
                ../cpp/installer.h
                ../cpp/sqlite3.h
                ../cpp/JSIHelper.h
                ../cpp/JSIHelper.cpp
                ../cpp/ThreadPool.h
                ../cpp/ThreadPool.cpp
                ../cpp/sqlfileloader.h
                ../cpp/sqlfileloader.cpp
                ../cpp/sqlbatchexecutor.h
                ../cpp/sqlbatchexecutor.cpp
                cpp-adapter.cpp
        )
else()
        message(STATUS "Embedding sqlite")
        add_library(
                ${PACKAGE_NAME}
                SHARED
                ../cpp/sqliteBridge.cpp
                ../cpp/sqliteBridge.h
                ../cpp/installer.cpp
                ../cpp/installer.h
                ../cpp/sqlite3.h
                ../cpp/sqlite3.c
                ../cpp/JSIHelper.h
                ../cpp/JSIHelper.cpp
                ../cpp/ThreadPool.h
                ../cpp/ThreadPool.cpp
                ../cpp/sqlfileloader.h
                ../cpp/sqlfileloader.cpp
                ../cpp/sqlbatchexecutor.h
                ../cpp/sqlbatchexecutor.cpp
                cpp-adapter.cpp
        )
endif()

# find fbjni package
file (GLOB LIBFBJNI_INCLUDE_DIR "${BUILD_DIR}/fbjni-*-headers.jar/")

target_include_directories(
        ${PACKAGE_NAME}
        PRIVATE
        # --- fbjni ---
        "${LIBFBJNI_INCLUDE_DIR}"
        # --- React Native ---
        "${NODE_MODULES_DIR}/react-native/React"
        "${NODE_MODULES_DIR}/react-native/React/Base"
        "${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/jni"
        "${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/java/com/facebook/react/turbomodule/core/jni"
        "${NODE_MODULES_DIR}/react-native/ReactCommon"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/callinvoker"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
        "${NODE_MODULES_DIR}/hermes-engine/android/include/"
        ${INCLUDE_JSI_CPP} # only on older RN versions
        ${INCLUDE_JSIDYNAMIC_CPP} # only on older RN versions
)

file (GLOB LIBRN_DIR "${BUILD_DIR}/react-native-0*/jni/${ANDROID_ABI}")

find_library(
        FBJNI_LIB
        fbjni
        PATHS ${LIBRN_DIR}
        NO_CMAKE_FIND_ROOT_PATH
)

find_library(
        REACT_NATIVE_JNI_LIB
        reactnativejni
        PATHS ${LIBRN_DIR}
        NO_CMAKE_FIND_ROOT_PATH
)

if(${REACT_NATIVE_VERSION} LESS 66)
        # JSI lib didn't exist on RN 0.65 and before. Simply omit it.
        set (JSI_LIB "")
else()
        # RN 0.66 distributes libjsi.so, can be used instead of compiling jsi.cpp manually.
        find_library(
                JSI_LIB
                jsi
                PATHS ${LIBRN_DIR}
                NO_CMAKE_FIND_ROOT_PATH
        )
endif()

find_library(
        LOG_LIB
        log
)

if($ENV{QUICK_SQLITE_USE_PHONE_VERSION})
        find_package (SQLite3)
        include_directories(${SQLite3_INCLUDE_DIRS})
        target_link_libraries (${PACKAGE_NAME} ${SQLite3_LIBRARIES})
endif()

target_link_libraries(
        ${PACKAGE_NAME}
        ${LOG_LIB}
        ${JSI_LIB}
        ${REACT_NATIVE_JNI_LIB}
        ${FBJNI_LIB}
        android
)
