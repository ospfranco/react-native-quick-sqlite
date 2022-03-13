/*
 * SequelResult.h
 *
 * Created by Oscar Franco on 2021/03/07
 * Copyright (c) 2021 Oscar Franco
 *
 * This code is licensed under the MIT license
 */

#pragma once

#include <jsi/jsilib.h>
#include <jsi/jsi.h>
#include <string>
#include <any>

using namespace std;
using namespace facebook;

enum ResultType
{
  SequelResultOk,
  SequelResultError
};

struct SequelResult
{
  ResultType type;
  string message;
  jsi::Value value;
};

struct SequelLiteralUpdateResult
{
  ResultType type;
  string message;
  int affectedRows;
};

struct SequelBatchOperationResult
{
  ResultType type;
  string message;
  int affectedRows;
  int commands;
};

struct SQLiteValueWrapper
{
  string name;
  any value;
};
