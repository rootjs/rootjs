#include "Toolbox.h"

#include <iostream>

#include <v8.h>

namespace rootJS
{
	void Toolbox::throwException(const std::string& message)
	{
		v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), message.c_str()));
	}

	void Toolbox::logInfo(const std::string& message)
	{
		if (Toolbox::VERBOSITY > 0)
		{
			// TODO: maybe find a way to directly call console.log("...") here
			std::cout << message << std::endl;
		}	
	}

	void Toolbox::logError(const std::string& message)
	{
		if (Toolbox::VERBOSITY > 1)
		{
			// TODO: maybe find a way to directly call console.error("...") here
			std::cerr << message << std::endl;
		}
	}
}
