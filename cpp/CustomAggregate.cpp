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
                                 const shared_ptr<Function> step,
                                 const Value* start,
                                 const Value* inverse,
                                 const Value* result
    ) :
        CustomFunction(rt, name, step),
        invoke_result(isFunction(rt, result)),
        invoke_start(isFunction(rt, start)),
        invoke_inverse(isFunction(rt, inverse)),
        start(start)
        {
            // we need to copy this function, reason being that Javascript destroys the argument storage once the function call is finished.
            const function<Value(Runtime&, const Value&, const Value*, size_t)>& nullFunction = [](Runtime& rt, const Value& thisValue, const Value* arguments, size_t count) -> Value { return Value::undefined(); };
            const Function& inverseFunction = invoke_inverse ? getFunction(rt, inverse): Function::createFromHostFunction(rt, PropNameID::forAscii(rt, "inverse"), 0, nullFunction);
            this->inverse = &inverseFunction;
            const Function& resultFunction = invoke_result ? getFunction(rt, result) : Function::createFromHostFunction(rt, PropNameID::forAscii(rt, "result"), 0, nullFunction);
            this->result = &resultFunction;
        }

void CustomAggregate::xStep(sqlite3_context* invocation, int argc, sqlite3_value** argv) {
    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
    CustomAggregate::xStepBase(invocation, argc, argv, self->fn.get());
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

    Value* value = getArguments(self->rt, argv, argc);
    Value convertedValue[argc+1];
    memcpy(&convertedValue[0], acc->value, sizeof(Value));
    for (int i = 0; i < argc; i++) memcpy(&convertedValue[i+1], &value[i], sizeof(Value));
    
    const Value maybeReturnValue = ptrtm->call(self->rt, convertedValue, argc + 1);

    if (isEmpty(self->rt, &maybeReturnValue)) {
        self->PropagateJSError(invocation);
    } else {
        memcpy(acc->value, &maybeReturnValue, sizeof(maybeReturnValue));
    }
}

inline void CustomAggregate::xValueBase(sqlite3_context* invocation, bool is_final) {
    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
    Accumulator* acc = self->GetAccumulator(invocation);
    if (acc->value == nullptr) return;

    if (!is_final) {
        acc->is_window = true;
    } else if (acc->is_window) {
        DestroyAccumulator(invocation);
        return;
    }

    if (self->invoke_result) {
        const Value& maybeResult = self->result->call(self->rt, *acc->value, 1);
        if (isEmpty(self->rt,  &maybeResult)) {
            self->PropagateJSError(invocation);
            return;
        }


    } else {
        jsToSqliteValue(*acc->value, self->rt, invocation);
    }


    if (is_final) DestroyAccumulator(invocation);
}

Accumulator* CustomAggregate::GetAccumulator(sqlite3_context* invocation) {
    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
    Accumulator* acc = static_cast<Accumulator*>(sqlite3_aggregate_context(invocation, sizeof(Accumulator)));

    if (acc->initialized) return acc;

    acc->initialized = true;
    if (isFunction(rt, self->start)) {
        const Object& startObject = self->start->getObject(rt);
        const Function& startFunction = startObject.getFunction(rt);
        Value maybeSeed = startFunction.call(rt);
        if (isEmpty(self->rt, &maybeSeed)) PropagateJSError(invocation);
        else acc->value = &maybeSeed;
    } else if (isEmpty(self->rt, self->start)) {
        throw JSError(rt, "[react-native-quick-sqlite][aggregate] start should not be null or undefined");
    }

    acc->value = (Value*) malloc(sizeof(*self->start));
    memcpy(acc->value, self->start, sizeof(*self->start));
    return acc;
}

void CustomAggregate::DestroyAccumulator(sqlite3_context* invocation) {
    Accumulator* acc = static_cast<Accumulator*>(sqlite3_aggregate_context(invocation, sizeof(Accumulator)));
    assert(acc->initialized);
    acc->value = nullptr;
}

void CustomAggregate::PropagateJSError(sqlite3_context* invocation) {
    DestroyAccumulator(invocation);
    CustomFunction::PropagateJSError(invocation, "");
}
