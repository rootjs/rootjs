#include "FunctionProxyFactory.h"
#include "Toolbox.h"
#include "Types.h"

#include "FunctionProxy.h"
#include <vector>
#include <map>

#include <v8.h>
#include <TROOT.h>
#include <TGlobal.h>
#include <TClass.h>
#include <TClassRef.h>
#include <TMethodArg.h>

namespace rootJS
{

	std::map<std::string, v8BasicTypes> FunctionProxyFactory::basicTypeMap = {
		{"char", v8BasicTypes::STRING},
		{"TStringasdf", v8BasicTypes::STRING},
		{"Int_t", v8BasicTypes::NUMBER},
		{"int", v8BasicTypes::NUMBER},
		{"Double_t", v8BasicTypes::NUMBER},
		{"Bool_t", v8BasicTypes::BOOLEAN}
	};

	FunctionProxyFactory::FunctionProxyFactory()
	{
		//TODO
	}

	FunctionProxy* FunctionProxyFactory::createFunctionProxy(TFunction *function, TClass *scope)
	{
		FunctionInfo mode(function);
		return new FunctionProxy(FunctionProxy::getCallFunc(scope, function), mode, function, scope);
	}

	TFunction* FunctionProxyFactory::determineFunction(std::string const& name, TClass *scope, const v8::Local<v8::Array> args)
	{
		std::vector<TFunction*> options;	// list of overridden / overloaded options
		TFunction *validFunction = nullptr;

		if(scope == nullptr)
		{
			// Global function has been called
			TCollection *globals = gROOT->GetListOfGlobalFunctions(kTRUE);
			TFunction *function;

			TIter next(globals);
			while((function = (TFunction*)next()))
			{
				if(strcmp(function->GetName(), name.c_str()) == 0)
				{
					options.push_back(function);
				}
			}
		}
		else
		{
			options = FunctionProxy::getMethodsFromName(scope, name);
		}

		for(TFunction* function : options)
		{
			if((int)args->Length() > function->GetNargs() || (int)args->Length() < (function->GetNargs() - function->GetNargsOpt()))
			{
				continue;
			}

			TList *funcArgs = function->GetListOfMethodArgs();
			bool argsMatch = true;

			for(int i = 0; i < (int)args->Length(); i++)
			{
				if(!paramMatches(((TMethodArg*)funcArgs->At(i))->GetTypeName(), args->Get(i)))
				{
					argsMatch = false;
					break;
				}
			}

			if(argsMatch)
			{
				validFunction = function;
				break;
			}
		}

		return validFunction;
	}

	FunctionProxy* FunctionProxyFactory::fromArgs(std::string const& name, TClass *scope, const v8::Local<v8::Array> args)
	{
		TFunction* function = determineFunction(name, scope, args);

		if(function == nullptr)
		{
			return nullptr;
		}

		return createFunctionProxy(function, scope);
	}

	bool FunctionProxyFactory::paramMatches(const char *type, v8::Local<v8::Value> arg)
	{
		if (Types::isV8Primitive(arg))
		{
			std::map<std::string, v8BasicTypes>::iterator it = basicTypeMap.find(std::string(type));
			if(it != basicTypeMap.end())
			{
				switch(it->second)
				{
				case v8BasicTypes::STRING:
					return Types::isV8String(arg);
				case v8BasicTypes::NUMBER:
					return Types::isV8Number(arg);
				case v8BasicTypes::BOOLEAN:
					return Types::isV8Boolean(arg);
				case v8BasicTypes::ARRAY:
					//TODO: Check array contents...
					return false;
				case v8BasicTypes::OBJECT:
					//TODO: Check object type
					return false;
				default:
					Toolbox::throwException("Jonas was too lazy to implement this...");
					return false;
				}
			}
		}
		else
		{
			v8::Object *objectArg = static_cast<v8::Object*>(*arg);
			if (objectArg->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT)
			{
				Toolbox::logError("Supplied JavaScript object contains an unexpected number of internal fields.");
				return false;
			}

			ObjectProxy *argProxy = static_cast<ObjectProxy*>(objectArg->GetAlignedPointerFromInternalField(Toolbox::ObjectProxyPtr));
			return strcmp(type, argProxy->getTypeName()) == 0; // TODO: this will not work
		}

		return false;
	}
}
