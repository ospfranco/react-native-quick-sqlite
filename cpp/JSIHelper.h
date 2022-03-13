//
//  JSIHelper.hpp
//  react-native-quick-sqlite
//
//  Created by Oscar on 13.03.22.
//

#ifndef JSIHelper_h
#define JSIHelper_h

#include <stdio.h>
#include <jsi/jsilib.h>
#include <jsi/jsi.h>
#include <vector>
#include <map>
#include "SequelResult.h"

using namespace std;
using namespace facebook;

/**
 * Fill the target vector with parsed parameters
 * */
void jsiQueryArgumentsToSequelParam(jsi::Runtime &rt, jsi::Value const &args, vector<SequelValue> *target);

SequelValue createNullSequelValue();
SequelValue createBooleanSequelValue(bool value);
SequelValue createTextSequelValue(string value);
SequelValue createIntegerSequelValue(int value);
SequelValue createIntegerSequelValue(double value);
SequelValue createInt64SequelValue(long long value);
SequelValue createDoubleSequelValue(double value);
SequelValue createArrayBufferSequelValue(uint8_t *arrayBufferValue, size_t arrayBufferSize);
jsi::Value createSequelQueryExecutionResult(jsi::Runtime &rt, SequelOperationStatus status, vector<map<string,SequelValue>> *results);


#endif /* JSIHelper_h */
