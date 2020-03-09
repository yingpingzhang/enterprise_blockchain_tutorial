#include "param.h"

// CBC ========================================================

void ParamAesCBC::FromV8(Local<Value> v8Value) {
	try {
		Nan::HandleScope();

		if (!v8Value->IsObject()) {
			THROW_ERROR("Parameter 1 MUST be Object", NULL);
		}

		Local<Object> v8Params = v8Value->ToObject();

		// Check data
		if (!check_param_buffer(v8Params, STR_IV))
			THROW_ERROR("Attribute 'iv' MUST be BUFFER", NULL);
		if (!(check_param_empty(v8Params, STR_DATA) || check_param_buffer(v8Params, STR_DATA)))
			THROW_ERROR("Attribute 'data' MUST be NULL | Buffer", NULL);
		Free();
		New();

		// Iv
		Local<Object> v8Iv = v8Params->Get(Nan::New(STR_IV).ToLocalChecked())->ToObject();
		memcpy(data.iv, node::Buffer::Data(v8Iv), node::Buffer::Length(v8Iv));

		// Data?
		if (!v8Params->Get(Nan::New(STR_DATA).ToLocalChecked())->IsUndefined()) {
			GET_BUFFER_SMPL(aesData, v8Params->Get(Nan::New(STR_DATA).ToLocalChecked())->ToObject());
			data.pData = (CK_BYTE_PTR)malloc(aesDataLen* sizeof(CK_BYTE));
			memcpy(data.pData, aesData, aesDataLen);
			data.length = (CK_ULONG)aesDataLen;
		}
	}
	CATCH_ERROR;
}

CK_AES_CBC_ENCRYPT_DATA_PARAMS_PTR ParamAesCBC::New() {
	data = CK_AES_CBC_ENCRYPT_DATA_PARAMS();
	data.pData = NULL;
	data.length = 0;

	return Get();
}

void ParamAesCBC::Free() {
	if (Get()) {
		if (data.pData)
			free(data.pData);
	}
}

// CCM ========================================================

void ParamAesCCM::FromV8(Local<Value> v8Value) {
	try {
		Nan::HandleScope();

		if (!v8Value->IsObject()) {
			THROW_ERROR("Parameter 1 MUST be Object", NULL);
		}

		Local<Object> v8Params = v8Value->ToObject();

		// Check data
		if (!check_param_number(v8Params, STR_DATA_LEN))
			THROW_ERROR("Attribute 'dataLen' MUST be NUMBER", NULL);
		if (!(check_param_empty(v8Params, STR_NONCE) || check_param_buffer(v8Params, STR_NONCE)))
			THROW_ERROR("Attribute 'nonce' MUST be NULL || BUFFER", NULL);
		if (!(check_param_empty(v8Params, STR_AAD) || check_param_buffer(v8Params, STR_AAD)))
			THROW_ERROR("Attribute 'aad' MUST be NULL || BUFFER", NULL);
		if (!check_param_number(v8Params, STR_MAC_LEN))
			THROW_ERROR("Attribute 'macLen' MUST be NUMBER", NULL);

		Free();
		New();

		data.ulDataLen = Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_DATA_LEN).ToLocalChecked())).ToLocalChecked()->Uint32Value();
		data.ulMACLen = Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_MAC_LEN).ToLocalChecked())).ToLocalChecked()->Uint32Value();

		if (!check_param_empty(v8Params,STR_NONCE)) {
			GET_BUFFER_SMPL(nonce, v8Params->Get(Nan::New(STR_NONCE).ToLocalChecked())->ToObject());
			data.pNonce = (CK_BYTE_PTR)malloc(nonceLen * sizeof(CK_BYTE));
			memcpy(data.pNonce, nonce, nonceLen);
			data.ulNonceLen = (CK_ULONG)nonceLen;
		}

		if (!check_param_empty(v8Params, STR_AAD)) {
			GET_BUFFER_SMPL(aad, v8Params->Get(Nan::New(STR_AAD).ToLocalChecked())->ToObject());
			data.pAAD = (CK_BYTE_PTR)malloc(aadLen * sizeof(CK_BYTE));
			memcpy(data.pAAD, aad, aadLen);
			data.ulAADLen = (CK_ULONG)aadLen;
		}
	}
	CATCH_ERROR;
}

CK_AES_CCM_PARAMS_PTR ParamAesCCM::New() {
	data = CK_AES_CCM_PARAMS();
	data.ulDataLen = 0;
	data.pNonce = NULL;
	data.ulNonceLen = 0;
	data.pAAD = NULL;
	data.ulAADLen = 0;
	data.ulMACLen = 0;

	return Get();
}

void ParamAesCCM::Free() {
	if (Get()) {
		if (data.pNonce)
			free(data.pNonce);
		if (data.pAAD)
			free(data.pAAD);
	}
}

// GCM ========================================================

void ParamAesGCM::FromV8(Local<Value> v8Value) {
	try {
		Nan::HandleScope();

		if (!v8Value->IsObject()) {
			THROW_ERROR("Parameter 1 MUST be Object", NULL);
		}

		Local<Object> v8Params = v8Value->ToObject();

		// Check data
		if (!check_param_number(v8Params, STR_TAG_BITS))
			THROW_ERROR("Attribute 'tagBits' MUST be NUMBER", NULL);
		if (!check_param_number(v8Params, STR_IV_BITS))
			THROW_ERROR("Attribute 'ivBits' MUST be NUMBER", NULL);
		if (!(check_param_empty(v8Params, STR_IV) || check_param_buffer(v8Params, STR_IV)))
			THROW_ERROR("Attribute 'iv' MUST be NULL || BUFFER", NULL);
		if (!(check_param_empty(v8Params, STR_AAD) || check_param_buffer(v8Params, STR_AAD)))
			THROW_ERROR("Attribute 'aad' MUST be NULL || BUFFER", NULL);

		Free();
		New();

		data.ulIvBits = Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_IV_BITS).ToLocalChecked())).ToLocalChecked()->Uint32Value();
		data.ulTagBits = Nan::To<v8::Number>(v8Params->Get(Nan::New(STR_TAG_BITS).ToLocalChecked())).ToLocalChecked()->Uint32Value();

		if (!check_param_empty(v8Params, STR_IV)) {
			GET_BUFFER_SMPL(buffer, v8Params->Get(Nan::New(STR_IV).ToLocalChecked())->ToObject());
			data.pIv = (CK_BYTE_PTR)malloc(bufferLen * sizeof(CK_BYTE));
			memcpy(data.pIv, buffer, bufferLen);
			data.ulIvLen = (CK_ULONG)bufferLen;
		}

		if (!check_param_empty(v8Params, STR_AAD)) {
			GET_BUFFER_SMPL(buffer, v8Params->Get(Nan::New(STR_AAD).ToLocalChecked())->ToObject());
			data.pAAD = (CK_BYTE_PTR)malloc(bufferLen * sizeof(CK_BYTE));
			memcpy(data.pAAD, buffer, bufferLen);
			data.ulAADLen = (CK_ULONG)bufferLen;
		}
	}
	CATCH_ERROR;
}

CK_AES_GCM_PARAMS_PTR ParamAesGCM::New() {
	data = CK_AES_GCM_PARAMS();
	data.pAAD = NULL;
	data.ulAADLen = 0;
	data.pIv = NULL;
	data.ulIvLen = 0;
	data.ulIvBits = 0;
	data.ulTagBits = 0;

	return Get();
}

void ParamAesGCM::Free() {
	if (Get()) {
		if (data.pIv)
			free(data.pIv);
		if (data.pAAD)
			free(data.pAAD);
	}
}