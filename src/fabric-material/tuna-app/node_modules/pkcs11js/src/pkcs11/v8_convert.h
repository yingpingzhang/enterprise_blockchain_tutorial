#ifndef INCLUDE_H_PKCS11_V8
#define INCLUDE_H_PKCS11_V8

#ifdef WIN32
#pragma pack(push, cryptoki, 1)
#endif
#include <pkcs11/pkcs11.h>
#ifdef WIN32
#pragma pack(pop, cryptoki)
#endif

#include <nan.h>
#include "error.h"
#include "strings.h"

using namespace v8;

template <typename T>
class V8Converter {
protected:
	T data;
public:
	virtual void FromV8(Local<Value> obj) { THROW_ERROR("Not implemented", NULL); }
	virtual Local<Object> ToV8() { THROW_ERROR("Not implemented", NULL); }
	virtual T* New() {
		data = T();
		return this->Get();
	}
	T* Get() {
		return &data;
	}

};

#endif // INCLUDE_H_PKCS11_V8