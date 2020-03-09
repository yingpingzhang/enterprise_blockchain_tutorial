#ifndef INCLUDE_H_PKCS11_CORE
#define INCLUDE_H_PKCS11_CORE

#define GET_BUFFER_SMPL(varName, v8Object)									\
	char* varName = node::Buffer::Data(v8Object);						\
	auto varName##Len = node::Buffer::Length(v8Object);

#define GET_BUFFER_ARGS(varName, argIndex)								\
	GET_BUFFER_SMPL(varName, info[argIndex]);

#endif // INCLUDE_H_PKCS11_CORE