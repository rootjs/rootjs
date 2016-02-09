#include "FunctionProxyFactory.h"
#include "Toolbox.h"

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
		{"TString", v8BasicTypes::STRING},
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

	TFunction* FunctionProxyFactory::determineFunction(std::string name, TClass *scope, const v8::Local<v8::Array> args) {
		std::vector<TFunction*> validFuncs;
		TFunction *callableFunction = nullptr;
		if(scope == nullptr)
		{	// Global function has been called
			TCollection *globals = gROOT->GetListOfGlobalFunctions(kTRUE);
			TFunction *func;

			TIter next(globals);
			while((func = (TFunction*)next()))
			{
				if(strcmp(func->GetName(), name.c_str()) == 0)
				{
					validFuncs.push_back(func);
				}
			}
		}
		else
		{
			validFuncs = FunctionProxy::getMethodsFromName(scope, name);
		}

		for(TFunction* value: validFuncs)
		{
			if(value->GetNargs() != (int)args->Length())
			{
				continue;
			}
			TList *funcArgs = value->GetListOfMethodArgs();
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
				callableFunction = value;
				break;
			}
		}
		if(callableFunction) {
			return callableFunction;
		}
		return nullptr;
	}

	FunctionProxy* FunctionProxyFactory::fromArgs(std::string name, TClass *scope, v8::Local<v8::Array> args)
	{
		std::vector<TFunction*> validFuncs;
		TFunction *callableFunction = nullptr;
		if(scope == nullptr)
		{	// Global function has been called
			TCollection *globals = gROOT->GetListOfGlobalFunctions(kTRUE);
			TFunction *func;

			TIter next(globals);
			while((func = (TFunction*)next()))
			{
				if(strcmp(func->GetName(), name.c_str()) == 0)
				{
					validFuncs.push_back(func);
				}
			}
		}
		else
		{
			validFuncs = FunctionProxy::getMethodsFromName(scope, name);
		}

		for(TFunction* value: validFuncs)
		{
			if(value->GetNargs() != (int)args->Length())
			{
				continue;
			}
			TList *funcArgs = value->GetListOfMethodArgs();
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
				callableFunction = value;
				break;
			}
		}
		if(callableFunction)
		{
			return createFunctionProxy(callableFunction, scope);
		}

		return nullptr;
	}

	void* FunctionProxyFactory::createInstance(std::string name, TClass *scope, v8::Local<v8::Array> args)
	{
		// TODO
		return nullptr;
	}

	bool FunctionProxyFactory::paramMatches(const char* type, v8::Local<v8::Value> arg)
	{
		if (arg->IsObject())
		{
			v8::Object *objectArg = static_cast<v8::Object*>(*arg);
			if (objectArg->InternalFieldCount() > 0)
			{
				ObjectProxy *argProxy = static_cast<ObjectProxy*>(objectArg->GetAlignedPointerFromInternalField(Toolbox::v8ObjectInternalField::ObjectProxyPtr));
				return strcmp(type, argProxy->getTypeName()) == 0; // TODO: this will not work
			}
			else
			{
				Toolbox::throwException(std::string("v8::Object contains no InternalField entries"));
			}
		}
		else
		{
			std::map<std::string, v8BasicTypes>::iterator it = basicTypeMap.find(std::string(type));
			if(it != basicTypeMap.end())
			{
				switch(it->second)
				{
				case v8BasicTypes::STRING:
					return arg->IsString();
				case v8BasicTypes::NUMBER:
					return arg->IsNumber() || arg->IsNumberObject();
				case v8BasicTypes::BOOLEAN:
					return arg->IsBoolean() || arg->IsBooleanObject();
				case v8BasicTypes::ARRAY:
					//TODO: CHeck array contents...
					return false;
				case v8BasicTypes::OBJECT:
					//TODO: Check object type
					return false;
				default:
					v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Jonas was too lazy to implement this..."));
					return false;
				}
			}
		}

		return false;
	}
}
