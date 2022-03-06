cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 11)

# Uncomment the following lines to compile the example project
include_directories(
  ../cpp
  ../node_modules/react-native/React
  ../node_modules/react-native/React/Base
  ../node_modules/react-native/ReactCommon/jsi
)

add_library(sequel
  SHARED

  ../node_modules/react-native/ReactCommon/jsi/jsi/jsi.cpp
  ../cpp/sequel.cpp
  ../cpp/sequel.h
  ../cpp/SequelResult.h
  ../cpp/react-native-quick-sqlite.cpp
  ../cpp/react-native-quick-sqlite.h
  ../cpp/sqlite3.h
  ../cpp/sqlite3.c
  cpp-adapter.cpp
)

# include_directories(
#   ../cpp
#   ../../react-native/React
#   ../../react-native/React/Base
#   ../../react-native/ReactCommon/jsi
# )

# add_library(sequel
#   SHARED
#   ../../react-native/ReactCommon/jsi/jsi/jsi.cpp
#   ../cpp/sequel.cpp
#   ../cpp/sequel.h
#   ../cpp/SequelResult.h
#   ../cpp/react-native-quick-sqlite.cpp
#   ../cpp/react-native-quick-sqlite.h
#   ../cpp/sqlite3.h
#   ../cpp/sqlite3.c
#   cpp-adapter.cpp
# )

target_link_libraries(sequel android log)
