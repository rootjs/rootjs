#include "BooleanProxy.h"
#include "FunctionProxy.h"
#include "NumberProxy.h"
#include "ObjectProxy.h"
#include "StringProxy.h"
#include "Toolbox.h"

#include <map>
#include <sstream>
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

	FunctionProxy::FunctionProxy(void* address, TFunction *info, TClass* scope) : Proxy(address, scope), info(info)
	{
	}

	TFunction* FunctionProxy::getMetaInfo()
	{
		return info;
	}

	std::vector<ObjectProxy*> FunctionProxy::validateArgs(v8::FunctionCallbackInfo<v8::Value> args)
	{
		std::vector<ObjectProxy*> validatedArgs;

		TFunction method = *(this->getMetaInfo());
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
						void *arg = objectArg->GetAlignedPointerFromInternalField(Toolbox::v8ObjectInternalField::ObjectProxyPtr);
						validatedArgs.push_back(static_cast<ObjectProxy*>(arg));
					}
					else
					{
						std::ostringstream msgStream;
						msgStream << "Error while validating arg " << i << ": v8::Object at " << &(*(*args[i])) << " has no internal fields";
						Toolbox::throwException(msgStream.str());
					}
				}
				else
				{
					// Else, it must be a JavaScript primitive
					if (args[i]->IsBoolean())
					{
						// TODO proper function validation
						if (BooleanProxy::isBoolean(expectedArg->GetTypeName()))
						{
							v8::Boolean *booleanArg = static_cast<v8::Boolean*>(*args[i]);
							bool value = booleanArg->Value();
							// TODO push_back ObjectProxy*
						}
						else
						{
							std::ostringstream msgStream;
							msgStream << "Error while validating arg " << i << ": Expected " << expectedArg->GetTypeNormalizedName() << " but got Boolean instead";
							Toolbox::throwException(msgStream.str());
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
							std::ostringstream msgStream;
							msgStream << "Error while validating arg " << i << ": Expected " << expectedArg->GetTypeNormalizedName() << " but got Number instead";
							Toolbox::throwException(msgStream.str());
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
							std::ostringstream msgStream;
							msgStream << "Error while validating arg " << i << ": Expected " << expectedArg->GetTypeNormalizedName() << " but got String instead";
							Toolbox::throwException(msgStream.str());
						}
					}
					else
					{
						std::ostringstream msgStream;
						msgStream << "Error while validating arg " << i << ": v8::Value at " << &(*(*args[i])) << "is neither a v8::Object nor a v8::Primitive";
						Toolbox::throwException(msgStream.str());
					}
				}
			}
		}
		else
		{
			std::ostringstream msgStream;
			msgStream << method.GetName();

			if (method.GetNargs() == method.GetNargsOpt())
			{
				msgStream << " takes exactly " << method.GetNargs();
			}
			else if (args.Length() < method.GetNargs())
			{
				msgStream << " takes at least " << method.GetNargs();
			}
			else if (args.Length() > method.GetNargsOpt())
			{
				msgStream << " takes at most " << method.GetNargsOpt();
			}

			msgStream << " arguments (" << args.Length() << " given)";
			Toolbox::throwException(msgStream.str());
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
