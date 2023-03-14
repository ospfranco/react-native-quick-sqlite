#include<string>
#include<sqlite3.h>
#include "JSIHelper.h"
#include<CustomFunction.h>

using namespace std;
using namespace facebook;

namespace osp {
    CustomFunction::CustomFunction(
                                   jsi::Runtime& rt,
                                   const string name,
                                   const std::shared_ptr<jsi::Function> fn
                                   ) :
    rt(rt),
    name(name),
    fn(fn)
    {
    }

    CustomFunction::~CustomFunction() {}

    void CustomFunction::xDestroy(void* self) {
        delete static_cast<CustomFunction*>(self);
    }

    jsi::Value* CustomFunction::getArguments(jsi::Runtime& rt, sqlite3_value** argv, int argc, jsi::Value* value, const size_t startIndex) {
        if (value == nullptr) {
            value = (jsi::Value*) malloc(sizeof(jsi::Value) * argc);
        }
        
        for (int i = startIndex; i < startIndex + argc; i++) {
            int type = sqlite3_value_type(argv[i]);
            switch (type) {
                case SQLITE_BLOB:
                case SQLITE_TEXT:
                    value[i] = jsi::String::createFromAscii(rt, (char *) sqlite3_value_blob(argv[i]));
                    break;
                case SQLITE_INTEGER:
                    value[i] = sqlite3_value_int(argv[i]);
                    break;
                case SQLITE_FLOAT:
                    value[i] = sqlite3_value_double(argv[i]);
                    break;
                default:
                    value[i] = nullptr;
                    break;
            }
        }
        
        return value;
    }

    void CustomFunction::xFunc(sqlite3_context* invocation, int argc, sqlite3_value** argv) {
        CustomFunction* self = (CustomFunction*) sqlite3_user_data(invocation);
        
        jsi::Value value[argc];
        CustomFunction::getArguments(self->rt, argv, argc, value);
        try {
            const jsi::Value mayBeResult = self->fn->call(self->rt, value, argc);
            if (!isEmpty(self->rt, &mayBeResult)) {
                CustomFunction::jsToSqliteValue(mayBeResult, self->rt, invocation);
                return;
            }
            
            CustomFunction::PropagateJSError (invocation, "");
        } catch (const exception& e) {
            CustomFunction::PropagateJSError (invocation, e.what());
        }
    }


    jsi::Value CustomFunction::copyValue (jsi::Runtime& rt, jsi::Value& value) {
        if (value.isNumber()) {
            return jsi::Value(value.asNumber());
        } else if (value.isBigInt()) {
            return jsi::Value(value.asBigInt(rt));
        } else if (value.isString()) {
            return jsi::Value(jsi::String::createFromUtf8(rt, value.asString(rt).utf8(rt).c_str()));
        } else if (value.isNull()) {
            return jsi::Value::null();
        } else if (value.isObject()) {
            const jsi::Object obj = value.asObject(rt);
            if (obj.isArray(rt)) {
                return jsi::Value(obj.asArray(rt));
            } else if (obj.isFunction(rt)) {
                return jsi::Value(obj.asFunction(rt));
            }
            
            return jsi::Value(value.asObject(rt));
        }
        
        return jsi::Value::undefined();
    }

    void CustomFunction::jsToSqliteValue(const jsi::Value& value, jsi::Runtime& rt, sqlite3_context *invocation) {
        CustomFunction* self = (CustomFunction*) sqlite3_user_data(invocation);
        if (value.isNumber()) {
            return sqlite3_result_double(
                                         invocation,
                                         value.asNumber()
                                         );
        } else if (value.isBigInt()) {
            return sqlite3_result_int64(invocation, value.asBigInt(rt).asInt64(rt));
        } else if (value.isString()) {
            const string utf8 = value.asString(rt).utf8(rt);
            return sqlite3_result_text(
                                       invocation,
                                       utf8.c_str(),
                                       utf8.length(),
                                       SQLITE_TRANSIENT
                                       );
        } else if (value.isNull() || value.isUndefined()) {
            return sqlite3_result_null(invocation);
        }
        
        throw jsi::JSError(rt, self->GetDataErrorPrefix() + " an invalid value");
    }


    void CustomFunction::PropagateJSError(sqlite3_context* invocation, string error) {
        sqlite3_result_error(invocation, error.c_str(), error.size());
    }

    string CustomFunction::GetDataErrorPrefix() {
        return string("User-defined function ") + name + "() returned";
    }
}
