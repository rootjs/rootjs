#include "Toolbox.h"

#include <iostream>

#include <nan.h>

namespace rootJS
{
	void Toolbox::throwException(const std::string& message)
	{
		Nan::ThrowError(Nan::New(message.c_str()).ToLocalChecked());
	}

	void Toolbox::logInfo(const std::string& message, int verbosity)
	{
		if (Toolbox::VERBOSITY >= verbosity) {
			// TODO: maybe find a way to directly call console.log("...") here
			std::cout << message << std::endl;
		}
	}

	void Toolbox::logError(const std::string& message)
	{
		// TODO: maybe find a way to directly call console.error("...") here
		std::cerr << message << std::endl;
	}

	std::string Toolbox::Stringv8toStd(v8::Local<v8::String> vstr)
	{
		return std::string(*v8::String::Utf8Value(vstr->ToString()));
	}

}
