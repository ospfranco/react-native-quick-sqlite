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

using namespace std;
using namespace facebook;

enum SequelDataType {
  NULL_VALUE,
  TEXT,
  INTEGER,
  INT64, 
  DOUBLE,
  BOOLEAN,
  ARRAY_BUFFER,
};

/**
 * TypeSafe dynamic parameter value to bind on sqlite statements
 */
struct SequelValue {
  SequelDataType dataType;
  int booleanValue;
  double doubleOrIntValue;
  long long int64Value;
  string textValue;
  shared_ptr<uint8_t> arrayBufferValue;
  size_t arrayBufferSize;
};

/**
 * TypeSafe dynamic column value holder representing a sqlite columnValue
 */
struct SequelColumnValue {
  SequelValue value;
  string columnName;
};

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

struct SequelOperationStatus
{
  ResultType type;
  string errorMessage;
  int rowsAffected;
  double insertId;
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

