//
//  CustomAggregate.cpp
//  react-native-quick-sqlite
//
//  Created by Roshan Kumar on 08/03/23.
//

#include "CustomAggregate.h"
#include<string>
#include<sqlite3.h>
#include<JSIHelper.h>

using namespace std;
using namespace facebook;
using namespace jsi;


CustomAggregate::CustomAggregate(
                                 Runtime& rt,
                                 const string name,
                                 const Function& step,
                                 const Value& start,
                                 const Value& inverse,
                                 const Value& result
    ) :
        CustomFunction(rt, name, step),
        invoke_result(result.isObject() && result.getObject(rt).isFunction(rt)),
        invoke_start(start.isObject() && start.getObject(rt).isFunction(rt)),
        invoke_inverse(inverse.isObject() && inverse.getObject(rt).isFunction(rt))
        {
            // we need to copy this function, reason being that Javascript destroys the argument storage once the function call is finished.
            this->inverse = (Function*) malloc(sizeof(Function));
            const Function& inverseFunction = invoke_inverse ? inverse.getObject(rt).getFunction(rt) : Function::createFromHostFunction(rt, PropNameID::forAscii(rt, "inverse"), 0, [](Runtime& rt, const Value& thisValue, const Value* arguments, size_t count) -> Value { return Value::null(); });
            memcpy(this->inverse, &inverseFunction, sizeof(inverseFunction));
            this->result = (Function*) malloc(sizeof(Function));
            const Function& resultFunction = invoke_result ? result.getObject(rt).getFunction(rt) : Function::createFromHostFunction(rt, PropNameID::forAscii(rt, "result"), 0, [](Runtime& rt, const Value& thisValue, const Value* arguments, size_t count) -> Value { return Value::null(); });
            memcpy(this->result, &resultFunction, sizeof(resultFunction));
            this->start = (Value*) malloc(sizeof(Value));
            memcpy(this->start, &start, sizeof(start));
        }

void CustomAggregate::xStep(sqlite3_context* invocation, int argc, sqlite3_value** argv) {
    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
    CustomAggregate::xStepBase(invocation, argc, argv, self->fnCpy);
}

void CustomAggregate::xInverse(sqlite3_context* invocation, int argc, sqlite3_value** argv) {
    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
    CustomAggregate::xStepBase(invocation, argc, argv, self->inverse);
}

void CustomAggregate::xValue(sqlite3_context* invocation) {
    CustomAggregate::xValueBase(invocation, false);
}

void CustomAggregate::xFinal(sqlite3_context* invocation) {
    CustomAggregate::xValueBase(invocation, true);
}

void CustomAggregate::xStepBase(sqlite3_context* invocation, int argc, sqlite3_value** argv, const Function* ptrtm) {
    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
    Accumulator* acc = self->GetAccumulator(invocation);

    Value value[argc];
    getArguments(self->rt, argv, argc, value);

    Value maybeReturnValue = ptrtm->call(self->rt, value, argc);

    if (maybeReturnValue.isNull() || maybeReturnValue.isUndefined()) {
        self->PropagateJSError(invocation);
    } else {
        acc->value = &maybeReturnValue;
    }
}

inline void CustomAggregate::xValueBase(sqlite3_context* invocation, bool is_final) {
    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
    Accumulator* acc = self->GetAccumulator(invocation);
    if (acc->value->isNull() || acc->value->isUndefined()) return;

    if (!is_final) {
        acc->is_window = true;
    } else if (acc->is_window) {
        DestroyAccumulator(invocation);
        return;
    }

    Value& result = *acc->value;
    if (self->invoke_result) {
        const Value& maybeResult = self->result->call(self->rt, result, 1);
        if (maybeResult.isNull() || maybeResult.isUndefined()) {
            self->PropagateJSError(invocation);
            return;
        }

//        result = maybeResult;
    }

    jsToSqliteValue(result, self->rt, invocation);
    if (is_final) DestroyAccumulator(invocation);
}

Accumulator* CustomAggregate::GetAccumulator(sqlite3_context* invocation) {
    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
    Accumulator* acc = static_cast<Accumulator*>(sqlite3_aggregate_context(invocation, sizeof(Accumulator)));
    if (!acc->initialized) {
        assert(acc->value->isNull() || acc->value->isUndefined());
        acc->initialized = true;
        if (invoke_start) {
            Value maybeSeed = self->start->getObject(rt).getFunction(rt).call(rt, Value::null(), 0);
            if (maybeSeed.isNull() || maybeSeed.isUndefined()) PropagateJSError(invocation);
            else acc->value = &maybeSeed;
        } else {
            assert(!(self->start->isNull() || self->start->isUndefined()));
            acc->value = self->start;
        }
    }
    return acc;
}

void CustomAggregate::DestroyAccumulator(sqlite3_context* invocation) {
    Accumulator* acc = static_cast<Accumulator*>(sqlite3_aggregate_context(invocation, sizeof(Accumulator)));
    assert(acc->initialized);
    Value undefined = Value::undefined();
    acc->value = &undefined;
}

void CustomAggregate::PropagateJSError(sqlite3_context* invocation) {
    DestroyAccumulator(invocation);
    CustomFunction::PropagateJSError(invocation, "");
}

