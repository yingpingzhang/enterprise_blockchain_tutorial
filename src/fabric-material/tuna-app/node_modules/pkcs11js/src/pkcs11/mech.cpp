#include "mech.h"
#include "param.h"

#define CREATE_PARAM(class_name)					\
	class_name* p = new class_name();				\
	this->param = p;								\
	p->FromV8(v8Parameter);							\
	data.pParameter = p->Get();						\
	data.ulParameterLen = sizeof(*p->Get())

Mechanism::Mechanism() {
	New();
}

Mechanism::~Mechanism() {
	Free();
}

void Mechanism::FromV8(Local<Value> v8Value) {
	try {
		Nan::HandleScope();

		if (!v8Value->IsObject()) {
			THROW_ERROR("Parameter 1 MUST be Object", NULL);
		}

		Local<Object> v8Object = v8Value->ToObject();

		Local<Value> v8MechType = v8Object->Get(Nan::New(STR_MECHANISM).ToLocalChecked());
		if (!v8MechType->IsNumber()) {
			THROW_ERROR("Attribute 'mechanism' MUST be Number", NULL);
		}

		Local<Value> v8Parameter = v8Object->Get(Nan::New(STR_PARAMETER).ToLocalChecked());
		if (!(v8Parameter->IsUndefined() || v8Parameter->IsNull() || node::Buffer::HasInstance(v8Parameter) || v8Parameter->IsObject())) {
			THROW_ERROR("Attribute 'parameter' MUST be Null | Buffer | Object", NULL);
		}

		Free();
		New();

		data.mechanism = Nan::To<v8::Number>(v8MechType).ToLocalChecked()->Uint32Value();
		if (!(v8Parameter->IsUndefined() || v8Parameter->IsNull())) {
			Local<Object> v8Param = v8Parameter->ToObject();
			if (!node::Buffer::HasInstance(v8Param)) {
				Local<Object> v8Param = v8Parameter->ToObject();
				Local<Value> v8Type = v8Param->Get(Nan::New(STR_TYPE).ToLocalChecked());
				CK_ULONG type = v8Type->IsNumber() ? Nan::To<v8::Number>(v8Type).ToLocalChecked()->Uint32Value() : 0;
					switch (type) {
					case CK_PARAMS_EC_DH: {
						CREATE_PARAM(ParamEcdh1);
						break;
					}
					case CK_PARAMS_AES_GCM: {
						CREATE_PARAM(ParamAesGCM);
						break;
					}
					case CK_PARAMS_AES_CCM: {
						CREATE_PARAM(ParamAesCCM);
						break;
					}
					case CK_PARAMS_RSA_OAEP: {
						CREATE_PARAM(ParamRsaOAEP);
						break;
					}
					case CK_PARAMS_RSA_PSS: {
						CREATE_PARAM(ParamRsaPSS);
						break;
					}
					default:
						THROW_ERROR("Unknown type Mech param in use", NULL);
					}
			}
			else {
				GET_BUFFER_SMPL(buf, v8Parameter->ToObject());
				data.pParameter = (char*)malloc(bufLen);
				memcpy(data.pParameter, buf, bufLen);
				data.ulParameterLen = (CK_ULONG)bufLen;
			}
		}
	}
	CATCH_ERROR;
}

Local<Object> Mechanism::ToV8() {
	try {
		Nan::HandleScope();

		Local<Object> v8Mechanism = Nan::New<Object>();
		// Mechanism
		v8Mechanism->Set(Nan::New(STR_MECHANISM).ToLocalChecked(), Nan::New<Number>(data.mechanism));

		// Parameter
		if (data.pParameter) {
			Local<Object> v8Parameter = node::Buffer::Copy(Isolate::GetCurrent(), (char *)data.pParameter, data.ulParameterLen).ToLocalChecked();
			v8Mechanism->Set(Nan::New(STR_PARAMETER).ToLocalChecked(), v8Parameter);
		}
		else {
			v8Mechanism->Set(Nan::New(STR_PARAMETER).ToLocalChecked(), Nan::Null());
		}

		return v8Mechanism;
	}
	CATCH_ERROR;
}

CK_MECHANISM_PTR Mechanism::New() {
	param = NULL;
	data = CK_MECHANISM();
	data.mechanism = 0;
	data.pParameter = NULL;
	data.ulParameterLen = 0;
	return Get();
}

void Mechanism::Free() {
	if (Get() && data.pParameter && !param) {
		free(data.pParameter);
	}
	if (param) {
		delete(param);
	}
}