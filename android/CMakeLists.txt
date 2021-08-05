cmake_minimum_required(VERSION 3.4.1)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 11)

# This flag is set on the example project build.gradle (in the ext variables)
# Needed to change the directories when building JUST the example project
# message ("DEV_CMAKE=${DEV_CMAKE}")

# if(DEV_CMAKE)
#   include_directories(
#     ../cpp
#     ../node_modules/react-native/React
#     ../node_modules/react-native/React/Base
#     ../node_modules/react-native/ReactCommon/jsi
#   )
  
#   add_library(sequel
#     SHARED

#     ../node_modules/react-native/ReactCommon/jsi/jsi/jsi.cpp
#     ../cpp/sequel.cpp
#     ../cpp/sequel.h
#     ../cpp/SequelResult.h
#     ../cpp/react-native-quick-sqlite.cpp
#     ../cpp/react-native-quick-sqlite.h
#     ../cpp/sqlite3.h
#     ../cpp/sqlite3.c
#     cpp-adapter.cpp
#   )
# else()
  include_directories(
    ../cpp
    ../../react-native/React
    ../../react-native/React/Base
    ../../react-native/ReactCommon/jsi
  )
  
  add_library(sequel
    SHARED
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
# endif()

target_link_libraries(sequel android log)