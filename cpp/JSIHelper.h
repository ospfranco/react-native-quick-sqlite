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
#include <any>

using namespace std;
using namespace facebook;

vector<any> jsiArrayToVector(jsi::Runtime &rt, jsi::Array values);

#endif /* JSIHelper_h */
