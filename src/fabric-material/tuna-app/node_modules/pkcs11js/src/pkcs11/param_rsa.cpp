#include "param.h"

void ParamRsaOAEP::FromV8(Local<Value> v8Value) {
	try {
		Nan::HandleScope();

		if (!v8Value->IsObject()) {
			THROW_ERROR("Parameter 1 MUST be Object", NULL);
		}

		Local<Object> v8Params = v8Value->ToObject();

		// Check data
		if (!check_param_number(v8Params, STR_MGF))
			THROW_ERROR("Attribute 'mgf' MUST be NUMBER", NULL);
		if (!check_param_number(v8Params, STR_SOURCE))
			THROW_ERROR("Attribute 'source' MUST be NUMBER", NULL);
		if (!check_param_number(v8Params, STR_HASH_ALG))
			THROW_ERROR("Attribute 'hashAlg' MUST be NUMBER", NULL);
		if (!(check_param_empty(v8Params, STR_SOURCE_DATA) || check_param_buffer(v8Params, STR_SOURCE_DATA)))
			THROW_ERROR("Attribute 'iv' MUST be NULL || BUFFER", NULL);

		Free();
		New();

		data.source = Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_SOURCE).ToLocalChecked())).ToLocalChecked()->Uint32Value();
		data.mgf= Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_MGF).ToLocalChecked())).ToLocalChecked()->Uint32Value();
		data.hashAlg = Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_HASH_ALG).ToLocalChecked())).ToLocalChecked()->Uint32Value();

		if (!check_param_empty(v8Params, STR_SOURCE_DATA)) {
			GET_BUFFER_SMPL(buffer, v8Params->Get(Nan::New(STR_SOURCE_DATA).ToLocalChecked())->ToObject());
			data.pSourceData = (CK_BYTE_PTR)malloc(bufferLen * sizeof(CK_BYTE));
			memcpy(data.pSourceData, buffer, bufferLen);
			data.ulSourceDataLen = (CK_ULONG)bufferLen;
		}
	}
	CATCH_ERROR;
}

CK_RSA_PKCS_OAEP_PARAMS_PTR ParamRsaOAEP::New() {
	data = CK_RSA_PKCS_OAEP_PARAMS();
	data.hashAlg = 0;
	data.source= 0; // CKZ_DATA_SPECIFIED ???
	data.mgf= 0;
	data.pSourceData = NULL;
	data.ulSourceDataLen = 0;

	return Get();
}

void ParamRsaOAEP::Free() {
	if (Get()) {
		if (data.pSourceData)
			free(data.pSourceData);
	}
}

// PSS =================================================================================

void ParamRsaPSS::FromV8(Local<Value> v8Value) {
	try {
		Nan::HandleScope();

		if (!v8Value->IsObject()) {
			THROW_ERROR("Parameter 1 MUST be Object", NULL);
		}

		Local<Object> v8Params = v8Value->ToObject();

		// Check data
		if (!check_param_number(v8Params, STR_MGF))
			THROW_ERROR("Attribute 'mgf' MUST be NUMBER", NULL);
		if (!check_param_number(v8Params, STR_SALT_LEN))
			THROW_ERROR("Attribute 'saltLen' MUST be NUMBER", NULL);
		if (!check_param_number(v8Params, STR_HASH_ALG))
			THROW_ERROR("Attribute 'hashAlg' MUST be NUMBER", NULL);

		Free();
		New();

		data.sLen = Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_SALT_LEN).ToLocalChecked())).ToLocalChecked()->Uint32Value();
		data.mgf = Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_MGF).ToLocalChecked())).ToLocalChecked()->Uint32Value();
		data.hashAlg = Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_HASH_ALG).ToLocalChecked())).ToLocalChecked()->Uint32Value();

	}
	CATCH_ERROR;
}

CK_RSA_PKCS_PSS_PARAMS_PTR ParamRsaPSS::New() {
	data = CK_RSA_PKCS_PSS_PARAMS();
	data.hashAlg = 0;
	data.mgf = 0;
	data.sLen = 0;

	return Get();
}

void ParamRsaPSS::Free() {
}