#include "FunctionProxy.h"
#include "ObjectProxy.h"

#include <map>
#include <string>
#include <vector>

#include <v8.h>

#include <TClassRef.h>
#include <TCollection.h>
#include <TFunction.h>
#include <TIterator.h>
#include <TList.h>
#include <TMethodArg.h>

namespace rootJS {
	std::map<TFunction*, CallFunc_t*> FunctionProxy::functions;

	CallFunc_t* FunctionProxy::getCallFunc(TFunction* method)
	{
		std::map<TFunction*, CallFunc_t*>::iterator iterator = FunctionProxy::functions.find(method);
		if (iterator != FunctionProxy::functions.end())
		{
			return iterator->second;
		}
		else
		{
			// TODO handle TFunction not in functions cache
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

	FunctionProxy::FunctionProxy(void* address, TFunction function, TClassRef scope)
		: Proxy(address, function, scope)
	{
		// TODO
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
						if (strcmp(expectedArg->GetTypeName(), "bool") == 0)
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
						// TODO
					}
					else if (args[i]->IsString())
					{
						// TODO
					}
					else
					{
						// TODO
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

	// TODO
	/*ObjectProxy FunctionProxy::call(ObjectProxy args[]) const
	{
	}

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
