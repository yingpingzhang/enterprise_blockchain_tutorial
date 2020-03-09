#ifndef INCLUDE_H_PKCS11_MECHANISM
#define INCLUDE_H_PKCS11_MECHANISM

#include "core.h"
#include "v8_convert.h"

class Mechanism : public V8Converter<CK_MECHANISM> {
protected:
	void* param;
public:
	Mechanism();
	~Mechanism();
	void FromV8(Local<Value> obj);
	Local<Object> ToV8();
	CK_MECHANISM_PTR New();
	void Free();
};

#endif // INCLUDE_H_PKCS11_MECHANISM