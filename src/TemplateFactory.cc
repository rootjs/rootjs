#include "TemplateFactory.h"

#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "FunctionProxy.h"
#include "FunctionProxyFactory.h"

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
	std::map<std::string, v8::Persistent<v8::FunctionTemplate>> TemplateFactory::templates;

	TemplateFactory::TemplateFactory()
	{}

	TemplateFactory::~TemplateFactory()
	{}

	v8::Local<v8::FunctionTemplate> TemplateFactory::createTemplate(TClass *clazz)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		std::string className(clazz->GetName());

		// Check if template has been already created
		if (templates.count(className) && !templates[className].IsEmpty())
		{
			return v8::Local<v8::FunctionTemplate>::New(isolate, templates[className]);
		}

		v8::Local<v8::FunctionTemplate> tmplt = v8::FunctionTemplate::New(isolate, CallbackHandler::ctorCallback, v8::String::NewFromUtf8(isolate, className.c_str()));
		tmplt->SetClassName(v8::String::NewFromUtf8(isolate, className.c_str()));

		v8::Local<v8::ObjectTemplate> prototype = tmplt->PrototypeTemplate();
		prototype->SetInternalFieldCount(1);

		v8::Local<v8::ObjectTemplate> instance = tmplt->InstanceTemplate();
		instance->SetInternalFieldCount(1); // each instance stores a reference to an ObjectProxy
		// instance->SetNamedPropertyHandler();

		TIter funcIter(clazz->GetListOfAllPublicMethods(kTRUE));
		TMethod *method = nullptr;

		while ( (method = (TMethod*) funcIter()))
		{
			if (method == nullptr || !method->IsValid())
			{
				// TODO: log this
				continue;
			}

			// skip abstract or pure virtual functions
			Long_t property = method->Property();
			if ((property & kIsPureVirtual)) // (property & kIsAbstract)
			{
				continue;
			}

			/*
			 * TODO: make overridden or overloaded methods only occur once
			 */
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
					std::string methodName(className);
					methodName.append("::");
					methodName.append(method->GetName());

					FunctionProxy *proxy = FunctionProxyFactory::createFunctionProxy(method, clazz);
					CallbackHandler::registerStaticFunction(methodName, proxy);

					prototype->Set(v8::String::NewFromUtf8(isolate, method->GetName()), v8::Function::New(isolate, CallbackHandler::staticFunctionCallback, v8::String::NewFromUtf8(isolate, className.c_str())));
				}
				else
				{
					// std::cout << "non static " << method->GetName() << std::endl;
					instance->Set(v8::String::NewFromUtf8(isolate, method->GetName()), v8::Function::New(isolate, CallbackHandler::memberFunctionCallback, v8::String::NewFromUtf8(isolate, className.c_str())));
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
				// TODO: log this
				continue;
			}

			if(member->Property() & kIsStatic)
			{
				void *address = (void*) member->GetOffsetCint();

				TClass *memberClazz = classFromName(member->GetTypeName());
				if(memberClazz == nullptr)
				{
					// TODO: log this
					continue;
				}
				TClassRef classRef(memberClazz);

				std::string memberName(memberClazz->GetName());
				memberName.append("::");
				memberName.append(member->GetName());

				ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(address, classRef);
				CallbackHandler::registerStaticObject(memberName, proxy);

				prototype->SetAccessor(v8::String::NewFromUtf8(isolate, member->GetName()), CallbackHandler::staticGetterCallback, CallbackHandler::staticSetterCallback);
			}
			else
			{
				instance->SetAccessor(v8::String::NewFromUtf8(isolate, member->GetName()), CallbackHandler::memberGetterCallback, CallbackHandler::memberSetterCallback);
			}

		}

		templates[className].Reset(isolate, tmplt);
		v8::Local<v8::FunctionTemplate> tmp = v8::Local<v8::FunctionTemplate>::New(isolate, templates[className]);

		return tmp;
	}


	TClass* TemplateFactory::classFromName(const char *className)
	{
		if(className == nullptr)
		{
			return nullptr;
		}

		DictFuncPtr_t dictPtr = gClassTable->GetDict(className);
		if(dictPtr == nullptr)
		{
			return nullptr;
		}

		return dictPtr();
	}
}
