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

namespace osp {
	CustomAggregate::CustomAggregate(
	     jsi::Runtime& rt,
	     const string name,
	     const std::shared_ptr<jsi::Function> step,
	     const bool startIsFunction,
	     const bool inverseIsFunction,
	     const bool resultIsFunction,
	     const std::shared_ptr<jsi::Function> start,
	     const std::shared_ptr<jsi::Function> inverse,
	     const std::shared_ptr<jsi::Function> result
	    ) :
	        CustomFunction(rt, name, step),
	        startIsFunction(startIsFunction),
	        start(start),
	        inverseIsFunction(inverseIsFunction),
	        inverse(inverse),
	        resultIsFunction(resultIsFunction),
	        result(result)
	        {
	        }

	void CustomAggregate::xStep(sqlite3_context* invocation, int argc, sqlite3_value** argv) {
	    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
	    CustomAggregate::xStepBase(invocation, argc, argv, self->fn);
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

	void CustomAggregate::xStepBase(sqlite3_context* invocation, int argc, sqlite3_value** argv, shared_ptr<jsi::Function> ptrtm) {
	    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
	    Accumulator* acc = self->GetAccumulator(invocation);

	    jsi::Value* value = getArguments(self->rt, argv, argc);
	    jsi::Value convertedValue[argc+1];
	    convertedValue[0] = copyValue(self->rt, acc->value);
	    for (int i = 0; i < argc; i++) convertedValue[i+1] = copyValue(self->rt, value[i]);

	    jsi::Value maybeReturnValue = ptrtm.get()->call(self->rt, convertedValue, argc + 1);

	    if (!maybeReturnValue.isUndefined()) {
	        acc->value = copyValue(self->rt, maybeReturnValue);
	    }
	}

	void CustomAggregate::xValueBase(sqlite3_context* invocation, bool is_final) {
	    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
	    Accumulator* acc = self->GetAccumulator(invocation);

	    if (!is_final) {
	        acc->is_window = true;
	    } else if (acc->is_window) {
	        DestroyAccumulator(invocation);
	        return;
	    }

	    jsi::Value result = copyValue(self->rt, acc->value);
	    if (self->resultIsFunction) {
	        result = self->result.get()->call(self->rt, result);

	        if (result.isUndefined()) {
	            result = jsi::Value::null();
	        }
	    }

	    jsToSqliteValue(result, self->rt, invocation);

	    if (is_final) DestroyAccumulator(invocation);
	}

	Accumulator* CustomAggregate::GetAccumulator(sqlite3_context* invocation) {
	    CustomAggregate* self = (CustomAggregate*) sqlite3_user_data(invocation);
	    Accumulator* acc = static_cast<Accumulator*>(sqlite3_aggregate_context(invocation, sizeof(Accumulator)));

	    if (acc->initialized) return acc;

	    acc->initialized = true;
	    jsi::Value maybeSeed = this->start.get()->call(rt);
	    acc->value = copyValue(self->rt, maybeSeed);
	    return acc;
	}

	void CustomAggregate::DestroyAccumulator(sqlite3_context* invocation) {
	    Accumulator* acc = static_cast<Accumulator*>(sqlite3_aggregate_context(invocation, sizeof(Accumulator)));
	    assert(acc->initialized);
	    acc->value = jsi::Value::undefined();
	}

	void CustomAggregate::PropagateJSError(sqlite3_context* invocation) {
	    DestroyAccumulator(invocation);
	    CustomFunction::PropagateJSError(invocation, "");
	}
}
