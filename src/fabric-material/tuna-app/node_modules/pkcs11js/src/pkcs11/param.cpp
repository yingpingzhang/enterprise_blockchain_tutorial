#include "param.h"

#define RELEASE_CHECK_PARAM(name)													\
bool check_param_##name(Local<Object> obj, const char* paramName) {			\
	Nan::HandleScope();														\
	Local<Value> v8Value = obj->Get(Nan::New(paramName).ToLocalChecked());	\
	return check_##name(v8Value);											\
}

static bool check_buffer(Local<Value> obj) {
	return node::Buffer::HasInstance(obj);
}
RELEASE_CHECK_PARAM(buffer);

static bool check_number(Local<Value> obj) {
	return obj->IsNumber();
}
RELEASE_CHECK_PARAM(number);

static bool check_empty(Local<Value> obj) {
	return obj->IsUndefined() || obj->IsNull();
}
RELEASE_CHECK_PARAM(empty);