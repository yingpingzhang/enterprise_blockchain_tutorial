#ifndef INCLUDE_H_PKCS11_ERROR
#define INCLUDE_H_PKCS11_ERROR

#include <string>

#include "scope.h"

using namespace std;

class Error {
protected:
	Scoped<Error> stack;
	Scoped<string> function;
	int line;
	Scoped<string> Stack();

public:
	Scoped<string> message;
	Error(const char* msg, Scoped<Error> stack, const char* function, int line) :
		stack(stack),
		function(new string(function)),
		line(line),
		message(Scoped<string>(new string(msg)))
	{};
	Error(Scoped<string> msg, Scoped<Error> stack, const char* function, int line) :
		stack(stack),
		function(new string(function)),
		line(line),
		message(msg)
	{};
	~Error() {};
	Scoped<string> ToString();
};

#define THROW_ERROR(msg, stack)					\
     throw Scoped<Error>(new Error(msg, stack, __FUNCTION__, __LINE__))

#define CATCH_ERROR										\
	catch (Scoped<Error> e) { throw e; }					\
	catch (...) { THROW_ERROR("Unknown error", NULL); }

#endif // INCLUDE_H_PKCS11_ERROR