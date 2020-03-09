#include "error.h"

Scoped<string> Error::ToString() {
	Scoped<string> res(new string(""));
	*res += *message;
	*res += *Stack();
	return res;
}

Scoped<string> Error::Stack() {
	Scoped<string> res(new string(""));

	*res += string("\n");
	*res += string("    at Error (native) ");
	*res += *function;
	*res += string(":");
	*res += to_string(line);
	if (stack.get()) {
		*res += *stack->Stack();
	}
	return res;
}