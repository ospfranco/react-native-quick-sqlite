//
//  JSIHelper.cpp
//  react-native-quick-sqlite
//
//  Created by Oscar on 13.03.22.
//

#include "JSIHelper.h"

using namespace std;
using namespace facebook;

vector<any> jsiArrayToVector(jsi::Runtime &rt, jsi::Array values)
{
//  int jsiParamsLength = params.length(rt);
//  vector<string> res;
//  for (int ii = 0; ii < jsiParamsLength; ii++)
//  {
//    res.push_back(params.getValueAtIndex(rt, ii).asString(rt).utf8(rt));
//  }
//  return res;
  vector<any> res;
  
  for (int ii = 0; ii < values.length(rt); ii++)
  {
    jsi::Value value = values.getValueAtIndex(rt, ii);
    if (value.isNull())
    {
      res.push_back(nullptr);
//      sqlite3_bind_null(statement, ii + 1);
    }
    else if (value.isBool())
    {
      res.push_back(value.getBool());
//      int intVal = int(value.getBool());
//      sqlite3_bind_int(statement, ii + 1, intVal);
    }
    else if (value.isNumber())
    {
      res.push_back(value.asNumber());
//      double doubleVal = value.asNumber();
//      int intVal = (int)doubleVal;
//      long long longVal = (long)doubleVal;
//      if (intVal == doubleVal)
//      {
//        sqlite3_bind_int(statement, ii + 1, intVal);
//      }
//      else if (longVal == doubleVal)
//      {
//        sqlite3_bind_int64(statement, ii + 1, longVal);
//      }
//      else
//      {
//        sqlite3_bind_double(statement, ii + 1, doubleVal);
//      }
    }
    else if (value.isString())
    {
      res.push_back(value.asString(rt).utf8(rt));
//      string strVal = value.asString(rt).utf8(rt);
//
//      sqlite3_bind_text(statement, ii + 1, strVal.c_str(), strVal.length(), SQLITE_TRANSIENT);
    }
    else if (value.isObject())
    {
//      auto obj = value.asObject(rt);
//      if (obj.isArrayBuffer(rt))
//      {
//        auto buf = obj.getArrayBuffer(rt);
//        // The statement is executed before returning control to JSI, so we don't need to copy the data to extend its lifetime.
//        sqlite3_bind_blob(statement, ii + 1, buf.data(rt), buf.size(rt), SQLITE_STATIC);
//      }
    }
  }
  
  
  
  return res;
}
