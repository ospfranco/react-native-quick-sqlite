//
//  JSIHelper.cpp
//  react-native-quick-sqlite
//
//  Created by Oscar on 13.03.22.
//

#include "JSIHelper.h"

using namespace std;
using namespace facebook;

SequelValue createNullSequelValue() {
  return SequelValue {
    .dataType = NULL_VALUE
  };
}

SequelValue createBooleanSequelValue(bool value) {
  return SequelValue {
    .dataType = BOOLEAN,
    .booleanValue = int(value)
  };
}

SequelValue createTextSequelValue(string value) {
  return SequelValue {
    .dataType = TEXT,
    .textValue = value
  };
}

SequelValue createIntegerSequelValue(int value) {
  return SequelValue {
    .dataType = INTEGER,
    .doubleOrIntValue = static_cast<double>(value)
  };
}

SequelValue createIntegerSequelValue(double value) {
  return SequelValue {
    .dataType = INTEGER,
    .doubleOrIntValue = value
  };
}

SequelValue createInt64SequelValue(long long value) {
  return SequelValue {
    .dataType = INT64,
    .int64Value = value
  };
}

SequelValue createDoubleSequelValue(double value) {
  return SequelValue {
    .dataType = DOUBLE,
    .doubleOrIntValue = value
  };
}

SequelValue createArrayBufferSequelValue(uint8_t *arrayBufferValue, size_t arrayBufferSize) {
  return SequelValue {
    .dataType = ARRAY_BUFFER,
    .arrayBufferValue = shared_ptr<uint8_t>{arrayBufferValue},
    .arrayBufferSize = arrayBufferSize
  };
}

void jsiQueryArgumentsToSequelParam(jsi::Runtime &rt, jsi::Value const &params, vector<SequelValue> *target) 
{
  if (params.isNull() || params.isUndefined())
  {
    return;
  }

  jsi::Array values = params.asObject(rt).asArray(rt);

  for (int ii = 0; ii < values.length(rt); ii++)
  {

    jsi::Value value = values.getValueAtIndex(rt, ii);
    if (value.isNull())
    {
      target->push_back(createNullSequelValue());
    }
    else if (value.isBool())
    {
      int intVal = int(value.getBool());
      target->push_back(createBooleanSequelValue(value.getBool()));
    }
    else if (value.isNumber())
    {
      double doubleVal = value.asNumber();
      int intVal = (int)doubleVal;
      long long longVal = (long)doubleVal;
      if (intVal == doubleVal)
      {
        target->push_back(createIntegerSequelValue(intVal));
      }
      else if (longVal == doubleVal)
      {
        target->push_back(createInt64SequelValue(longVal));
      }
      else
      {
        target->push_back(createDoubleSequelValue(doubleVal));
      }
    }
    else if (value.isString())
    {
      string strVal = value.asString(rt).utf8(rt);
      target->push_back(createTextSequelValue(strVal));
    }
    else if (value.isObject())
    {
      auto obj = value.asObject(rt);
      if (obj.isArrayBuffer(rt))
      {
        auto buf = obj.getArrayBuffer(rt);
        target->push_back(createArrayBufferSequelValue(buf.data(rt), buf.size(rt)));
      }
    }
  }
}

jsi::Value createSequelQueryExecutionResult(jsi::Runtime &rt, SequelOperationStatus status, vector<map<string,SequelValue>> *results) 
{
  jsi::Object res = jsi::Object(rt);
  if(status.type == SequelResultOk)
  {
    //res.setProperty(rt, "rows", move(rows));
    res.setProperty(rt, "rowsAffected", jsi::Value(status.rowsAffected));
    if (status.rowsAffected > 0 && status.insertId != 0)
    {
      res.setProperty(rt, "insertId", jsi::Value(status.insertId));
    }

    // Converting row results into objects
    size_t rowCount = results->size();
    jsi::Object rows = jsi::Object(rt);
    if(rowCount > 0)
    {
      auto array = jsi::Array(rt, rowCount);
      for(int i = 0; i<rowCount; i++)
      {
        jsi::Object rowObject = jsi::Object(rt);
        auto row = results->at(i);
        for (auto const& entry : row)
        {
          std::string columnName = entry.first;
          SequelValue value = entry.second;
          if (value.dataType == TEXT)
          {
            rowObject.setProperty(rt, columnName.c_str(), jsi::String::createFromUtf8(rt, value.textValue.c_str()));
          }
          else if (value.dataType == INTEGER)
          {
            rowObject.setProperty(rt, columnName.c_str(), jsi::Value(value.doubleOrIntValue));
          }
          else if (value.dataType == DOUBLE)
          {
            rowObject.setProperty(rt, columnName.c_str(), jsi::Value(value.doubleOrIntValue));
          }
          else if (value.dataType == ARRAY_BUFFER)
          {
            jsi::Function array_buffer_ctor = rt.global().getPropertyAsFunction(rt, "ArrayBuffer");
            jsi::Object o = array_buffer_ctor.callAsConstructor(rt, (int)value.arrayBufferSize).getObject(rt);
            jsi::ArrayBuffer buf = o.getArrayBuffer(rt);
            // It's a shame we have to copy here: see https://github.com/facebook/hermes/pull/419 and https://github.com/facebook/hermes/issues/564.
            memcpy(buf.data(rt), value.arrayBufferValue.get(), value.arrayBufferSize);
            rowObject.setProperty(rt, columnName.c_str(), o);
          }
          else
          {
            rowObject.setProperty(rt, columnName.c_str(), jsi::Value(nullptr));
          }
        }
        array.setValueAtIndex(rt, i, move(rowObject));
      }
      rows.setProperty(rt, "_array", move(array));
      res.setProperty(rt, "rows", move(rows));
    }
    rows.setProperty(rt, "status", jsi::Value(0));
    rows.setProperty(rt, "length", jsi::Value((int)rowCount));
  }
  else
  {
    res.setProperty(rt, "status", jsi::Value(1));
    res.setProperty(rt, "message", jsi::String::createFromUtf8(rt, status.errorMessage.c_str()));
  }

  return move(res);
}