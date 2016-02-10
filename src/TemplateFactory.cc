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
	std::map<std::string, v8::Persistent<v8::FunctionTemplate>> TemplateFactory::classTemplates;
	std::map<std::string, v8::Persistent<v8::FunctionTemplate>> TemplateFactory::structTemplates;

	TemplateFactory::TemplateFactory()
	{}

	TemplateFactory::~TemplateFactory()
	{}

	v8::Local<v8::Object> TemplateFactory::getInstance(TClass *clazz) throw(std::invalid_argument)
	{
		if(!isValid(clazz))
		{
			throw std::invalid_argument("Specified TClass is null or not loaded.");
		}

		if (clazz->Property() & kIsNamespace)
		{
			return createNamespaceTemplate(clazz)->NewInstance();
		}
		else if (clazz->Property() & kIsClass)
		{
			return createClassTemplate(clazz)->InstanceTemplate()->NewInstance(); // creation without ctor callback
			// return createClassTemplate(clazz)->GetFunction()->NewInstance();   // creation with ctor callback
			// return getConstructor(clazz)->NewInstance();						  // creation with ctor callback
		}
		else if (clazz->Property() & kIsStruct)
		{
			return createStructTemplate(clazz)->InstanceTemplate()->NewInstance(); // creation without ctor callback
			// return createStructTemplate(clazz)->GetFunction()->NewInstance();   // creation with ctor callback
			// return getConstructor(clazz)->NewInstance();						   // creation with ctor callback
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
			throw std::invalid_argument("The type of the specified TClass '" + std::string(clazz->GetName()) + "'is not supported.");
		}
	}

	v8::Local<v8::Function> TemplateFactory::getConstructor(TClass *clazz) throw(std::invalid_argument)
	{
		if(!isValid(clazz))
		{
			throw std::invalid_argument("Specified TClass is null or not loaded.");
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
			throw std::invalid_argument("The type of the specified TClass '" + std::string(clazz->GetName()) + "'is not supported.");
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
		v8::Local<v8::ObjectTemplate> namespaceTemplate = v8::ObjectTemplate::New(v8::Isolate::GetCurrent());
		return namespaceTemplate;
	}

	v8::Local<v8::ObjectTemplate> TemplateFactory::createEnumTemplate(TClass *clazz) throw(std::invalid_argument)
	{
		throw std::invalid_argument("Not implemented yet.");
	}

	v8::Local<v8::FunctionTemplate> TemplateFactory::createStructTemplate(TClass *clazz) throw(std::invalid_argument)
	{
		if(!isValid(clazz))
		{
			throw std::invalid_argument("Specified TClass is null or not loaded.");
		}

		if(!(clazz->Property() & kIsStruct))
		{
			throw std::invalid_argument("Specified TClass '" + std::string(clazz->GetName()) + "' is not a struct.");
		}

		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		std::string structName(clazz->GetName());

		// check if template has been created already
		if (structTemplates.count(structName) && !structTemplates[structName].IsEmpty())
		{
			return handle_scope.Escape(v8::Local<v8::FunctionTemplate>::New(isolate, structTemplates[structName]));
		}

		v8::Local<v8::FunctionTemplate> tmplt = v8::FunctionTemplate::New(isolate, CallbackHandler::ctorCallback, CallbackHandler::createFunctionCallbackData(clazz));
		tmplt->SetClassName(v8::String::NewFromUtf8(isolate, structName.c_str()));

		// create template
		createInstantiableTemplate(clazz, tmplt);

		// store template in map
		structTemplates[structName].Reset(isolate, tmplt);

		return handle_scope.Escape(v8::Local<v8::FunctionTemplate>::New(isolate, structTemplates[structName]));
	}

	v8::Local<v8::FunctionTemplate> TemplateFactory::createClassTemplate(TClass *clazz) throw(std::invalid_argument)
	{
		if(!isValid(clazz))
		{
			throw std::invalid_argument("Specified TClass is null or not loaded.");
		}

		if(!(clazz->Property() & kIsClass))
		{
			throw std::invalid_argument("Specified TClass '" + std::string(clazz->GetName()) + "' is not a class.");
		}

		if(clazz->Property() & kClassIsAbstract)
		{
			throw std::invalid_argument("Specified TClass '" + std::string(clazz->GetName()) + "'is abstract.");
		}

		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		std::string className(clazz->GetName());

		// check if template has been created already
		if (classTemplates.count(className) && !classTemplates[className].IsEmpty())
		{
			return handle_scope.Escape(v8::Local<v8::FunctionTemplate>::New(isolate, classTemplates[className]));
		}

		v8::Local<v8::FunctionTemplate> tmplt = v8::FunctionTemplate::New(isolate, CallbackHandler::ctorCallback, CallbackHandler::createFunctionCallbackData(clazz));
		tmplt->SetClassName(v8::String::NewFromUtf8(isolate, className.c_str()));
		tmplt->PrototypeTemplate()->SetInternalFieldCount(1);

		// create template
		createInstantiableTemplate(clazz, tmplt);

		// store template in map
		classTemplates[className].Reset(isolate, tmplt);

		return handle_scope.Escape(v8::Local<v8::FunctionTemplate>::New(isolate, classTemplates[className]));
	}

	void TemplateFactory::createInstantiableTemplate(TClass *clazz, v8::Local<v8::FunctionTemplate> tmplt) throw(std::invalid_argument)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		std::string className(clazz->GetName());

		// add static functions and members to the prototype template
		v8::Local<v8::ObjectTemplate> prototype = tmplt->PrototypeTemplate();
		// prototype->SetInternalFieldCount(1);

		// add non-static functions and members to the instance template
		v8::Local<v8::ObjectTemplate> instance = tmplt->InstanceTemplate();
		instance->SetInternalFieldCount(Toolbox::INTERNAL_FIELD_COUNT); // each instance stores a map containing the property proxies

		/**
		 *	Add public methods as properties
		 */
		std::map<std::string, TFunction*> methods;

		TIter funcIter(clazz->GetListOfAllPublicMethods(kTRUE));
		TMethod *method = nullptr;

		while ( (method = (TMethod*) funcIter()))
		{
			if (method == nullptr || !method->IsValid())
			{
				Toolbox::log("Invalid method found in '" + className + "'.");
				continue;
			}

			std::string methodName(method->GetName());

			// Skip template functions
			if(isTemplateFunction(methodName))
			{
				// Toolbox::log("Skipped template method '" + methodName + "' in '" + className + "'.");
				continue;
			}

			// skip abstract or pure virtual functions
			Long_t property = method->Property();
			if ((property & kIsPureVirtual)) // (property & kIsAbstract)
			{
				// Toolbox::log("Skipped pure virtual method '" + methodName + "' in '" + className + "'.");
				continue;
			}

			// make overridden or overloaded methods only occur once
			if (methods.count(methodName))
			{
				// Toolbox::log("Already set method '" + methodName + "' as property in '" + className + "'.");
				continue;
			}
			else
			{
				methods[methodName] = method;
			}

			switch (method->ExtraProperty())
			{
			case kIsConstructor:
			case kIsDestructor:
			case kIsConversion:
				// don't expose
				break;
			case kIsOperator:
				// TODO: handle operators
				// Toolbox::log("Operator '" + methodName + "' found in '" + className + "'.");
				break;
			default:

				if (property & kIsStatic)
				{
					v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(methodName, clazz);
					prototype->Set(v8::String::NewFromUtf8(isolate, methodName.c_str()), v8::Function::New(isolate, CallbackHandler::staticFunctionCallback, data));
				}
				else
				{
					v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(methodName, clazz);
					instance->Set(v8::String::NewFromUtf8(isolate, methodName.c_str()), v8::Function::New(isolate, CallbackHandler::memberFunctionCallback, data));
				}
				break;
			}
		}

		/**
		 *	Add public members as properties
		 */
		TIter memberIter((TList*)clazz->GetListOfAllPublicDataMembers(kTRUE));
		TDataMember *member = nullptr;

		while ( (member = (TDataMember*) memberIter()))
		{
			if (member == nullptr || !member->IsValid())
			{
				Toolbox::log("Invalid member found in '" + className + "'.");
				continue;
			}

			if(member->Property() & kIsStatic)
			{
				MemberInfo info(*member, (void*)(member->GetOffsetCint()));	// direct cast to void* works because sizeof(void*) equals sizeof(Long_t)
				ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(info, clazz);
				CallbackHandler::registerStaticObject(member->GetName(), clazz, proxy);

				prototype->SetAccessor(v8::String::NewFromUtf8(isolate, member->GetName()), CallbackHandler::staticGetterCallback, CallbackHandler::staticSetterCallback);
			}
			else
			{
				instance->SetAccessor(v8::String::NewFromUtf8(isolate, member->GetName()), CallbackHandler::memberGetterCallback, CallbackHandler::memberSetterCallback);
			}

		}
	}

	bool TemplateFactory::isTemplateFunction(std::string const& functionName)
	{
		if(functionName.size() > 0)
		{
			return (functionName[functionName.size()-1] == '>') && (functionName.find('<') != std::string::npos);
		}

		return false;
	}

	bool TemplateFactory::isValid(TClass* clazz)
	{
		return ((clazz != nullptr) && clazz->IsLoaded());
	}
}
