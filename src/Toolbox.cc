#include "Toolbox.h"

#include <v8.h>

namespace rootJS
{
	void Toolbox::throwException(const char *message)
	{
		v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), message));
	}
}
