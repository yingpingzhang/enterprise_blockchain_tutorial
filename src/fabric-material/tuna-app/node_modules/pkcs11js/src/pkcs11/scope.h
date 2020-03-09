#ifndef INCLUDE_H_PKCS11_SCOPE
#define INCLUDE_H_PKCS11_SCOPE

#include <memory>

template <typename T>
using Scoped = std::shared_ptr<T>;

#endif // INCLUDE_H_PKCS11_SCOPE