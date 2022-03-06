cmake_minimum_required(VERSION 3.9.0)

set (PACKAGE_NAME "react-native-quick-sqlite")
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 11)

include_directories(
  ../cpp
  "${NODE_MODULES_DIR}/react-native/React"
  "${NODE_MODULES_DIR}/react-native/React/Base"
  "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
)

add_library(sequel
  SHARED
  "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi/jsi/jsi.cpp"
  ../cpp/sequel.cpp
  ../cpp/sequel.h
  ../cpp/SequelResult.h
  ../cpp/react-native-quick-sqlite.cpp
  ../cpp/react-native-quick-sqlite.h
  ../cpp/sqlite3.h
  ../cpp/sqlite3.c
  cpp-adapter.cpp
)

target_link_libraries(sequel android log)
