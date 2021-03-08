/*
 * sequel.h
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the SSPL license
 * https://www.mongodb.com/licensing/server-side-public-license
 */

#include <vector>
#include <jsi/jsilib.h>
#include <jsi/jsi.h>

#include "SequelResult.h"

using namespace std;
using namespace facebook;

SequelResult sequel_open(string const &dbName);

SequelResult sequel_close(string const &dbName);

SequelResult sequel_remove(string const &dbName);

vector<jsi::Object> sequel_execute(jsi::Runtime &rt, string const &dbName, string const &query);
