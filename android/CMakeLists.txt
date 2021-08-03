cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 11)

# message ( ---------------ROPO---------- )
# message( Path ${CMAKE_CURRENT_LIST_DIR})

include_directories(
  ../cpp
  # If you want to run the example project you have to uncomment these
  # and comment the ones below
  # on a real deployment the dependency is all on the node_modules directory
  # on the example project the dependencies are on the root folder
  # ../node_modules/react-native/React
  # ../node_modules/react-native/React/Base
  # ../node_modules/react-native/ReactCommon/jsi
  ../../react-native/React
  ../../react-native/React/Base
  ../../react-native/ReactCommon/jsi
)

add_library(sequel
  SHARED
  # If you want to run the example project you have to uncomment these
  # and comment the ONE below
  # ../node_modules/react-native/ReactCommon/jsi/jsi/jsi.cpp
  ../../react-native/ReactCommon/jsi/jsi/jsi.cpp
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