/*
 * react-native-quick-sqlite.h
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the MIT license
 */

#include <jsi/jsilib.h>
#include <jsi/jsi.h>
// #include <ReactCommon/CallInvoker.h>

using namespace facebook;

// void installSequel(jsi::Runtime& rt, std::shared_ptr<react::CallInvoker> callInvoker);
void installSequel(jsi::Runtime &rt, const char *docPath);

void cleanUpSequel();
