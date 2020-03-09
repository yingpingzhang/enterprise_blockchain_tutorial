#ifndef INCLUDE_H_CONST

#define INCLUDE_H_CONST

#include <node.h>
#include <v8.h>
#include <nan.h>

#ifdef WIN32
#pragma pack(push, cryptoki, 1)
#endif
#include <pkcs11/pkcs11.h>
#ifdef WIN32
#pragma pack(pop, cryptoki)
#endif

#include "pkcs11/param.h"

using namespace v8;
using namespace node;

#define SET_CONST(target, value)									\
	Nan::ForceSet(													\
		target,														\
		Nan::New(#value).ToLocalChecked(),							\
		Nan::New<Number>(value),									\
		static_cast<PropertyAttribute>(ReadOnly | DontDelete));

void declare_attributes(Local<Object> target);
void declare_flags(Local<Object> target);
void declare_objects(Local<Object> target);
void declare_ket_types(Local<Object> target);
void declare_mechanisms(Local<Object> target);
void declare_certificates(Local<Object> target);
void declare_mgf(Local<Object> target);
void declare_kdf(Local<Object> target);
void declare_params(Local<Object> target);

#endif // INCLUDE_H_CONST