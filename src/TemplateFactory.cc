#include "TemplateFactory.h"

#include "ObjectProxy.h"
#include "ObjectProxyFactory.h"
#include "FunctionProxy.h"
#include "FunctionProxyFactory.h"
#include "MemberInfo.h"
#include "PointerInfo.h"
#include "EnumInfo.h"
#include "EnumConstInfo.h"
#include "EnumConstProxy.h"
#include "Types.h"
#include "Toolbox.h"

#include <string>

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
	std::map<std::string, Nan::Persistent<v8::FunctionTemplate>> TemplateFactory::classTemplates;
	std::map<std::string, Nan::Persistent<v8::FunctionTemplate>> TemplateFactory::structTemplates;

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
		Nan::EscapableHandleScope handle_scope;

		v8::Local<v8::Object> nspace = createNamespaceTemplate(clazz)->NewInstance();

		// populate namespace object
		PointerInfo info(nullptr, clazz->GetName()); // TODO replace with something like ClassInfo / NSpaceInfo
		Nan::SetInternalFieldPointer(nspace, Toolbox::ObjectProxyPtr, new ObjectProxy(info, clazz));
		Nan::SetInternalFieldPointer(nspace, Toolbox::PropertyMapPtr, new std::map<std::string, ObjectProxy*>()); // there are no non-static members

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

		v8::Local<v8::ObjectTemplate>  nspace = Nan::New<v8::ObjectTemplate>();
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
				{
					std::map<std::string, std::string>::const_iterator opNameIt = Types::operatorNames.find(method->GetName());
					if(opNameIt == Types::operatorNames.end())
					{
						Toolbox::logInfo(std::string("Operator: ") + method->GetName() + " not handled",1);
					}
					else
					{
						v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(method->GetName(), clazz);
						nspace->Set(Nan::New(opNameIt->second.c_str()).ToLocalChecked(), Nan::New<v8::Function>(CallbackHandler::staticFunctionCallback, data));
					}
				}
				break;
			default:

				v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(methodName, clazz);
				nspace->Set(Nan::New(methodName.c_str()).ToLocalChecked(), Nan::New<v8::Function>(CallbackHandler::staticFunctionCallback, data));
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
				std::string fullTypeName(member->GetFullTypeName());
				int derefCount = std::count(fullTypeName.begin(), fullTypeName.end(), '*');

				MemberInfo info(*member, (void*)(member->GetOffsetCint()), derefCount + 2);	// direct cast to void* works because sizeof(void*) equals sizeof(Long_t)
				ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(info, clazz);

				if(proxy != nullptr)	// don't expose members that could not be encapsulated
				{
					v8::Local<v8::Value> data = CallbackHandler::registerStaticObject(member->GetName(), clazz, proxy);
					Nan::SetAccessor(nspace, Nan::New(member->GetName()).ToLocalChecked(), CallbackHandler::staticGetterCallback, CallbackHandler::staticSetterCallback, data);
				}
			}
		}

		return nspace;
	}

	v8::Local<v8::Object> TemplateFactory::encapsulateEnum(TEnum *eNum) throw(std::invalid_argument)
	{
		if(eNum == nullptr || !eNum->IsValid())
		{
			throw std::invalid_argument("Specified TEnum is null or not loaded.");
		}

		Nan::EscapableHandleScope handle_scope;

		v8::Local<v8::Object> instance = createEnumTemplate(eNum)->NewInstance();

		// populate enum object
		EnumInfo info(*eNum);
		ObjectProxy *proxy = new ObjectProxy(info, eNum->GetClass());
		Nan::SetInternalFieldPointer(instance, Toolbox::ObjectProxyPtr, proxy);
		proxy->setProxy(instance);

		std::map<std::string, ObjectProxy*>* propertyMap = new std::map<std::string, ObjectProxy*>();
		TIter enumIter(eNum->GetConstants());
		TEnumConstant *eConst = nullptr;
		while ( (eConst = (TEnumConstant*) enumIter()))
		{
			if(eConst->IsValid() && (eConst->Property() & kIsEnum))
			{
				EnumConstInfo constInfo(*eConst, true);	// make explicitly constant
				(*propertyMap)[std::string(eConst->GetName())] = new EnumConstProxy(constInfo, eNum->GetClass());
			}
		}
		Nan::SetInternalFieldPointer(instance, Toolbox::PropertyMapPtr, propertyMap);

		return handle_scope.Escape(instance);
	}

	v8::Local<v8::ObjectTemplate> TemplateFactory::createEnumTemplate(TEnum *eNum) throw(std::invalid_argument)
	{
		if(eNum == nullptr || !eNum->IsValid())
		{
			throw std::invalid_argument("Specified TEnum is null or not loaded.");
		}

		v8::Local<v8::ObjectTemplate> tmplt = Nan::New<v8::ObjectTemplate>();
		std::string enumName(eNum->GetQualifiedName());
		tmplt->SetInternalFieldCount(Toolbox::INTERNAL_FIELD_COUNT); // each instance stores a map containing the property proxies

		TIter enumIter(eNum->GetConstants());
		TEnumConstant *eConst = nullptr;
		while ( (eConst = (TEnumConstant*) enumIter()))
		{
			if(eConst->IsValid() && (eConst->Property() & kIsEnum))
			{
				Nan::SetAccessor(tmplt, Nan::New(eConst->GetName()).ToLocalChecked(), CallbackHandler::memberGetterCallback, CallbackHandler::memberSetterCallback);
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

		Nan::EscapableHandleScope handle_scope;

		std::string structName(clazz->GetName());

		// check if template has been created already
		if (structTemplates.count(structName) && !structTemplates[structName].IsEmpty())
		{
			return handle_scope.Escape(Nan::New(structTemplates[structName]));
		}

		v8::Local<v8::FunctionTemplate> tmplt = Nan::New<v8::FunctionTemplate>(CallbackHandler::ctorCallback, CallbackHandler::createFunctionCallbackData(clazz));
		tmplt->SetClassName(Nan::New(structName.c_str()).ToLocalChecked());

		// create template
		createInstantiableTemplate(clazz, tmplt);

		// store template in map
		structTemplates[structName].Reset(tmplt);

		return handle_scope.Escape(Nan::New(structTemplates[structName]));
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

		Nan::EscapableHandleScope handle_scope;

		std::string className(clazz->GetName());

		// check if template has been created already
		if (classTemplates.count(className) && !classTemplates[className].IsEmpty())
		{
			return handle_scope.Escape(Nan::New(classTemplates[className]));
		}

		v8::Local<v8::FunctionTemplate> tmplt = Nan::New<v8::FunctionTemplate>(CallbackHandler::ctorCallback, CallbackHandler::createFunctionCallbackData(clazz));
		tmplt->SetClassName(Nan::New(className.c_str()).ToLocalChecked());

		// create template
		createInstantiableTemplate(clazz, tmplt);

		// store template in map
		classTemplates[className].Reset(tmplt);

		return handle_scope.Escape(Nan::New(classTemplates[className]));
	}

	void TemplateFactory::createInstantiableTemplate(TClass *clazz, v8::Local<v8::FunctionTemplate> tmplt) throw(std::invalid_argument)
	{
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
							tmplt->Set(Nan::New(opNameIt->second.c_str()).ToLocalChecked(), Nan::New<v8::Function>(CallbackHandler::staticFunctionCallback, data));
						}
						else
						{
							v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(method->GetName(), clazz);
							instance->Set(Nan::New(opNameIt->second.c_str()).ToLocalChecked(), Nan::New<v8::Function>(CallbackHandler::memberFunctionCallback, data));
						}
					}
				}
				break;
			default:

				if (property & kIsStatic)
				{
					v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(methodName, clazz);
					tmplt->Set(Nan::New(methodName.c_str()).ToLocalChecked(), Nan::New<v8::Function>(CallbackHandler::staticFunctionCallback, data));
				}
				else
				{
					v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(methodName, clazz);
					instance->Set(Nan::New(methodName.c_str()).ToLocalChecked(), Nan::New<v8::Function>(CallbackHandler::memberFunctionCallback, data));
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
				std::string fullTypeName(member->GetFullTypeName());
				int derefCount = std::count(fullTypeName.begin(), fullTypeName.end(), '*');

				MemberInfo info(*member, (void*)(member->GetOffsetCint()), derefCount + 2);	// direct cast to void* works because sizeof(void*) equals sizeof(Long_t)
				ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(info, clazz);

				if(proxy != nullptr)	// don't expose members that could not be encapsulated
				{
					v8::Local<v8::Value> data = CallbackHandler::registerStaticObject(member->GetName(), clazz, proxy);
					#if (NODE_MODULE_VERSION < NODE_0_12_MODULE_VERSION)
					tmplt->Set(Nan::New(member->GetName()).ToLocalChecked(),
						Nan::New("Not supported in node 0.10").ToLocalChecked(),
						v8::ReadOnly);
					#else
					tmplt->SetNativeDataProperty(
							Nan::New(member->GetName()).ToLocalChecked(),
							CallbackHandler::staticGetterCallback,
							CallbackHandler::staticSetterCallback,
							data);
					#endif
				}
			}

			Nan::SetAccessor(instance,
					Nan::New(member->GetName()).ToLocalChecked(),
					CallbackHandler::memberGetterCallback,
					CallbackHandler::memberSetterCallback);


		}
	}

	void TemplateFactory::addEnumTemplate(TClass *clazz, v8::Local<v8::ObjectTemplate> tmplt) throw(std::invalid_argument)
	{
		TIter enumIter(clazz->GetListOfEnums(kTRUE));
		TEnum *eNum = nullptr;

		while ((eNum = (TEnum*) enumIter()))
		{
			if (eNum == nullptr || !eNum->IsValid())
			{
				Toolbox::logInfo("Invalid enum found in '" + std::string(clazz->GetName()) + "'.", 1);
				continue;
			}

			// can not use eNum->Property() & kIsPublic
			v8::Local<v8::Value> data = CallbackHandler::registerStaticObject(eNum->GetName(), clazz, (ObjectProxy*) Nan::GetInternalFieldPointer(encapsulateEnum(eNum), Toolbox::ObjectProxyPtr));
			Nan::SetAccessor(tmplt,
					Nan::New(eNum->GetName()).ToLocalChecked(),
					CallbackHandler::staticGetterCallback,
					CallbackHandler::staticSetterCallback,
					data);
		}

	}

	void TemplateFactory::addEnumTemplate(TClass *clazz, v8::Local<v8::FunctionTemplate> tmplt) throw(std::invalid_argument)
	{
		TIter enumIter(clazz->GetListOfEnums(kTRUE));
		TEnum *eNum = nullptr;

		while ((eNum = (TEnum*) enumIter()))
		{
			if (eNum == nullptr || !eNum->IsValid())
			{
				Toolbox::logInfo("Invalid enum found in '" + std::string(clazz->GetName()) + "'.", 1);
				continue;
			}

			// can not use eNum->Property() & kIsPublic
			v8::Local<v8::Value> data = CallbackHandler::registerStaticObject(eNum->GetName(), clazz, (ObjectProxy*) Nan::GetInternalFieldPointer(encapsulateEnum(eNum), Toolbox::ObjectProxyPtr));
			#if (NODE_MODULE_VERSION < NODE_0_12_MODULE_VERSION)
			tmplt->Set(Nan::New(eNum->GetName()).ToLocalChecked(),
				Nan::New("Not supported in node < 0.12").ToLocalChecked(),
				v8::ReadOnly);
			#else
			tmplt->SetNativeDataProperty(
					Nan::New(eNum->GetName()).ToLocalChecked(),
					CallbackHandler::staticGetterCallback,
					CallbackHandler::staticSetterCallback,
					data);
			#endif
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
