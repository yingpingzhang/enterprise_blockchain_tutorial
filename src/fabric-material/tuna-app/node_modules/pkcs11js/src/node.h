#include <uv.h>
#include <node_object_wrap.h>

#include <nan.h>

#include "pkcs11/pkcs11.h"

using namespace v8;
using namespace node;

class WPKCS11 : public node::ObjectWrap {
public:
	static v8::Local<v8::Object> NewInstance() {
		v8::Local<v8::Function> cons = Nan::New(constructor());
		return Nan::NewInstance(cons).ToLocalChecked();
	}
	static v8::Local<v8::Object> NewInstance(int argc, v8::Local<v8::Value> argv[]) {
		v8::Local<v8::Function> cons = Nan::New(constructor());
		return Nan::NewInstance(cons, argc, argv).ToLocalChecked();
	}

	static void Init(v8::Handle<v8::Object> exports);

	static NAN_METHOD(New);
	static NAN_METHOD(Load);

	// PKCS11
	static NAN_METHOD(C_Initialize);
	static NAN_METHOD(C_Finalize);
	static NAN_METHOD(C_GetInfo);
	static NAN_METHOD(C_GetSlotList);
	static NAN_METHOD(C_GetSlotInfo);
	static NAN_METHOD(C_GetTokenInfo);
	static NAN_METHOD(C_GetMechanismList);
	static NAN_METHOD(C_GetMechanismInfo);
	static NAN_METHOD(C_InitToken);
	static NAN_METHOD(C_InitPIN);
	static NAN_METHOD(C_SetPIN);
	static NAN_METHOD(C_OpenSession);
	static NAN_METHOD(C_CloseSession);
	static NAN_METHOD(C_CloseAllSessions);
	static NAN_METHOD(C_GetSessionInfo);
	// static NAN_METHOD(C_GetOperationState);
	// static NAN_METHOD(C_SetOperationState);
	static NAN_METHOD(C_Login);
	static NAN_METHOD(C_Logout);
	
	///* Object management */
	static NAN_METHOD(C_CreateObject);
	static NAN_METHOD(C_CopyObject);
	static NAN_METHOD(C_DestroyObject);
	static NAN_METHOD(C_GetObjectSize);
	static NAN_METHOD(C_FindObjectsInit);
	static NAN_METHOD(C_FindObjects);
	static NAN_METHOD(C_FindObjectsFinal);
	static NAN_METHOD(C_GetAttributeValue);
	static NAN_METHOD(C_SetAttributeValue);

	///* Encryption and decryption */
	static NAN_METHOD(C_EncryptInit);
	static NAN_METHOD(C_Encrypt);
	static NAN_METHOD(C_EncryptUpdate);
	static NAN_METHOD(C_EncryptFinal);
	static NAN_METHOD(C_DecryptInit);
	static NAN_METHOD(C_Decrypt);
	static NAN_METHOD(C_DecryptUpdate);
	static NAN_METHOD(C_DecryptFinal);

	///* Message digesting */
	static NAN_METHOD(C_DigestInit);
	static NAN_METHOD(C_Digest);
	static NAN_METHOD(C_DigestUpdate);
	static NAN_METHOD(C_DigestFinal);
	static NAN_METHOD(C_DigestKey);

	///* Signing and MACing */
	static NAN_METHOD(C_SignInit);
	static NAN_METHOD(C_Sign);
	static NAN_METHOD(C_SignUpdate);
	static NAN_METHOD(C_SignFinal);
	static NAN_METHOD(C_SignRecoverInit);
	static NAN_METHOD(C_SignRecover);

	/* Verifying signatures and MACs */
	static NAN_METHOD(C_VerifyInit);
	static NAN_METHOD(C_Verify);
	static NAN_METHOD(C_VerifyUpdate);
	static NAN_METHOD(C_VerifyFinal);
	static NAN_METHOD(C_VerifyRecoverInit);
	static NAN_METHOD(C_VerifyRecover);

	/* Key management */
	static NAN_METHOD(C_GenerateKey);
	static NAN_METHOD(C_GenerateKeyPair);
	static NAN_METHOD(C_WrapKey);
	static NAN_METHOD(C_UnwrapKey);
	static NAN_METHOD(C_DeriveKey);

	/* Random number generation */
	static NAN_METHOD(C_SeedRandom);
	static NAN_METHOD(C_GenerateRandom);

	Scoped<PKCS11> pkcs11;

protected:

	static inline Nan::Persistent<v8::Function> & constructor() {
		static Nan::Persistent<v8::Function> my_constructor;
		return my_constructor;
	}
};