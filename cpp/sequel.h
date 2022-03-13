/*
 * sequel.h
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the MIT license
 */

#include <jsi/jsilib.h>
#include <jsi/jsi.h>
#include "SequelResult.h"
#include <vector>
#include <any>

using namespace std;
using namespace facebook;

SequelResult sequel_open(string const dbName, string const docPath);

SequelResult sequel_close(string const dbName);

SequelResult sequel_remove(string const dbName, string const docPath);

//SequelResult sequel_attach(string const &dbName);

SequelResult sequel_execute(jsi::Runtime &rt, string const dbName, string const &query, jsi::Value const &params);

vector<vector<SQLiteValueWrapper>> sequel_execute2(string const dbName, string const &query, const vector<any> &params);

SequelLiteralUpdateResult sequel_execute_literal_update(string const dbName, string const &query);
