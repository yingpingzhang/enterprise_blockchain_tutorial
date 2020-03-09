#include "./async.h"

static Local<Object> handle_to_v8(CK_ULONG handle) {
	Nan::HandleScope();

	Local<Object> v8Buffer = Nan::NewBuffer(sizeof(CK_ULONG)).ToLocalChecked();
	char* buf = node::Buffer::Data(v8Buffer);

	memcpy(buf, &handle, sizeof(CK_ULONG));

	return v8Buffer;
}

void AsyncGenerateKey::Execute() {
	try {
		hKey = pkcs11->C_GenerateKey(hSession, mech, tmpl);
	}
	catch (Scoped<Error> e) {
		this->SetErrorMessage(e->ToString()->c_str());
	}
}

void AsyncGenerateKey::HandleOKCallback() {
	Nan::HandleScope scope;

	v8::Local<v8::Value> argv[] = {
		Nan::Null(),
		handle_to_v8(hKey)
	};

	callback->Call(2, argv);
}

void AsyncGenerateKeyPair::Execute() {
	try {
		keyPair = pkcs11->C_GenerateKeyPair(hSession, mech, publicKeyTemplate, privateKeyTemplate);
	}
	catch (Scoped<Error> e) {
		this->SetErrorMessage(e->ToString()->c_str());
	}
}

void AsyncGenerateKeyPair::HandleOKCallback() {
	Nan::HandleScope scope;

	Local<Object> v8KeyPair = Nan::New<Object>();
	v8KeyPair->Set(Nan::New(STR_PRIVATE_KEY).ToLocalChecked(), handle_to_v8(keyPair->privateKey));
	v8KeyPair->Set(Nan::New(STR_PUBLIC_KEY).ToLocalChecked(), handle_to_v8(keyPair->publicKey));

	v8::Local<v8::Value> argv[] = {
		Nan::Null(),
		v8KeyPair
	};

	callback->Call(2, argv);
}

void AsyncCrypto::Execute() {
	try {
		switch (type) {
		case ASYNC_CRYPTO_DIGEST:
			result = pkcs11->C_Digest(hSession, input, output);
			break;
		case ASYNC_CRYPTO_ENCRYPT:
			result = pkcs11->C_Encrypt(hSession, input, output);
			break;
		case ASYNC_CRYPTO_DECRYPT:
			result = pkcs11->C_Decrypt(hSession, input, output);
			break;
		case ASYNC_CRYPTO_SIGN:
			result = pkcs11->C_Sign(hSession, input, output);
			break;
		case ASYNC_CRYPTO_VERIFY:
			pkcs11->C_Verify(hSession, input, output);
			break;
		}
	}
	catch (Scoped<Error> e) {
		this->SetErrorMessage(e->ToString()->c_str());
	}
}

void AsyncCrypto::HandleOKCallback() {
	Nan::HandleScope scope;

	Local<Value> v8Result;

	if (type == ASYNC_CRYPTO_VERIFY) {
		v8Result = Nan::New<Boolean>(true);
	}
	else {
		v8Result = Nan::CopyBuffer(result->c_str(), (uint32_t)result->length()).ToLocalChecked();
	}

	v8::Local<v8::Value> argv[] = {
		Nan::Null(),
		v8Result
	};

	callback->Call(2, argv);
}

void AsyncWrapKey::Execute() {
	try {
		result = pkcs11->C_WrapKey(hSession, mech, hWrappingKey, hKey, wrappedKey);
	}
	catch (Scoped<Error> e) {
		this->SetErrorMessage(e->ToString()->c_str());
	}
}

void AsyncWrapKey::HandleOKCallback() {
	Nan::HandleScope scope;

	Local<Value> v8Result;

	v8Result = Nan::CopyBuffer(result->c_str(), (uint32_t)result->length()).ToLocalChecked();

	v8::Local<v8::Value> argv[] = {
		Nan::Null(),
		v8Result
	};

	callback->Call(2, argv);
}

void AsyncUnwrapKey::Execute() {
	try {
		result = pkcs11->C_UnwrapKey(hSession, mech, hUnwrappingKey, wrappedKey, tmpl);
	}
	catch (Scoped<Error> e) {
		this->SetErrorMessage(e->ToString()->c_str());
	}
}

void AsyncUnwrapKey::HandleOKCallback() {
	Nan::HandleScope scope;

	Local<Value> v8Result;

	v8Result = handle_to_v8(result);

	v8::Local<v8::Value> argv[] = {
		Nan::Null(),
		v8Result
	};

	callback->Call(2, argv);
}

void AsyncDeriveKey::Execute() {
	try {
		result = pkcs11->C_DeriveKey(hSession, mech, hBaseKey, tmpl);
	}
	catch (Scoped<Error> e) {
		this->SetErrorMessage(e->ToString()->c_str());
	}
}

void AsyncDeriveKey::HandleOKCallback() {
	Nan::HandleScope scope;

	Local<Value> v8Result;

	v8Result = handle_to_v8(result);

	v8::Local<v8::Value> argv[] = {
		Nan::Null(),
		v8Result
	};

	callback->Call(2, argv);
}