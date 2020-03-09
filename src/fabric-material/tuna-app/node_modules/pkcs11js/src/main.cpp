#include <nan.h>
#include <node.h>

#include "const.h"
#include "node.h"

NAN_MODULE_INIT(init) {
	Nan::HandleScope scope;

	WPKCS11::Init(target);

	declare_objects(target);
	declare_attributes(target);
	declare_ket_types(target);
	declare_mechanisms(target);
	declare_flags(target);
	declare_certificates(target);
	declare_mgf(target);
	declare_kdf(target);
	declare_params(target);
}

NODE_MODULE(pkcs11, init)