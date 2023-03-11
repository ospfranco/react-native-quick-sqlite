//
//  CustomAggregate.hpp
//  react-native-quick-sqlite
//
//  Created by Roshan Kumar on 08/03/23.
//

#ifndef CustomAggregate_hpp
#define CustomAggregate_hpp

#include<stdio.h>
#include<string>
#include<sqlite3.h>
#include<JSIHelper.h>
#include<CustomFunction.h>

using namespace std;
using namespace facebook;
using namespace jsi;


struct Accumulator {
    public:
        Value value;
        bool initialized;
        bool is_window;
};

class CustomAggregate : public CustomFunction {
public:
    const bool startIsFunction;
    const bool inverseIsFunction;
    const bool resultIsFunction;
    const shared_ptr<Function> inverse;
    const shared_ptr<Function> result;
    const shared_ptr<Function> start;

    explicit CustomAggregate(
                             Runtime& rt,
                             const string name,
                             const shared_ptr<Function> step,
                             const bool startIsFunction,
                             const bool inverseIsFunction,
                             const bool resultIsFunction,
                             const shared_ptr<Function> start,
                             const shared_ptr<Function> inverse,
                             const shared_ptr<Function> result
    );

    static void xStep(sqlite3_context* invocation, int argc, sqlite3_value** argv);
    static void xInverse(sqlite3_context* invocation, int argc, sqlite3_value** argv);
    static void xValue(sqlite3_context* invocation);
    static void xFinal(sqlite3_context* invocation);

private:

    static inline void xStepBase(sqlite3_context* invocation, int argc, sqlite3_value** argv, const shared_ptr<Function> ptrtm);
    static inline void xValueBase(sqlite3_context* invocation, bool is_final);

    Accumulator* GetAccumulator(sqlite3_context* invocation);
    static void DestroyAccumulator(sqlite3_context* invocation);
    void PropagateJSError(sqlite3_context* invocation);
};



#endif /* CustomAggregate_hpp */
