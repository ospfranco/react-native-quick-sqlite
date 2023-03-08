
#ifndef CustomFunction_hpp
#define CustomFunction_hpp

#include<string>
#include<sqlite3.h>
#include "JSIHelper.h"

using namespace std;
using namespace facebook;
using namespace jsi;

class CustomFunction {
    protected:
        const string name;
        Runtime& rt;
        Function* fnCpy;
        static void PropagateJSError(sqlite3_context* invocation, string error);
        string GetDataErrorPrefix();

    public:
        CustomFunction(
            Runtime& rt,
            const string name,
            const Function& fn
        );
        virtual ~CustomFunction();
        static void xDestroy(void* self);
        static void jsToSqliteValue(const Value& value, Runtime& rt, sqlite3_context* invocation);
        static void xFunc(sqlite3_context* invocation, int argc, sqlite3_value** argv);
        static void getArguments(Runtime& rt, sqlite3_value** argv, int argc, Value* value);
};

#endif /* CustomFunction_hpp */
