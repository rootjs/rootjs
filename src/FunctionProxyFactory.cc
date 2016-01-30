#include "FunctionProxyFactory.h"

#include "FunctionProxy.h"
#include <v8.h>
#include <TGlobal.h>
#include <TClass.h>
#include <TClassRef.h>

namespace rootJS
{

	FunctionProxyFactory::FunctionProxyFactory()
	{
		//TODO
	}

	FunctionProxy* FunctionProxyFactory::createFunctionProxy(TFunction *function, TClassRef const& scope)
	{
		return nullptr; //TODO
	}

	FunctionProxy* FunctionProxyFactory::fromArgs(std::string &name, TClassRef const& scope, const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		return nullptr; //TODO
	}

	void* FunctionProxyFactory::createInstance(TClassRef &type, v8::Local<v8::Array> args)
	{
		// TODO
		return nullptr;
	}

}
