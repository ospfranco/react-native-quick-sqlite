
#ifndef CustomFunction_hpp
#define CustomFunction_hpp

#include<string>
#include<sqlite3.h>
#include "JSIHelper.h"

using namespace std;
using namespace facebook;

namespace osp {
    class CustomFunction {
    protected:
        const string name;
        jsi::Runtime& rt;
        const shared_ptr<jsi::Function> fn;
        static void PropagateJSError(sqlite3_context* invocation, string error);
        string GetDataErrorPrefix();
        
    public:
        CustomFunction(
                       jsi::Runtime& rt,
                       const string name,
                       const shared_ptr<jsi::Function> fn
                       );
        virtual ~CustomFunction();
        static void xDestroy(void* self);
        static void jsToSqliteValue(const jsi::Value& value, jsi::Runtime& rt, sqlite3_context* invocation);
        static void xFunc(sqlite3_context* invocation, int argc, sqlite3_value** argv);
        static jsi::Value* getArguments(jsi::Runtime& rt, sqlite3_value** argv, int argc, jsi::Value* value = nullptr, const size_t startIndex = 0);
        static jsi::Value copyValue (jsi::Runtime& rt, jsi::Value& value);
    };
};
#endif /* CustomFunction_hpp */
