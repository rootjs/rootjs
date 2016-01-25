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

namespace RootJS {
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
		std::vector<ObjectProxy*> proxiedArgs;
		//TFunction method = this->getType();

		v8::Object *objectArg;
		void *arg;
		for (int i = 0; i < args.Length(); i++)
		{
			// Check if the argument is a JavaScript object
			if (args[i]->IsObject())
			{
				objectArg = static_cast<v8::Object*>(*args[i]);
				if (objectArg->InternalFieldCount() > 0)
				{
					arg = objectArg->GetAlignedPointerFromInternalField(ObjectProxy::InternalField::Pointer);
					proxiedArgs.push_back(static_cast<ObjectProxy*>(arg));
				}
				else
				{
					// TODO throw JS exception
				}
			}
			else
			{
				// Else, it must be a JavaScript primitive
				if (args[i]->IsBoolean())
				{
					// TODO
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
					// TODO throw JS exception
				}
			}
		}

		return proxiedArgs;
	}

	ObjectProxy FunctionProxy::call(ObjectProxy args[]) const
	{
		// TODO
	}

	bool FunctionProxy::processCall(TFunction* method, void* args, void* self, void* result)
	{
		// TODO
	}

	void* FunctionProxy::callConstructor(TFunction* method, TClassRef type, void* args)
	{
		// TODO
	}

	void FunctionProxy::callDestructor(TClassRef type, void* self)
	{
		// TODO
	}

	void* FunctionProxy::callObject(TFunction* method, void* self, void* args, TClassRef resType)
	{
		// TODO
	}

	template <typename T>
	T FunctionProxy::callPrimitive(TFunction* method, void* self, void* args)
	{
		// TODO
	}
}
