#ifndef INCLUDE_H_PKCS11_TEMPLATE
#define INCLUDE_H_PKCS11_TEMPLATE

// #include "attr.h"
#include "core.h"
#include "v8_convert.h"

struct TEMPLATE {
	CK_ULONG size;
	CK_ATTRIBUTE_PTR items;
};

class Attributes : public V8Converter<TEMPLATE> {
public:
	Attributes();
	~Attributes();
	void FromV8(Local<Value> obj);
	Local<Object> ToV8();
	TEMPLATE* New();
	void Push(CK_ATTRIBUTE_PTR attr);
};

#endif // INCLUDE_H_PKCS11_TEMPLATE