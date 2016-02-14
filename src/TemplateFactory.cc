#include "TemplateFactory.h"

#include "ObjectProxy.h"
#include "NumberProxy.h"
#include "ObjectProxyFactory.h"
#include "FunctionProxy.h"
#include "FunctionProxyFactory.h"
#include "MemberInfo.h"
#include "PointerInfo.h"
#include "EnumInfo.h"
#include "EnumConstInfo.h"
#include "Types.h"
#include "Toolbox.h"

#include <string>
#include <iostream>

#include <TClassTable.h>
#include <TMethod.h>
#include <TDataMember.h>

#include <TCollection.h>
#include <TSeqCollection.h>
#include <TList.h>
#include <RtypesCore.h>
#include <Rtypes.h>

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
			return initializeNamespace(clazz);
		}
		else if (clazz->Property() & kIsClass)
		{
			return createClassTemplate(clazz)->InstanceTemplate()->NewInstance(); // creation without ctor callback
		}
		else if (clazz->Property() & kIsStruct)
		{
			return createStructTemplate(clazz)->InstanceTemplate()->NewInstance(); // creation without ctor callback

		}
		else if (clazz->Property() & kIsUnion)
		{
			return createUnionTemplate(clazz)->InstanceTemplate()->NewInstance(); // creation without ctor callback
		}
		else if (clazz->Property() & kIsArray)
		{
			return createArrayTemplate(clazz)->NewInstance();
		}
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
		else if (clazz->Property() & kIsUnion)
		{
			return createUnionTemplate(clazz)->GetFunction();
		}
		else
		{
			throw std::invalid_argument("The type of the specified TClass '" + std::string(clazz->GetName()) + "'is not supported.");
		}
	}

	v8::Local<v8::Object> TemplateFactory::initializeNamespace(TClass *clazz) throw(std::invalid_argument)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		v8::Local<v8::Object> nspace = createNamespaceTemplate(clazz)->NewInstance();

		// populate namespace object
		PointerInfo info(nullptr, clazz->GetName()); // TODO replace with something like ClassInfo / NSpaceInfo
		nspace->SetAlignedPointerInInternalField(Toolbox::ObjectProxyPtr, new ObjectProxy(info, clazz));
		nspace->SetAlignedPointerInInternalField(Toolbox::PropertyMapPtr, new std::map<std::string, ObjectProxy*>()); // there are no non-static members

		return handle_scope.Escape(nspace);
	}

	v8::Local<v8::ObjectTemplate> TemplateFactory::createNamespaceTemplate(TClass *clazz) throw(std::invalid_argument)
	{
		if(!isValid(clazz))
		{
			throw std::invalid_argument("Specified TClass is null or not loaded.");
		}

		if(!(clazz->Property() & kIsNamespace))
		{
			throw std::invalid_argument("Specified TClass '" + std::string(clazz->GetName()) + "' is not a namespace.");
		}

		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::Local<v8::ObjectTemplate>  nspace = v8::ObjectTemplate::New(isolate);
		std::string className(clazz->GetName());
		nspace->SetInternalFieldCount(Toolbox::INTERNAL_FIELD_COUNT); // each instance stores a map containing the property proxies

		/**
		 *  Add enums
		 */
		addEnumTemplate(clazz, nspace);

		/**
		 *	Add public methods as properties
		 */
		std::map<std::string, TFunction*> methods;
		TIter funcIter(clazz->GetListOfAllPublicMethods(kTRUE));
		TMethod* method;
		while ( (method = (TMethod*) funcIter()))
		{
			if (method == nullptr || !method->IsValid())
			{
				Toolbox::logError(std::string("Invalid method found in '").append(className).append("'."));
				continue;
			}

			std::string methodName(method->GetName());

			// Skip template functions
			if(isTemplateFunction(methodName))
			{
				Toolbox::logInfo("Skipped template method '" + methodName + "' in '" + className + "'.",1);
				continue;
			}
			// make overridden or overloaded methods only occur once
			if (methods.count(methodName))
			{
				// Toolbox::logInfo("Already set method '" + methodName + "' as property in '" + className + "'.");
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
				// Toolbox::logInfo("Operator '" + methodName + "' found in '" + className + "'.");
				break;
			default:

				v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(methodName, clazz);
				nspace->Set(v8::String::NewFromUtf8(isolate, methodName.c_str()), v8::Function::New(isolate, CallbackHandler::staticFunctionCallback, data));
				break;
			}
		}
		/**
		    *	Add public static members as properties
		    */
		TIter memberIter(clazz->GetListOfAllPublicDataMembers(kTRUE));
		TDataMember *member;
		while ( (member = (TDataMember*) memberIter()))
		{
			if (member == nullptr || !member->IsValid())
			{
				Toolbox::logError("Invalid member found in '" + className + "'.");
				continue;
			}

			if(member->Property() & kIsStatic)
			{
				MemberInfo info(*member, (void*)(member->GetOffsetCint()));	// direct cast to void* works because sizeof(void*) equals sizeof(Long_t)
				ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(info, clazz);
				v8::Local<v8::Value> data = CallbackHandler::registerStaticObject(member->GetName(), clazz, proxy);
				nspace->SetAccessor(v8::String::NewFromUtf8(isolate, member->GetName()), CallbackHandler::staticGetterCallback, CallbackHandler::staticSetterCallback, data);
			}
		}

		return nspace;
	}

	v8::Local<v8::Object> TemplateFactory::initializeEnum(TEnum *eNum) throw(std::invalid_argument)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		v8::Local<v8::Object> instance = createEnumTemplate(eNum)->NewInstance();

		// populate enum object
		EnumInfo info(*eNum);
		ObjectProxy *proxy = new ObjectProxy(info, eNum->GetClass());
		instance->SetAlignedPointerInInternalField(Toolbox::ObjectProxyPtr, proxy);
		proxy->setProxy(instance);

		std::map<std::string, ObjectProxy*>* propertyMap = new std::map<std::string, ObjectProxy*>();
		TIter enumIter(eNum->GetConstants());
		TEnumConstant *eConst = nullptr;
		while ( (eConst = (TEnumConstant*) enumIter()))
		{
			if(eConst->IsValid())
			{
				EnumConstInfo constInfo(*eConst);
				(*propertyMap)[std::string(eConst->GetName())] = NumberProxy::llongConstruct(constInfo, eNum->GetClass());
			}
		}
		instance->SetAlignedPointerInInternalField(Toolbox::PropertyMapPtr, propertyMap);

		return handle_scope.Escape(instance);
	}

	v8::Local<v8::ObjectTemplate> TemplateFactory::createEnumTemplate(TEnum *eNum) throw(std::invalid_argument)
	{
		if(eNum == nullptr || !eNum->IsValid())
		{
			throw std::invalid_argument("Specified TEnum is null or not loaded.");
		}

		v8::Isolate *isolate = v8::Isolate::GetCurrent();

		v8::Local<v8::ObjectTemplate> tmplt = v8::ObjectTemplate::New(isolate);
		std::string enumName(eNum->GetQualifiedName());
		tmplt->SetInternalFieldCount(Toolbox::INTERNAL_FIELD_COUNT); // each instance stores a map containing the property proxies

		TIter enumIter(eNum->GetConstants());
		TEnumConstant *eConst = nullptr;
		while ( (eConst = (TEnumConstant*) enumIter()))
		{
			if(eConst->IsValid())
			{
				tmplt->SetAccessor(v8::String::NewFromUtf8(isolate, eConst->GetName()), CallbackHandler::memberGetterCallback, CallbackHandler::memberSetterCallback);
			}
		}

		return tmplt;
	}

	v8::Local<v8::ObjectTemplate> TemplateFactory::createArrayTemplate(TClass *clazz) throw(std::invalid_argument)
	{
		throw std::invalid_argument("Not implemented yet.");
	}

	v8::Local<v8::FunctionTemplate> TemplateFactory::createUnionTemplate(TClass *clazz) throw(std::invalid_argument)
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
		// v8::Local<v8::ObjectTemplate> prototype = tmplt->PrototypeTemplate();

		// add non-static functions and members to the instance template
		v8::Local<v8::ObjectTemplate> instance = tmplt->InstanceTemplate();
		instance->SetInternalFieldCount(Toolbox::INTERNAL_FIELD_COUNT); // each instance stores a map containing the property proxies

		/**
		 *  Add enums
		 */
		addEnumTemplate(clazz, tmplt);

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
				Toolbox::logError(std::string("Invalid method found in '").append(className).append("'."));
				continue;
			}

			std::string methodName(method->GetName());

			// Skip template functions
			if(isTemplateFunction(methodName))
			{
				// Toolbox::logInfo("Skipped template method '" + methodName + "' in '" + className + "'.");
				continue;
			}

			// skip abstract or pure virtual functions
			Long_t property = method->Property();
			if ((property & kIsPureVirtual))
			{ // (property & kIsAbstract)
				// Toolbox::logInfo("Skipped pure virtual method '" + methodName + "' in '" + className + "'.");
				//TODO? continue;
			}

			// make overridden or overloaded methods only occur once
			if (methods.count(methodName))
			{
				// Toolbox::logInfo("Already set method '" + methodName + "' as property in '" + className + "'.");
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
				{
					std::map<std::string, std::string>::const_iterator opNameIt = Types::operatorNames.find(method->GetName());
					if(opNameIt == Types::operatorNames.end())
					{
						Toolbox::logInfo(std::string("Operator: ") + method->GetName() + " not handled",1);
					}
					else
					{
						if (property & kIsStatic)
						{
							v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(method->GetName(), clazz);
							tmplt->Set(v8::String::NewFromUtf8(isolate, opNameIt->second.c_str()), v8::Function::New(isolate, CallbackHandler::staticFunctionCallback, data));
						}
						else
						{
							v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(method->GetName(), clazz);
							instance->Set(v8::String::NewFromUtf8(isolate, opNameIt->second.c_str()), v8::Function::New(isolate, CallbackHandler::memberFunctionCallback, data));
						}
					}
				}
				break;
			default:

				if (property & kIsStatic)
				{
					v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(methodName, clazz);
					tmplt->Set(v8::String::NewFromUtf8(isolate, methodName.c_str()), v8::Function::New(isolate, CallbackHandler::staticFunctionCallback, data));
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
				Toolbox::logError("Invalid member found in '" + className + "'.");
				continue;
			}

			if(member->Property() & kIsStatic)
			{
				MemberInfo info(*member, (void*)(member->GetOffsetCint()));	// direct cast to void* works because sizeof(void*) equals sizeof(Long_t)
				ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(info, clazz);
				v8::Local<v8::Value> data = CallbackHandler::registerStaticObject(member->GetName(), clazz, proxy);

				tmplt->SetNativeDataProperty(v8::String::NewFromUtf8(isolate, member->GetName()), CallbackHandler::staticGetterCallback, CallbackHandler::staticSetterCallback, data);
			}
			else
			{
				instance->SetAccessor(v8::String::NewFromUtf8(isolate, member->GetName()), CallbackHandler::memberGetterCallback, CallbackHandler::memberSetterCallback);
			}

		}
	}

	void TemplateFactory::addEnumTemplate(TClass *clazz, v8::Local<v8::ObjectTemplate> tmplt) throw(std::invalid_argument)
	{
		TIter enumIter(clazz->GetListOfEnums(kTRUE));
		TEnum *eNum = nullptr;

		while ((eNum = (TEnum*) enumIter()))
		{
			if (eNum == nullptr || !eNum->IsValid())
			{ // assert eNum->GetClass() == clazz
				Toolbox::logError("Invalid enum found in '" + std::string(clazz->GetName()) + "'.");
				continue;
			}

			v8::Local<v8::Value> data = CallbackHandler::registerStaticObject(eNum->GetName(), clazz, (ObjectProxy*) initializeEnum(eNum)->GetAlignedPointerFromInternalField(Toolbox::ObjectProxyPtr));
			tmplt->SetAccessor(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), eNum->GetName()), CallbackHandler::staticGetterCallback, CallbackHandler::staticSetterCallback, data);
		}

	}

	void TemplateFactory::addEnumTemplate(TClass *clazz, v8::Local<v8::FunctionTemplate> tmplt) throw(std::invalid_argument)
	{
		TIter enumIter(clazz->GetListOfEnums(kTRUE));
		TEnum *eNum = nullptr;

		while ((eNum = (TEnum*) enumIter()))
		{
			if (eNum == nullptr || !eNum->IsValid())
			{ // assert eNum->GetClass() == clazz
				Toolbox::logError("Invalid enum found in '" + std::string(clazz->GetName()) + "'.");
				continue;
			}

			v8::Local<v8::Value> data = CallbackHandler::registerStaticObject(eNum->GetName(), clazz, (ObjectProxy*) initializeEnum(eNum)->GetAlignedPointerFromInternalField(Toolbox::ObjectProxyPtr));
			tmplt->SetNativeDataProperty(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), eNum->GetName()), CallbackHandler::staticGetterCallback, CallbackHandler::staticSetterCallback, data);
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

	bool TemplateFactory::isValid(TClass *clazz)
	{
		return ((clazz != nullptr) && clazz->IsLoaded());
	}
}
