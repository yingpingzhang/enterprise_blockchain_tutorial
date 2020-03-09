#include <node.h>
#include <node_buffer.h>

#ifdef WIN32
#include "dl.h"
#else
#include <dlfcn.h>
#endif // WIN32

#include "node.h"
#include "async.h"

#define UNWRAP_PKCS11 auto __pkcs11= WPKCS11::Unwrap<WPKCS11>(info.This())->pkcs11

#define SET_PKCS11_METHOD(name) SetPrototypeMethod(tpl, #name, name)

#define THROW_V8_ERROR(text)	\
	{Nan::ThrowError(text);		\
	return;}

#define THROW_V8_TYPE_ERROR(text)	\
	{Nan::ThrowTypeError(text);		\
	return;}

#define THROW_REQUIRED(argsIndex)	\
	{Local<String> v8ErrorMessage = String::Concat(Nan::New("Parameter ").ToLocalChecked(), Nan::New<Number>(argsIndex+1)->ToString()); \
	v8ErrorMessage = String::Concat(v8ErrorMessage, Nan::New(" is REQUIRED").ToLocalChecked()); \
	THROW_V8_TYPE_ERROR(v8ErrorMessage)}

#define CHECK_REQUIRED(argsIndex)								\
	if (info[argsIndex]->IsUndefined() /*|| info[argsIndex]->IsNull()*/)	\
		THROW_REQUIRED(argsIndex)

#define THROW_WRONG_TYPE(argsIndex, v8Type)	\
	{Local<String> v8ErrorMessage = String::Concat(Nan::New("Parameter ").ToLocalChecked(), Nan::New<Number>(argsIndex+1)->ToString()); \
	v8ErrorMessage = String::Concat(v8ErrorMessage, Nan::New(" MUST be ").ToLocalChecked()); \
	v8ErrorMessage = String::Concat(v8ErrorMessage, Nan::New(#v8Type).ToLocalChecked()); \
	THROW_V8_TYPE_ERROR(v8ErrorMessage)}

#define CHECK_BUFFER(argsIndex)											\
	if (!node::Buffer::HasInstance(info[argsIndex]))						\
		THROW_WRONG_TYPE(argsIndex, Buffer)

#define CATCH_V8_ERROR catch (Scoped<Error> e) {Nan::ThrowError(e->ToString()->c_str());return;}

#define CHECK_TYPE(argsIndex, v8Type)										\
	if (!info[argsIndex]->Is##v8Type())										\
		THROW_WRONG_TYPE(argsIndex, v8Type)

#define GET_MECHANISM(name, argsIndex)										\
	CHECK_REQUIRED(argsIndex)												\
	Scoped<Mechanism> name(new Mechanism);									\
	name->FromV8(info[argsIndex]);

#define GET_TEMPLATE(name, argsIndex)										\
	Scoped<Attributes> name(new Attributes);								\
	name->FromV8(info[argsIndex]);

#define GET_TEMPLATE_R(name, argsIndex)										\
	CHECK_REQUIRED(argsIndex)												\
	GET_TEMPLATE(name, argsIndex)

#define GET_BUFFER(name, argsIndex)											\
	CHECK_REQUIRED(argsIndex);												\
	CHECK_BUFFER(argsIndex);												\
	Scoped<string> name = buffer_to_string(info[argsIndex])

static Scoped<string> buffer_to_string(Local<Value> v8Value) {
	Nan::HandleScope();
	Local<Object> v8Buffer = v8Value->ToObject();
	auto buf = node::Buffer::Data(v8Buffer);
	auto bufLen = node::Buffer::Length(v8Buffer);
	return Scoped<string>(new string(buf, bufLen));
}

static Local<Object> handle_to_v8(CK_ULONG handle) {
	Nan::HandleScope();

	Local<Object> v8Buffer = Nan::NewBuffer(sizeof(CK_ULONG)).ToLocalChecked();
	char* buf = node::Buffer::Data(v8Buffer);

	memcpy(buf, &handle, sizeof(CK_ULONG));

	return v8Buffer;
}

static CK_ULONG v8_to_handle(Local<Value> v8Value) {
	Nan::HandleScope();

	// Check buffer size
	if (node::Buffer::Length(v8Value) < sizeof(CK_ULONG)) {
		THROW_ERROR("Buffer size is less than CK_ULONG size", NULL);
	}

	CK_ULONG handle;

	char* buf = node::Buffer::Data(v8Value);
	handle = *reinterpret_cast<CK_ULONG *>(buf);

	return handle;
}

#define GET_HANDLE(type, name, argsIndex)									\
	CHECK_BUFFER(argsIndex);												\
	type name = static_cast<type>(v8_to_handle(info[argsIndex]))

#define GET_SESSION_HANDLE(name, argsIndex)										\
	GET_HANDLE(CK_SESSION_HANDLE, name, argsIndex)

#define GET_OBJECT_HANDLE(name, argsIndex)										\
	GET_HANDLE(CK_OBJECT_HANDLE, name, argsIndex)

#define GET_SLOT_ID_HANDLE(name, argsIndex)										\
	GET_HANDLE(CK_SLOT_ID, name, argsIndex)

static Scoped<string> get_string(Local<Value> v8String, const char* defaultValue = "") {
	Scoped<string> res;
	if (v8String->IsString()) {
		res = Scoped<string>(new string(*String::Utf8Value(v8String)));
	}
	else
		res = Scoped<string>(new string(defaultValue));

	return res;
}

#define GET_STRING(name, argsIndex, defaultValue)								\
	Scoped<string> name = get_string(info[argsIndex], defaultValue);

static Handle<Object> GetVersion(CK_VERSION& version) {
	Nan::HandleScope();

	Local<Object> v8Version = Nan::New<Object>();
	v8Version->Set(Nan::New(STR_MAJOR).ToLocalChecked(), Nan::New(version.major));
	v8Version->Set(Nan::New(STR_MINOR).ToLocalChecked(), Nan::New(version.minor));

	return  v8Version;
}

#define CN_PKCS11 "PKCS11"

void WPKCS11::Init(Handle<Object> exports) {
	Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
	tpl->SetClassName(Nan::New(CN_PKCS11).ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// methods
	SetPrototypeMethod(tpl, "load", Load);
	SET_PKCS11_METHOD(C_Initialize);
	SET_PKCS11_METHOD(C_Finalize);
	SET_PKCS11_METHOD(C_GetInfo);
	SET_PKCS11_METHOD(C_GetSlotList);
	SET_PKCS11_METHOD(C_GetSlotInfo);
	SET_PKCS11_METHOD(C_GetTokenInfo);
	SET_PKCS11_METHOD(C_GetMechanismList);
	SET_PKCS11_METHOD(C_GetMechanismInfo);
	SET_PKCS11_METHOD(C_InitToken);
	SET_PKCS11_METHOD(C_InitPIN);
	SET_PKCS11_METHOD(C_SetPIN);
	SET_PKCS11_METHOD(C_OpenSession);
	SET_PKCS11_METHOD(C_CloseSession);
	SET_PKCS11_METHOD(C_CloseAllSessions);
	SET_PKCS11_METHOD(C_GetSessionInfo);
	SET_PKCS11_METHOD(C_Login);
	SET_PKCS11_METHOD(C_Logout);
	SET_PKCS11_METHOD(C_CreateObject);
	SET_PKCS11_METHOD(C_CopyObject);
	SET_PKCS11_METHOD(C_DestroyObject);
	SET_PKCS11_METHOD(C_GetObjectSize);
	SET_PKCS11_METHOD(C_FindObjectsInit);
	SET_PKCS11_METHOD(C_FindObjects);
	SET_PKCS11_METHOD(C_FindObjectsFinal);
	SET_PKCS11_METHOD(C_GetAttributeValue);
	SET_PKCS11_METHOD(C_SetAttributeValue);
	SET_PKCS11_METHOD(C_EncryptInit);
	SET_PKCS11_METHOD(C_Encrypt);
	SET_PKCS11_METHOD(C_EncryptUpdate);
	SET_PKCS11_METHOD(C_EncryptFinal);
	SET_PKCS11_METHOD(C_DecryptInit);
	SET_PKCS11_METHOD(C_Decrypt);
	SET_PKCS11_METHOD(C_DecryptUpdate);
	SET_PKCS11_METHOD(C_DecryptFinal);
	SET_PKCS11_METHOD(C_DigestInit);
	SET_PKCS11_METHOD(C_Digest);
	SET_PKCS11_METHOD(C_DigestUpdate);
	SET_PKCS11_METHOD(C_DigestFinal);
	SET_PKCS11_METHOD(C_DigestKey);
	SET_PKCS11_METHOD(C_SignInit);
	SET_PKCS11_METHOD(C_Sign);
	SET_PKCS11_METHOD(C_SignUpdate);
	SET_PKCS11_METHOD(C_SignFinal);
	SET_PKCS11_METHOD(C_SignRecoverInit);
	SET_PKCS11_METHOD(C_SignRecover);
	SET_PKCS11_METHOD(C_VerifyInit);
	SET_PKCS11_METHOD(C_Verify);
	SET_PKCS11_METHOD(C_VerifyUpdate);
	SET_PKCS11_METHOD(C_VerifyFinal);
	SET_PKCS11_METHOD(C_VerifyRecoverInit);
	SET_PKCS11_METHOD(C_VerifyRecover);
	SET_PKCS11_METHOD(C_GenerateKey);
	SET_PKCS11_METHOD(C_GenerateKeyPair);
	SET_PKCS11_METHOD(C_WrapKey);
	SET_PKCS11_METHOD(C_UnwrapKey);
	SET_PKCS11_METHOD(C_DeriveKey);
	SET_PKCS11_METHOD(C_SeedRandom);
	SET_PKCS11_METHOD(C_GenerateRandom);

	constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());

	// static methods
	// Nan::SetMethod<Local<Object>>(tpl->GeFunction(), "generate", Generate);

	exports->Set(Nan::New(CN_PKCS11).ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(WPKCS11::New) {
	if (info.IsConstructCall()) {

		WPKCS11* obj = new WPKCS11();
		obj->pkcs11 = Scoped<PKCS11>(new PKCS11());
		obj->Wrap(info.This());

		info.GetReturnValue().Set(info.This());
	}
	else {
		Local<Function> cons = Nan::New(constructor());
		info.GetReturnValue().Set(Nan::NewInstance(cons, 0, nullptr).ToLocalChecked());
	}
};

NAN_METHOD(WPKCS11::Load) {

	CHECK_REQUIRED(0);
	CHECK_TYPE(0, String);
	GET_STRING(path, 0, "");

	UNWRAP_PKCS11;

	try {
		__pkcs11->Load(path);
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_Initialize) {
	UNWRAP_PKCS11;

	try {
		__pkcs11->C_Initialize();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_Finalize) {
	UNWRAP_PKCS11;

	try {
		__pkcs11->C_Finalize();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GetInfo) {
	UNWRAP_PKCS11;

	try {
		Scoped<CK_INFO> _info = __pkcs11->C_GetInfo();

		Local<Object> v8Object = Nan::New<Object>();
		if (_info->cryptokiVersion.major == 2) {
			/* Do lots of interesting cryptographic things with the token */
			v8Object->Set(Nan::New(STR_CRYPTOKI_VERSION).ToLocalChecked(), GetVersion(_info->cryptokiVersion));
			v8Object->Set(Nan::New(STR_MANUFACTURER_ID).ToLocalChecked(), Nan::New((char*)_info->manufacturerID, 32).ToLocalChecked());
			v8Object->Set(Nan::New(STR_FLAGS).ToLocalChecked(), Nan::New((uint32_t)_info->flags));

			v8Object->Set(Nan::New(STR_LIBRARY_DESCRIPTION).ToLocalChecked(), Nan::New((char*)_info->libraryDescription, 32).ToLocalChecked());
			v8Object->Set(Nan::New(STR_LIBRARY_VERSION).ToLocalChecked(), GetVersion(_info->libraryVersion));
		}

		info.GetReturnValue().Set(v8Object);
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GetSlotList) {
	try {

		UNWRAP_PKCS11;

		CK_BBOOL tokenPresent = info[0]->IsBoolean() ? true : info[0]->ToBoolean()->Value();

		auto arSlotList = __pkcs11->C_GetSlotList(tokenPresent);

		Local<Array> v8SlotList = Nan::New<Array>();
		for (uint32_t i = 0; i < arSlotList.size(); i++) {
			v8SlotList->Set(i, handle_to_v8(arSlotList[i]));
		}

		info.GetReturnValue().Set(v8SlotList);
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GetSlotInfo) {
	try {
		GET_SLOT_ID_HANDLE(slotID, 0);

		UNWRAP_PKCS11;

		auto _info = __pkcs11->C_GetSlotInfo(slotID);

		Local<Object> v8Object = Nan::New<Object>();

		v8Object->Set(Nan::New(STR_SLOT_DESCRIPTION).ToLocalChecked(), Nan::New((char*)_info->slotDescription, 64).ToLocalChecked());
		v8Object->Set(Nan::New(STR_MANUFACTURER_ID).ToLocalChecked(), Nan::New((char*)_info->manufacturerID, 32).ToLocalChecked());
		v8Object->Set(Nan::New(STR_FLAGS).ToLocalChecked(), Nan::New((uint32_t)_info->flags));
		v8Object->Set(Nan::New(STR_HARDWARE_VERSION).ToLocalChecked(), GetVersion(_info->hardwareVersion));
		v8Object->Set(Nan::New(STR_FIRMWARE_VERSION).ToLocalChecked(), GetVersion(_info->firmwareVersion));

		info.GetReturnValue().Set(v8Object);
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GetTokenInfo) {
	try {
		GET_SLOT_ID_HANDLE(slotID, 0);

		UNWRAP_PKCS11;

		auto _info = __pkcs11->C_GetTokenInfo(slotID);

		Local<Object> v8Object = Nan::New<Object>();
		v8Object->Set(Nan::New(STR_LABEL).ToLocalChecked(), Nan::New((char*)_info->label, 32).ToLocalChecked());
		v8Object->Set(Nan::New(STR_MANUFACTURER_ID).ToLocalChecked(), Nan::New((char*)_info->manufacturerID, 32).ToLocalChecked());
		v8Object->Set(Nan::New(STR_MODEL).ToLocalChecked(), Nan::New((char*)_info->model, 16).ToLocalChecked());
		v8Object->Set(Nan::New(STR_SERIAL_NUMER).ToLocalChecked(), Nan::New((char*)_info->serialNumber, 16).ToLocalChecked());
		v8Object->Set(Nan::New(STR_FLAGS).ToLocalChecked(), Nan::New<Number>(_info->flags));
		v8Object->Set(Nan::New(STR_MAX_SESSION_COUNT).ToLocalChecked(), Nan::New<Number>(_info->ulMaxSessionCount));
		v8Object->Set(Nan::New(STR_SESSION_COUNT).ToLocalChecked(), Nan::New<Number>(_info->ulSessionCount));
		v8Object->Set(Nan::New(STR_MAX_RW_SESSION_COUNT).ToLocalChecked(), Nan::New<Number>(_info->ulMaxRwSessionCount));
		v8Object->Set(Nan::New(STR_RW_SESSION_COUNT).ToLocalChecked(), Nan::New<Number>(_info->ulRwSessionCount));
		v8Object->Set(Nan::New(STR_MAX_PIN_LEN).ToLocalChecked(), Nan::New<Number>(_info->ulMaxPinLen));
		v8Object->Set(Nan::New(STR_MIN_PIN_LEN).ToLocalChecked(), Nan::New<Number>(_info->ulMinPinLen));
		v8Object->Set(Nan::New(STR_HARDWARE_VERSION).ToLocalChecked(), GetVersion(_info->hardwareVersion));
		v8Object->Set(Nan::New(STR_FIRMWARE_VERSION).ToLocalChecked(), GetVersion(_info->firmwareVersion));
		v8Object->Set(Nan::New(STR_UTC_TIME).ToLocalChecked(), Nan::New((char*)_info->utcTime, 16).ToLocalChecked());
		v8Object->Set(Nan::New(STR_TOTAL_PUBLIC_MEMORY).ToLocalChecked(), Nan::New<Number>(_info->ulTotalPublicMemory));
		v8Object->Set(Nan::New(STR_FREE_PUBLIC_MEMORY).ToLocalChecked(), Nan::New<Number>(_info->ulFreePublicMemory));
		v8Object->Set(Nan::New(STR_TOTAL_PRIVATE_MEMORY).ToLocalChecked(), Nan::New<Number>(_info->ulTotalPrivateMemory));
		v8Object->Set(Nan::New(STR_FREE_PRIVATE_MEMORY).ToLocalChecked(), Nan::New<Number>(_info->ulFreePrivateMemory));

		info.GetReturnValue().Set(v8Object);
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GetMechanismList) {
	try {
		GET_SLOT_ID_HANDLE(slotID, 0);

		UNWRAP_PKCS11;

		auto pMechanismList = __pkcs11->C_GetMechanismList(slotID);

		Local<Array> v8Res = Nan::New<Array>();
		for (uint32_t i = 0; i < pMechanismList.size(); i++) {
			v8Res->Set(i, Nan::New<Number>(pMechanismList[i]));
		}
		info.GetReturnValue().Set(v8Res);
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GetMechanismInfo) {
	try {
		GET_SLOT_ID_HANDLE(slotID, 0);

		CHECK_REQUIRED(1);
		CHECK_TYPE(1, Number);
		CK_MECHANISM_TYPE type = Nan::To<v8::Number>(info[1]).ToLocalChecked()->Uint32Value();

		UNWRAP_PKCS11;

		auto pInfo = __pkcs11->C_GetMechanismInfo(slotID, type);

		Local<Object> v8Res = Nan::New<Object>();

		v8Res->Set(Nan::New(STR_MIN_KEY_SIZE).ToLocalChecked(), Nan::New<Number>(pInfo->ulMinKeySize));
		v8Res->Set(Nan::New(STR_MAX_KEY_SIZE).ToLocalChecked(), Nan::New<Number>(pInfo->ulMaxKeySize));
		v8Res->Set(Nan::New(STR_FLAGS).ToLocalChecked(), Nan::New<Number>(pInfo->flags));

		info.GetReturnValue().Set(v8Res);
	}
	CATCH_V8_ERROR;
}

///* C_InitToken initializes a token. */

NAN_METHOD(WPKCS11::C_InitToken) {
	try {
		GET_SLOT_ID_HANDLE(slotID, 0);
		GET_STRING(pin, 1, "");
		GET_STRING(label, 2, "");
		UNWRAP_PKCS11;

		Scoped<string> rlabel = __pkcs11->C_InitToken(slotID, pin, label);

		info.GetReturnValue().Set(Nan::New(rlabel->c_str()).ToLocalChecked());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_InitPIN) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_STRING(pin, 1, "");

		UNWRAP_PKCS11;

		__pkcs11->C_InitPIN(hSession, pin);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_SetPIN) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_STRING(oldPin, 1, "");
		GET_STRING(newPin, 2, "");

		UNWRAP_PKCS11;

		__pkcs11->C_SetPIN(hSession, oldPin, newPin);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_OpenSession) {
	try {
		GET_SLOT_ID_HANDLE(slotID, 0);

		CHECK_REQUIRED(1);
		CHECK_TYPE(1, Number);
		CK_FLAGS flags = Nan::To<v8::Number>(info[1]).ToLocalChecked()->Uint32Value();

		UNWRAP_PKCS11;

		CK_SESSION_HANDLE hSession = __pkcs11->C_OpenSession(slotID, flags);

		info.GetReturnValue().Set(handle_to_v8(hSession));
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_CloseSession) {
	try {
		GET_SESSION_HANDLE(hSession, 0);

		UNWRAP_PKCS11;

		__pkcs11->C_CloseSession(hSession);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_CloseAllSessions) {
	try {
		GET_SLOT_ID_HANDLE(slotID, 0);

		UNWRAP_PKCS11;

		__pkcs11->C_CloseAllSessions(slotID);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GetSessionInfo) {
	try {
		GET_SESSION_HANDLE(hSession, 0);

		UNWRAP_PKCS11;

		Scoped<CK_SESSION_INFO> _info = __pkcs11->C_GetSessionInfo(hSession);

		Local<Object> v8Res = Nan::New<Object>();

		v8Res->Set(Nan::New(STR_SLOT_ID).ToLocalChecked(), handle_to_v8(_info->slotID));
		v8Res->Set(Nan::New(STR_STATE).ToLocalChecked(), Nan::New<Number>(_info->state));
		v8Res->Set(Nan::New(STR_FLAGS).ToLocalChecked(), Nan::New<Number>(_info->flags));
		v8Res->Set(Nan::New(STR_DEVICE_ERROR).ToLocalChecked(), Nan::New<Number>(_info->ulDeviceError));

		info.GetReturnValue().Set(v8Res);
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_Login) {
	try {
		GET_SESSION_HANDLE(hSession, 0);

		CHECK_REQUIRED(1);
		CHECK_TYPE(1, Number);
		CK_USER_TYPE userType = Nan::To<v8::Number>(info[1]).ToLocalChecked()->Uint32Value();

		UNWRAP_PKCS11;

		Scoped<string> pin = get_string(info[2]);

		__pkcs11->C_Login(hSession, userType, pin);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_Logout) {
	try {
		GET_SESSION_HANDLE(hSession, 0);

		UNWRAP_PKCS11;

		__pkcs11->C_Logout(hSession);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_FindObjectsInit) {
	try {
		GET_SESSION_HANDLE(hSession, 0);

		UNWRAP_PKCS11;

		if (!info[1]->IsArray()) {
			__pkcs11->C_FindObjectsInit(hSession);
		}
		else {
			Scoped<Attributes> tmpl(new Attributes);
			tmpl->FromV8(info[1]);

			__pkcs11->C_FindObjectsInit(hSession, tmpl);
		}

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_FindObjects) {
	try {
		GET_SESSION_HANDLE(hSession, 0);

		UNWRAP_PKCS11;

		CK_OBJECT_HANDLE hObject = __pkcs11->C_FindObjects(hSession);

		Local<Value> v8Res = (hObject != 0) ? handle_to_v8(hObject).As<Value>() : Nan::Null().As<Value>();

		info.GetReturnValue().Set(v8Res);
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_FindObjectsFinal) {
	try {
		GET_SESSION_HANDLE(hSession, 0);

		UNWRAP_PKCS11;

		__pkcs11->C_FindObjectsFinal(hSession);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GetAttributeValue) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_OBJECT_HANDLE(hObject, 1);
		GET_TEMPLATE_R(tmpl, 2);

		UNWRAP_PKCS11;

		tmpl = __pkcs11->C_GetAttributeValue(hSession, hObject, tmpl);

		info.GetReturnValue().Set(tmpl->ToV8());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_SetAttributeValue) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_OBJECT_HANDLE(hObject, 1);

		UNWRAP_PKCS11;

		CHECK_REQUIRED(2);
		Scoped<Attributes> tmpl(new Attributes());
		tmpl->FromV8(info[2]);

		__pkcs11->C_SetAttributeValue(hSession, hObject, tmpl);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_CreateObject) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_TEMPLATE_R(tmpl, 1);

		UNWRAP_PKCS11;

		CK_OBJECT_HANDLE hObject = __pkcs11->C_CreateObject(hSession, tmpl);

		info.GetReturnValue().Set(handle_to_v8(hObject));
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_CopyObject) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_OBJECT_HANDLE(hObject, 1);
		GET_TEMPLATE_R(tmpl, 2);

		UNWRAP_PKCS11;

		CK_OBJECT_HANDLE hNewObject = __pkcs11->C_CopyObject(hSession, hObject, tmpl);

		info.GetReturnValue().Set(handle_to_v8(hNewObject));
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_DestroyObject) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_OBJECT_HANDLE(hObject, 1);

		UNWRAP_PKCS11;

		__pkcs11->C_DestroyObject(hSession, hObject);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GetObjectSize) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_OBJECT_HANDLE(hObject, 1);

		UNWRAP_PKCS11;

		CK_ULONG ulSize = __pkcs11->C_GetObjectSize(hSession, hObject);

		info.GetReturnValue().Set(Nan::New<Number>(ulSize));
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_EncryptInit) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_OBJECT_HANDLE(hObject, 2);

		UNWRAP_PKCS11;

		__pkcs11->C_EncryptInit(hSession, mech, hObject);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_Encrypt) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(input, 1);
		GET_BUFFER(output, 2);

		UNWRAP_PKCS11;

		if (!info[3]->IsFunction()) {
			Scoped<string> res = __pkcs11->C_Encrypt(hSession, input, output);

			info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[3].As<Function>());

			Nan::AsyncQueueWorker(new AsyncCrypto(callback, __pkcs11, ASYNC_CRYPTO_ENCRYPT, hSession, input, output));
		}
	}
	CATCH_V8_ERROR
}

NAN_METHOD(WPKCS11::C_EncryptUpdate) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(input, 1);
		GET_BUFFER(output, 2);

		UNWRAP_PKCS11;

		Scoped<string> res = __pkcs11->C_EncryptUpdate(hSession, input, output);

		info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_EncryptFinal) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(output, 1);

		UNWRAP_PKCS11;

		Scoped<string> res = __pkcs11->C_EncryptFinal(hSession, output);

		info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_DecryptInit) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_OBJECT_HANDLE(hObject, 2);

		UNWRAP_PKCS11;

		__pkcs11->C_DecryptInit(hSession, mech, hObject);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_Decrypt) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(input, 1);
		GET_BUFFER(output, 2);

		UNWRAP_PKCS11;

		if (!info[3]->IsFunction()) {
			Scoped<string> res = __pkcs11->C_Decrypt(hSession, input, output);

			info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[3].As<Function>());

			Nan::AsyncQueueWorker(new AsyncCrypto(callback, __pkcs11, ASYNC_CRYPTO_DECRYPT, hSession, input, output));
		}
	}
	CATCH_V8_ERROR
}

NAN_METHOD(WPKCS11::C_DecryptUpdate) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(input, 1);
		GET_BUFFER(output, 2);

		UNWRAP_PKCS11;

		Scoped<string> res = __pkcs11->C_DecryptUpdate(hSession, input, output);

		info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_DecryptFinal) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(output, 1);

		UNWRAP_PKCS11;

		Scoped<string> res = __pkcs11->C_DecryptFinal(hSession, output);

		info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_DigestInit) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);

		UNWRAP_PKCS11;

		__pkcs11->C_DigestInit(hSession, mech);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_Digest) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(input, 1);
		GET_BUFFER(output, 2);

		UNWRAP_PKCS11;

		if (!info[3]->IsFunction()) {
			Scoped<string> res = __pkcs11->C_Digest(hSession, input, output);

			info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[3].As<Function>());

			Nan::AsyncQueueWorker(new AsyncCrypto(callback, __pkcs11, ASYNC_CRYPTO_DIGEST, hSession, input, output));
		}
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_DigestUpdate) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(input, 1);

		UNWRAP_PKCS11;

		__pkcs11->C_DigestUpdate(hSession, input);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_DigestFinal) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(output, 1);

		UNWRAP_PKCS11;

		Scoped<string> res = __pkcs11->C_DigestFinal(hSession, output);

		info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_DigestKey) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_OBJECT_HANDLE(hObject, 1);

		UNWRAP_PKCS11;

		__pkcs11->C_DigestKey(hSession, hObject);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_SignInit) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_OBJECT_HANDLE(hObject, 2);

		UNWRAP_PKCS11;

		__pkcs11->C_SignInit(hSession, mech, hObject);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_Sign) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(input, 1);
		GET_BUFFER(output, 2);

		UNWRAP_PKCS11;

		if (!info[3]->IsFunction()) {
			Scoped<string> res = __pkcs11->C_Sign(hSession, input, output);

			info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[3].As<Function>());

			Nan::AsyncQueueWorker(new AsyncCrypto(callback, __pkcs11, ASYNC_CRYPTO_SIGN, hSession, input, output));
		}
	}
	CATCH_V8_ERROR
}

NAN_METHOD(WPKCS11::C_SignUpdate) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(input, 1);

		UNWRAP_PKCS11;

		__pkcs11->C_SignUpdate(hSession, input);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_SignFinal) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(output, 1);

		UNWRAP_PKCS11;

		Scoped<string> res = __pkcs11->C_SignFinal(hSession, output);

		info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_SignRecoverInit) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_OBJECT_HANDLE(hObject, 2);

		UNWRAP_PKCS11;

		__pkcs11->C_SignRecoverInit(hSession, mech, hObject);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_SignRecover) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(data, 1);
		GET_BUFFER(signature, 2);

		UNWRAP_PKCS11;

		Scoped<string> res = __pkcs11->C_SignRecover(hSession, data, signature);

		info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_VerifyInit) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_OBJECT_HANDLE(hObject, 2);

		UNWRAP_PKCS11;

		__pkcs11->C_VerifyInit(hSession, mech, hObject);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_Verify) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(input, 1);
		GET_BUFFER(signature, 2);

		UNWRAP_PKCS11;

		if (!info[3]->IsFunction()) {
			__pkcs11->C_Verify(hSession, input, signature);

			info.GetReturnValue().Set(Nan::New<Boolean>(true));
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[3].As<Function>());

			Nan::AsyncQueueWorker(new AsyncCrypto(callback, __pkcs11, ASYNC_CRYPTO_VERIFY, hSession, input, signature));
		}
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_VerifyUpdate) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(data, 1);

		UNWRAP_PKCS11;

		__pkcs11->C_VerifyUpdate(hSession, data);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_VerifyFinal) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(signature, 1);

		UNWRAP_PKCS11;

		__pkcs11->C_VerifyFinal(hSession, signature);

		info.GetReturnValue().Set(Nan::New<v8::Boolean>(true));
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_VerifyRecoverInit) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_OBJECT_HANDLE(hObject, 2);

		UNWRAP_PKCS11;

		__pkcs11->C_VerifyRecoverInit(hSession, mech, hObject);

		info.GetReturnValue().SetNull();
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_VerifyRecover) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_BUFFER(signature, 1);
		GET_BUFFER(data, 2);

		UNWRAP_PKCS11;

		Scoped<string> res = __pkcs11->C_VerifyRecover(hSession, signature, data);

		info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GenerateKey) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_TEMPLATE_R(tmpl, 2);

		UNWRAP_PKCS11;

		if (!info[3]->IsFunction()) {

			CK_OBJECT_HANDLE hObject = __pkcs11->C_GenerateKey(hSession, mech, tmpl);
			info.GetReturnValue().Set(handle_to_v8(hObject));
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[3].As<Function>());

			Nan::AsyncQueueWorker(new AsyncGenerateKey(callback, __pkcs11, hSession, mech, tmpl));
		}

	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GenerateKeyPair) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_TEMPLATE_R(publicKey, 2);
		GET_TEMPLATE_R(privateKey, 3);

		UNWRAP_PKCS11;

		if (!info[4]->IsFunction()) {
			Scoped<KEY_PAIR> keyPair = __pkcs11->C_GenerateKeyPair(hSession, mech, publicKey, privateKey);

			// Result
			Local<Object> v8Result = Nan::New<Object>();
			v8Result->Set(Nan::New(STR_PRIVATE_KEY).ToLocalChecked(), handle_to_v8(keyPair->privateKey));
			v8Result->Set(Nan::New(STR_PUBLIC_KEY).ToLocalChecked(), handle_to_v8(keyPair->publicKey));

			info.GetReturnValue().Set(v8Result);
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[4].As<Function>());

			Nan::AsyncQueueWorker(new AsyncGenerateKeyPair(callback, __pkcs11, hSession, mech, publicKey, privateKey));
		}
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_WrapKey) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_OBJECT_HANDLE(hWrappingKey, 2);
		GET_OBJECT_HANDLE(hKey, 3);
		GET_BUFFER(wrappedKey, 4);

		int CALLBACK_INDEX = 5;

		UNWRAP_PKCS11;

		if (!info[CALLBACK_INDEX]->IsFunction()) {
			Scoped<string> res = __pkcs11->C_WrapKey(hSession, mech, hWrappingKey, hKey, wrappedKey);

			info.GetReturnValue().Set(Nan::CopyBuffer(res->c_str(), (uint32_t)res->length()).ToLocalChecked());
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[CALLBACK_INDEX].As<Function>());

			Nan::AsyncQueueWorker(new AsyncWrapKey(callback, __pkcs11, hSession, mech, hWrappingKey, hKey, wrappedKey));
		}
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_UnwrapKey) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_OBJECT_HANDLE(hUnwrappingKey, 2);
		GET_BUFFER(wrappedKey, 3);
		GET_TEMPLATE_R(tmpl, 4);

		int CALLBACK_INDEX = 5;

		UNWRAP_PKCS11;

		if (!info[CALLBACK_INDEX]->IsFunction()) {
			CK_OBJECT_HANDLE hKey = __pkcs11->C_UnwrapKey(hSession, mech, hUnwrappingKey, wrappedKey, tmpl);

			info.GetReturnValue().Set(handle_to_v8(hKey));
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[CALLBACK_INDEX].As<Function>());

			Nan::AsyncQueueWorker(new AsyncUnwrapKey(callback, __pkcs11, hSession, mech, hUnwrappingKey, wrappedKey, tmpl));
		}
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_DeriveKey) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		GET_MECHANISM(mech, 1);
		GET_OBJECT_HANDLE(hBaseKey, 2);
		GET_TEMPLATE_R(tmpl, 3);

		int CALLBACK_INDEX = 4;

		UNWRAP_PKCS11;

		if (!info[CALLBACK_INDEX]->IsFunction()) {
			CK_OBJECT_HANDLE hDerivedKey = __pkcs11->C_DeriveKey(hSession, mech, hBaseKey, tmpl);

			info.GetReturnValue().Set(handle_to_v8(hDerivedKey));
		}
		else {
			Nan::Callback *callback = new Nan::Callback(info[CALLBACK_INDEX].As<Function>());

			Nan::AsyncQueueWorker(new AsyncDeriveKey(callback, __pkcs11, hSession, mech, hBaseKey, tmpl));
		}
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_SeedRandom) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		CHECK_BUFFER(1);
		GET_BUFFER_ARGS(seed, 1);

		UNWRAP_PKCS11;

		__pkcs11->C_SeedRandom(hSession, seed, seedLen);

		info.GetReturnValue().Set(info[1]);
	}
	CATCH_V8_ERROR;
}

NAN_METHOD(WPKCS11::C_GenerateRandom) {
	try {
		GET_SESSION_HANDLE(hSession, 0);
		CHECK_BUFFER(1);
		GET_BUFFER_ARGS(buffer, 1);

		UNWRAP_PKCS11;

		__pkcs11->C_GenerateRandom(hSession, buffer, bufferLen);

		info.GetReturnValue().Set(info[1]);
	}
	CATCH_V8_ERROR;
}