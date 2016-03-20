#include "FunctionProxyFactory.h"
#include "Toolbox.h"
#include "Types.h"

#include "FunctionProxy.h"
#include <vector>
#include <map>

#include <nan.h>
#include <TROOT.h>
#include <TGlobal.h>
#include <TClass.h>
#include <TClassRef.h>
#include <TClassTable.h>
#include <TMethodArg.h>
#include <limits.h>
#include <float.h>

namespace rootJS
{

	std::map<std::string, v8BasicTypes> FunctionProxyFactory::basicTypeMap = {
		{"char*", v8BasicTypes::STRING},
		{"char", v8BasicTypes::CHAR},

		{"Bool_t", v8BasicTypes::BOOLEAN},
		{"bool", v8BasicTypes::BOOLEAN},

		{"int", v8BasicTypes::NUMBER},
		{"unsigned int", v8BasicTypes::NUMBER},

		{"short", v8BasicTypes::NUMBER},
		{"unsigned short", v8BasicTypes::NUMBER},

		{"unsigned char", v8BasicTypes::NUMBER},

		{"long", v8BasicTypes::NUMBER},
		{"long long", v8BasicTypes::NUMBER},

		{"unsigned long", v8BasicTypes::NUMBER},
		{"unsigned long long", v8BasicTypes::NUMBER},

		{"double", v8BasicTypes::NUMBER},
		{"long double",v8BasicTypes::NUMBER},

		{"float",v8BasicTypes::NUMBER},

		{"Double32_t", v8BasicTypes::NUMBER},
		{"Float16_t", v8BasicTypes::NUMBER},
		{"Long64_t", v8BasicTypes::NUMBER},
		{"ULong64_t", v8BasicTypes::NUMBER}
	};

	std::map<std::string, NumberInfo> FunctionProxyFactory::basicNumberInfoMap = {
		{"int", {false, INT_MAX, INT_MIN}},
		{"unsigned int", {false, UINT_MAX, 0}},

		{"double", {true, DBL_MAX, - DBL_MAX}},
		{"long double", {true, LDBL_MAX, - LDBL_MAX}},

		{"short", {false, SHRT_MAX, SHRT_MIN}},
		{"unsigned short", {false, USHRT_MAX, 0}},

		{"unsigned char", {false, UCHAR_MAX, 0}},

		{"long", {false, LONG_MAX, LONG_MIN}},
		{"long long", {false, LLONG_MAX, LLONG_MIN}},
		{"unsigned long", {false, ULONG_MAX, 0}},
		{"unsigned long long", {false, ULLONG_MAX, 0}},

		{"float", {true, FLT_MAX, - FLT_MAX}},

		{"Double32_t", {true, DBL_MAX, - DBL_MAX}},
		{"Float16_t", {true, FLT_MAX, - FLT_MAX}},
		{"Long64_t", {true, LLONG_MAX, - LLONG_MAX}},
		{"ULong64_t", {true, ULLONG_MAX, 0}}
	};

	FunctionProxyFactory::FunctionProxyFactory()
	{
		//TODO
	}

	FunctionProxy* FunctionProxyFactory::createFunctionProxy(TFunction *function, TClass *scope)
	{
		FunctionInfo info(*function, FunctionProxy::getCallFunc(scope, function), (scope == nullptr));
		return new FunctionProxy(info, function, scope);
	}

	TFunction* FunctionProxyFactory::determineFunction(std::string const& name, TClass *scope, const v8::Local<v8::Array> args)
	{
		std::vector<TFunction*> options;	// list of overridden / overloaded options
		TFunction *validFunction = nullptr;

		if(scope == nullptr) {
			// Global function has been called
			TCollection *globals = gROOT->GetListOfGlobalFunctions(kTRUE);
			TFunction *function;

			TIter next(globals);
			while((function = (TFunction*)next())) {
				if(strcmp(function->GetName(), name.c_str()) == 0) {
					options.push_back(function);
				}
			}
		} else {
			options = FunctionProxy::getMethodsFromName(scope, name);
		}

		for(TFunction* function : options) {
			if((int)args->Length() > function->GetNargs() || (int)args->Length() < (function->GetNargs() - function->GetNargsOpt())) {
				continue;
			}

			TList *funcArgs = function->GetListOfMethodArgs();
			bool argsMatch = true;
			bool prefer = true;

			for(int i = 0; i < (int)args->Length(); i++) {
				if(!paramMatches(((TMethodArg*)funcArgs->At(i))->GetFullTypeName(), ((TMethodArg*)funcArgs->At(i))->GetTypeName(), args->Get(i), prefer)) {
					argsMatch = false;
					break;
				}
			}

			if(argsMatch) {
				validFunction = function;
				if(prefer)
					break;
			}
		}

		return validFunction;
	}

	FunctionProxy* FunctionProxyFactory::fromArgs(std::string const& name, TClass *scope, const v8::Local<v8::Array> args)
	{
		TFunction* function = determineFunction(name, scope, args);

		if(function == nullptr) {
			return nullptr;
		}

		return createFunctionProxy(function, scope);
	}

	bool FunctionProxyFactory::checkNumberBounds(const char *typeName, v8::Local<v8::Value> arg, bool &prefer) {
		std::string strTypeName(typeName);
		std::map<std::string, NumberInfo>::iterator it = basicNumberInfoMap.find(strTypeName);
		if(it == basicNumberInfoMap.end()) {
			return false;
		} else {
			double val;
			if(arg->IsNumber()) {
				val = v8::Local<v8::Number>::Cast(arg)->Value();
			} else if (arg->IsNumberObject()) {
				#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
				val = v8::Local<v8::NumberObject>::Cast(arg)->ValueOf();
				#else
				val = v8::Local<v8::NumberObject>::Cast(arg)->NumberValue();
				#endif
			} else {
				return false;
			}
			if((val - (long long)val) != 0.0 && val != 0) {
				if(!it->second.floatingPoint) {
					return false;
				}
				if(strTypeName.find("double") != 0) {
					prefer = false;
				}
			}
			if (val > it->second.maxValue || val < it->second.minValue) {
				return false;
			}
			return true;
		}
	}

	bool FunctionProxyFactory::paramMatches(const char *fullTypeName, const char *typeName, v8::Local<v8::Value> arg, bool &prefer)
	{
		if (Types::isV8Primitive(arg)) {
			//Resolve the type
			TDataType* type = Types::getTypeByName(std::string(fullTypeName));
			if(type == nullptr) {
				return false; //Not primitive...
			}

			TString typeName = type->GetTypeName();
			std::string stdTypeName(typeName.Data());

			std::map<std::string, v8BasicTypes>::iterator it = basicTypeMap.find(stdTypeName);
			if(it != basicTypeMap.end()) {
				switch(it->second) {
				case v8BasicTypes::STRING:
					return Types::isV8String(arg);
				case v8BasicTypes::CHAR:
					if(Types::isV8String(arg)) {
						v8::String::Utf8Value string(arg->ToString());
						std::string stdStr(*string);
						return stdStr.length() == 1;
					} else {
						return false;
					}
				case v8BasicTypes::NUMBER:
					if(Types::isV8Number(arg)) {
						return checkNumberBounds(typeName, arg, prefer);
					} else {
						return false;
					}
				case v8BasicTypes::BOOLEAN:
					return Types::isV8Boolean(arg);
				default:
					return false;
				}
			}
		} else {
			v8::Local<v8::Object> objectArg = v8::Local<v8::Object>::Cast(arg);
			if (objectArg->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT) {
				Toolbox::logError("Supplied JavaScript object contains an unexpected number of internal fields.");
				return false;
			}

			ObjectProxy *argProxy = static_cast<ObjectProxy*>(Nan::GetInternalFieldPointer(objectArg, Toolbox::ObjectProxyPtr));
			DictFuncPtr_t dictFunc = gClassTable->GetDict(argProxy->getTypeName());
			if(dictFunc == nullptr) {
				return false;
			} else {
				return strcmp(typeName, argProxy->getTypeName()) == 0 || dictFunc()->GetBaseClass(typeName) != nullptr;
			}
		}

		return false;
	}
}
