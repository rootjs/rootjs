#include "FunctionProxy.h"
#include "BooleanProxy.h"
#include "NumberProxy.h"
#include "ObjectProxy.h"
#include "StringProxy.h"

#include <map>
#include <string>
#include <vector>

#include <v8.h>

#include <TClassRef.h>
#include <TInterpreter.h>

#include <TCollection.h>
#include <TFunction.h>
#include <TIterator.h>
#include <TList.h>
#include <TMethodArg.h>

namespace rootJS {
	std::map<TFunction*, CallFunc_t*> FunctionProxy::functions;
	std::map<std::string, mappedTypes> FunctionProxy::typeMap = {
		{"char", mappedTypes::CHAR}
	};

	CallFunc_t* FunctionProxy::getCallFunc(const TClassRef& classRef, TFunction* method)
	{
		std::map<TFunction*, CallFunc_t*>::iterator iterator = FunctionProxy::functions.find(method);
		if (iterator != FunctionProxy::functions.end())
		{
			return iterator->second;
		}
		else
		{
			CallFunc_t* callf = nullptr;
			TFunction* func = (TFunction*)method;
			std::string callString = "";

			ClassInfo_t* gcl;
			gcl = classRef.GetClass() ? classRef->GetClassInfo() : nullptr;
			if ( ! gcl ) {
				gcl = gInterpreter->ClassInfo_Factory();
			}

			TCollection* method_args = func->GetListOfMethodArgs();
			TIter iarg( method_args );

			TMethodArg* method_arg = 0;
			while ((method_arg = (TMethodArg*)iarg.Next())) {
				std::string fullType = method_arg->GetTypeNormalizedName();
				if ( callString.empty() )
					callString = fullType;
				else
					callString += ", " + fullType;
			}

			Long_t offset = 0;
			callf = gInterpreter->CallFunc_Factory();

			gInterpreter->CallFunc_SetFuncProto(
			    callf,
			    gcl,
			    func ? func->GetName() : classRef->GetName(),
			    callString.c_str(),
			    func ? (func->Property() & kIsConstMethod) : kFALSE,
			    &offset,
			    ROOT::kExactMatch );

			if ( ! gInterpreter->CallFunc_IsValid( callf ) ) {
				v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Cling sucks"));
			} else {
				return callf;
			}

			return nullptr;

		}
	}

	std::vector<TFunction*> FunctionProxy::getMethodsFromName(TClassRef scope, std::string name)
	{
		std::vector<TFunction*> methods;

		TFunction *method;
		TIterator *iterator = scope.GetClass()->GetListOfMethodOverloads(name.c_str())->MakeIterator();
		while ( (method = dynamic_cast<TFunction*>(iterator->Next())) )
		{
			methods.push_back(method);
		}

		return methods;
	}

	FunctionProxy::FunctionProxy(void* address, TFunction* function, TClassRef scope)
		: Proxy(address, *function, scope)
	{
		function = function;
		argsReflection = function->GetListOfMethodArgs();
	}

	const TFunction& FunctionProxy::getType()
	{
		return dynamic_cast<const TFunction&>(Proxy::getType());
	}

	std::vector<ObjectProxy*> FunctionProxy::validateArgs(v8::FunctionCallbackInfo<v8::Value> args)
	{
		std::vector<ObjectProxy*> validatedArgs;

		TFunction method = this->getType();
		if (method.GetNargs() <= args.Length() && args.Length() <= method.GetNargsOpt())
		{
			TList *expectedArgs = method.GetListOfMethodArgs();
			for (int i = 0; i < args.Length(); i++)
			{
				TMethodArg *expectedArg = static_cast<TMethodArg*>(expectedArgs->At(i));

				// Check if the argument is a JavaScript object
				if (args[i]->IsObject())
				{
					v8::Object *objectArg = static_cast<v8::Object*>(*args[i]);
					if (objectArg->InternalFieldCount() > 0)
					{
						// TODO validate arg
						void *arg = objectArg->GetAlignedPointerFromInternalField(ObjectProxy::InternalField::Pointer);
						validatedArgs.push_back(static_cast<ObjectProxy*>(arg));
					}
					else
					{
						// TODO throw JS internalFieldNotFound exception
					}
				}
				else
				{
					// Else, it must be a JavaScript primitive
					if (args[i]->IsBoolean())
					{
						// TODO proper type validation
						if (BooleanProxy::isBoolean(expectedArg->GetTypeName()))
						{
							v8::Boolean *booleanArg = static_cast<v8::Boolean*>(*args[i]);
							bool value = booleanArg->Value();
							// TODO push_back ObjectProxy*
						}
						else
						{
							// TODO throw JS invalidArgument exception
						}
					}
					else if (args[i]->IsNumber())
					{
						if (NumberProxy::isNumber(expectedArg->GetTypeName()))
						{
							// TODO
						}
						else
						{
							// TODO throw JS invalidArgument exception
						}
					}
					else if (args[i]->IsString())
					{
						if (StringProxy::isString(expectedArg->GetTypeName()))
						{
							// TODO
						}
						else
						{
							// TODO throw JS invalidArgument exception
						}
					}
					else
					{
						// TODO throw unknownType exception
					}
				}
			}
		}
		else
		{
			// TODO throw JS invalidArgCount exception
		}

		return validatedArgs;
	}

	void* FunctionProxy::bufferParam(TMethodArg* arg, v8::Local<v8::Value> originalArg) {
		std::map<std::string, mappedTypes>::iterator iterator = typeMap.find(std::string(arg->GetTypeName()));
		if(iterator == typeMap.end()) {
			v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Jonas was too lazy to implement this..."));
			return nullptr;
		}
		switch(iterator->second) {
			case mappedTypes::CHAR:
			v8::String::Utf8Value string(originalArg->ToString());
			char *str = (char *) malloc(string.length() + 1);
			strcpy(str, *string);
			return str;
			break;
		}

		//TODO: This will explode - huge fireball
		return nullptr;
	}

	v8::Local<v8::Object> FunctionProxy::call(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		CallFunc_t* callFunc = (CallFunc_t*)address;
		if(!callFunc) {
			//TODO Handle this, should not segfault (maybe throw something...)
		}
		TInterpreter::CallFuncIFacePtr_t facePtr = gCling->CallFunc_IFacePtr( callFunc );
		void *self; //TODO?
		void *result; //TODO?
		std::vector<void*> buf( args.Length() );
		for(int i = 0; i < args.Length(); i++) {
			void* bufEl = bufferParam(((TMethodArg*)argsReflection->At(i)), args[i]);
			buf[i] = &bufEl;
		}

		switch(facePtr.fKind) {
		case (TInterpreter::CallFuncIFacePtr_t::kGeneric):

			facePtr.fGeneric(&self, args.Length(), buf.data(), &result);
			break;
		default:
			v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "Jonas was too lazy to implement this..."));
		}

		for(int i = 0; i < args.Length(); i++) {
			free((*((void**)buf[i])));
		}

		return v8::Object::New(v8::Isolate::GetCurrent());
	}
	/*
	// TODO
	bool FunctionProxy::processCall(TFunction* method, void* args, void* self, void* result)
	{
	}

	void* FunctionProxy::callConstructor(TFunction* method, TClassRef type, void* args)
	{
	}

	void FunctionProxy::callDestructor(TClassRef type, void* self)
	{
	}

	void* FunctionProxy::callObject(TFunction* method, void* self, void* args, TClassRef resType)
	{
	}

	template <typename T>
	T FunctionProxy::callPrimitive(TFunction* method, void* self, void* args)
	{
	}*/
}
