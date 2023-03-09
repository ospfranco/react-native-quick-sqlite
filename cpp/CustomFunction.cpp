#include<string>
#include<sqlite3.h>
#include "JSIHelper.h"
#include<CustomFunction.h>

using namespace std;
using namespace facebook;
using namespace jsi;

CustomFunction::CustomFunction(
    Runtime& rt,
    const string name,
    const Function* fn
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

Value* CustomFunction::getArguments(Runtime& rt, sqlite3_value** argv, int argc, Value* value, const size_t startIndex) {
    if (value == nullptr) {
        value = (Value*) malloc(sizeof(Value) * argc);
    }

    for (int i = startIndex; i < startIndex + argc; i++) {
        int type = sqlite3_value_type(argv[i]);
        switch (type) {
            case SQLITE_BLOB:
            case SQLITE_TEXT:
                value[i] = String::createFromAscii(rt, (char *) sqlite3_value_blob(argv[i]));
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
    
    Value value[argc];
    CustomFunction::getArguments(self->rt, argv, argc, value);
    try {
        const Value mayBeResult = self->fn->call(self->rt, value, argc);
        if (!mayBeResult.isNull() && !mayBeResult.isUndefined()) {
            CustomFunction::jsToSqliteValue(mayBeResult, self->rt, invocation);
            return;
        }

        CustomFunction::PropagateJSError (invocation, "");
    } catch (const exception& e) {
        CustomFunction::PropagateJSError (invocation, e.what());
    }
}

void CustomFunction::jsToSqliteValue(const Value& value, Runtime& rt, sqlite3_context *invocation) {
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
}


void CustomFunction::PropagateJSError(sqlite3_context* invocation, string error) {
    sqlite3_result_error(invocation, error.c_str(), error.size());
}

string CustomFunction::GetDataErrorPrefix() {
    return string("User-defined function ") + name + "() returned";
}
