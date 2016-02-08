#include "TemplateFactory.h"

#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "FunctionProxy.h"
#include "FunctionProxyFactory.h"
#include "MemberInfo.h"
#include "Toolbox.h"

#include "TClassTable.h"
#include "TMethod.h"
#include "TDataMember.h"

#include "TCollection.h"
#include "TList.h"

#include "RtypesCore.h"
#include "Rtypes.h"

namespace rootJS
{
	// Initialize static class members
	std::map<std::string, v8::Persistent<v8::FunctionTemplate>> TemplateFactory::classTemplates;
	std::map<std::string, v8::Persistent<v8::FunctionTemplate>> TemplateFactory::structTemplates;

	TemplateFactory::TemplateFactory()
	{}

	TemplateFactory::~TemplateFactory()
	{}

	v8::Local<v8::Object> TemplateFactory::getInstance(TClass *clazz) throw(std::invalid_argument)
	{
		if(clazz == nullptr)
		{
			// Toolbox::throwException(std::string("Specified TClass is null."));
			throw std::invalid_argument(std::string("Specified TClass is null."));
		}

		if(!clazz->IsLoaded())
		{
			// Toolbox::throwException(std::string("Specified TClass is not loaded."));
			throw std::invalid_argument(std::string("Specified TClass is not loaded."));
		}

		if (clazz->Property() & kIsNamespace)
		{
			return createNamespaceTemplate(clazz)->NewInstance();
		}
		else if (clazz->Property() & kIsClass)
		{
			return createClassTemplate(clazz)->GetFunction()->NewInstance();
			// return getConstructor(clazz)->NewInstance();
		}
		else if (clazz->Property() & kIsStruct)
		{
			return createStructTemplate(clazz)->GetFunction()->NewInstance();
			// return getConstructor(clazz)->NewInstance();
		}
		else if (clazz->Property() & kIsEnum)
		{
			return createEnumTemplate(clazz)->NewInstance();
		}
		/*
		 * else if (clazz->Property() & kIsUnion)
		 * {
		 * 	return createUnionTemplate(clazz)->Instance();
		 * }
		 * 	else if (clazz->Property() & kIsArray)
		 * 	{
		 * 		return createArrayTemplate(clazz)->Instance();
		 * 	}
		 */
		else
		{
			throw std::invalid_argument(std::string("The type of the specified TClass is not supported."));
		}
	}

	v8::Local<v8::Function> TemplateFactory::getConstructor(TClass *clazz) throw(std::invalid_argument)
	{
		if(clazz == nullptr)
		{
			// Toolbox::throwException(std::string("Specified TClass is null."));
			throw std::invalid_argument(std::string("Specified TClass is null."));
		}

		if(!clazz->IsLoaded())
		{
			// Toolbox::throwException(std::string("Specified TClass is not loaded."));
			throw std::invalid_argument(std::string("Specified TClass is not loaded."));
		}

		if (clazz->Property() & kIsClass)
		{
			return createClassTemplate(clazz)->GetFunction();
		}
		else if (clazz->Property() & kIsStruct)
		{
			return createStructTemplate(clazz)->GetFunction();
		}
		else
		{
			throw std::invalid_argument(std::string("The type of the specified TClass is not supported."));
		}
	}

	/*
	 * v8::Local<v8::ObjectTemplate> TemplateFactory::createUnionTemplate(TClass *clazz) throw(std::invalid_argument)
	 * {}
	 *
	 * v8::Local<v8::ObjectTemplate> TemplateFactory::createArrayTemplate(TClass *clazz) throw(std::invalid_argument)
	 * {}
	*/

	v8::Local<v8::ObjectTemplate> TemplateFactory::createNamespaceTemplate(TClass *clazz) throw(std::invalid_argument)
	{
		throw std::invalid_argument(std::string("Not implemented yet."));
	}

	v8::Local<v8::ObjectTemplate> TemplateFactory::createEnumTemplate(TClass *clazz) throw(std::invalid_argument)
	{
		throw std::invalid_argument(std::string("Not implemented yet."));
	}

	v8::Local<v8::FunctionTemplate> TemplateFactory::createStructTemplate(TClass *clazz) throw(std::invalid_argument)
	{
		throw std::invalid_argument(std::string("Not implemented yet."));
	}

	v8::Local<v8::FunctionTemplate> TemplateFactory::createClassTemplate(TClass *clazz) throw(std::invalid_argument)
	{
		if(!isValid(clazz))
		{
			throw std::invalid_argument(std::string("Specified TClass is null or not loaded."));
		}

		if(clazz->Property() & kClassIsAbstract)
		{
			throw std::invalid_argument(std::string("Specified TClass is abstract."));
		}

		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		std::string className(clazz->GetName());

		// Check if template has been already created
		if (classTemplates.count(className) && !classTemplates[className].IsEmpty())
		{
			return v8::Local<v8::FunctionTemplate>::New(isolate, classTemplates[className]);
		}

		v8::Local<v8::FunctionTemplate> tmplt = v8::FunctionTemplate::New(isolate, CallbackHandler::ctorCallback, CallbackHandler::createFunctionCallbackData(clazz));
		tmplt->SetClassName(v8::String::NewFromUtf8(isolate, className.c_str()));

		v8::Local<v8::ObjectTemplate> prototype = tmplt->PrototypeTemplate();
		prototype->SetInternalFieldCount(1);

		v8::Local<v8::ObjectTemplate> instance = tmplt->InstanceTemplate();
		instance->SetInternalFieldCount(1); // each instance stores a reference to an ObjectProxy

		TIter funcIter(clazz->GetListOfAllPublicMethods(kTRUE));
		TMethod *method = nullptr;

		while ( (method = (TMethod*) funcIter()))
		{
			if (method == nullptr || !method->IsValid())
			{
				Toolbox::log("Invalid method found in " + className + ".");
				continue;
			}

			// skip abstract or pure virtual functions
			Long_t property = method->Property();
			if ((property & kIsPureVirtual)) // (property & kIsAbstract)
			{
				Toolbox::log("Skipped pure virtual method in " + className + ".");
				continue;
			}

			/*
			 * TODO: make overridden or overloaded methods only occur once
			 */

			Toolbox::log("Make overridden or overloaded methods only occur once.");

			switch (method->ExtraProperty())
			{
			case kIsConstructor:
			case kIsDestructor:
			case kIsConversion:
				// don't expose
				break;
			case kIsOperator:
				// TODO
				break;
			default:
				if (property & kIsStatic)
				{
					v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(method->GetName(), clazz);
					prototype->Set(v8::String::NewFromUtf8(isolate, method->GetName()), v8::Function::New(isolate, CallbackHandler::staticFunctionCallback, data));
				}
				else
				{
					v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(method->GetName(), clazz);
					instance->Set(v8::String::NewFromUtf8(isolate, method->GetName()), v8::Function::New(isolate, CallbackHandler::memberFunctionCallback, data));
				}
				break;
			}
		}


		TIter memberIter((TList*)clazz->GetListOfAllPublicDataMembers(kTRUE));
		TDataMember *member = nullptr;

		while ( (member = (TDataMember*) memberIter()))
		{
			if (member == nullptr || !member->IsValid())
			{
				Toolbox::log("Invalid member found in " + className + ".");
				continue;
			}

			if(member->Property() & kIsStatic)
			{
				// TODO: Call: ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(MemberInfo(member), clazz);
				CallbackHandler::registerStaticObject(member->GetName(), clazz, nullptr); /* member->GetName(), clazz, proxy*/

				prototype->SetAccessor(v8::String::NewFromUtf8(isolate, member->GetName()), CallbackHandler::staticGetterCallback, CallbackHandler::staticSetterCallback);
			}
			else
			{
				instance->SetAccessor(v8::String::NewFromUtf8(isolate, member->GetName()), CallbackHandler::memberGetterCallback, CallbackHandler::memberSetterCallback);
			}

		}

		classTemplates[className].Reset(isolate, tmplt);
		v8::Local<v8::FunctionTemplate> tmp = v8::Local<v8::FunctionTemplate>::New(isolate, classTemplates[className]);

		return tmp;
	}

	bool TemplateFactory::isValid(TClass* clazz)
	{
		return ((clazz != nullptr) && clazz->IsLoaded());
	}

}
