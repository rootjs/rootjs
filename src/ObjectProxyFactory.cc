#include "ObjectProxyFactory.h"

#include "Proxy.h"
#include "NumberProxy.h"
#include "StringProxy.h"
#include "BooleanProxy.h"
#include "VoidPointerProxy.h"

#include "Toolbox.h"
#include "Types.h"

#include "MemberInfo.h"
#include "GlobalInfo.h"
#include "PointerInfo.h"

#include "TemplateFactory.h"
#include "CallbackHandler.h"

#include <TROOT.h>
#include <TClassTable.h>
#include <TDataMember.h>
#include <TDataType.h>
#include <TList.h>
#include <TMethod.h>
#include <TEnum.h>
#include <Rtypes.h>
#include <RtypesCore.h>

namespace rootJS
{

	std::map<std::string, ProxyInitializator> ObjectProxyFactory::primitiveProxyMap;

	ObjectProxy* ObjectProxyFactory::createObjectProxy(MetaInfo &info, TClass *scope) throw(std::invalid_argument)
	{
		return createObjectProxy(info, scope, nullptr);
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(MetaInfo &info, TClass *scope, v8::Local<v8::Object> instance) throw(std::invalid_argument)
	{
		return createObjectProxy(info, scope, &instance);
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(MetaInfo &info, TClass *scope,  v8::Local<v8::Object>* instancePtr) throw(std::invalid_argument)
	{
		ObjectProxy* proxy = createPrimitiveProxy(info, scope);
		if(proxy != nullptr)
		{
			return proxy;
		}

		proxy = createEnumProxy(info, scope);
		if(proxy != nullptr)
		{
			return proxy;
		}

		TClass *type = getClass(std::string(info.getTypeName()));
		if(type == nullptr)
		{
			/*
			* Toolbox::logInfo("No TClass for '" + typeName + "' found.");
			* Toolbox::logInfo("------------------------------------------------------------");
			* Toolbox::logInfo("");
			*/

			// throw std::invalid_argument("Type '" + typeName + "' is not supported.");
			return nullptr;
		}

		v8::Local<v8::Object> instance;
		if(instancePtr != nullptr)
		{
			instance = *instancePtr;
		} else {
			instance = TemplateFactory::getInstance(type);
		}

		proxy = new ObjectProxy(info, scope);
		proxy->setProxy(instance);
		std::map<std::string, ObjectProxy*>* propertyMap = createPropertyMap(info, type, proxy);

		instance->SetAlignedPointerInInternalField(Toolbox::ObjectProxyPtr, proxy);
		instance->SetAlignedPointerInInternalField(Toolbox::PropertyMapPtr, propertyMap);

		return proxy;
	}

	std::map<std::string, ObjectProxy*>* ObjectProxyFactory::createPropertyMap(MetaInfo &info, TClass *scope, ObjectProxy *holder) throw(std::invalid_argument)
	{
		std::map<std::string, ObjectProxy*> *propertyMap = new std::map<std::string, ObjectProxy*>();

		if(scope == nullptr)
		{
			return nullptr;
		}

		TIter memberIter((TList*)scope->GetListOfAllPublicDataMembers(kTRUE));
		TDataMember *member = nullptr;

		while ( (member = (TDataMember*) memberIter()))
		{
			if (member == nullptr || !member->IsValid())
			{
				continue;
			}

			if(!(member->Property() & kIsStatic))
			{
				if(!holder->getProxy()->Has(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), member->GetName())))
				{
					Toolbox::logError("v8 instance of type '" + std::string(scope->GetName())
					                  + "' has no property '" + std::string(member->GetName())
					                  + "' of type '" + std::string(member->GetTypeName()) + "'.");
				}

				/*
				 * Toolbox::logInfo("Encapsulating '" + std::string(member->GetName()) + "' from '" + std::string(scope->GetName()) + "'.");
				 * Toolbox::logInfo("");
				 *
				 * Toolbox::logInfo("GetTypeName()     '" + std::string(member->GetTypeName()) + "'.");
				 * Toolbox::logInfo("GetFullTypeName() '" + std::string(member->GetFullTypeName()) + "'.");
				 * Toolbox::logInfo("GetTrueTypeName() '" + std::string(member->GetTrueTypeName()) + "'.");
				 *
				 * Toolbox::logInfo("GetDataType()->GetTypeName() '" + std::string(member->GetDataType()->GetTypeName()) + "'.");
				 *
				 * Toolbox::logInfo("------------------------------------------------------------");
				 * Toolbox::logInfo("");
				*/

				MemberInfo memberInfo(*member, info.getAddress());
				// ObjectProxy *memberProxy = ObjectProxyFactory::createObjectProxy(memberInfo, scope);
				(*propertyMap)[std::string(member->GetName())] = nullptr; // = memberProxy;
			}
		}

		return propertyMap;
	}

	ObjectProxy* ObjectProxyFactory::createEnumProxy(MetaInfo &info, TClass* clazz)
	{
		return nullptr;
	}

	ObjectProxy* ObjectProxyFactory::createPrimitiveProxy(MetaInfo &info, TClass* clazz)
	{
		std::string trueType;
		if(!resolveTypeName(info, trueType))
		{
			return nullptr;
		}

		if(primitiveProxyMap.find(trueType) == primitiveProxyMap.end())
		{
			Toolbox::logError("Could not resolve basic type '" + trueType
			                  + "' from '" + std::string(info.getName())
			                  + "' with type '" + std::string(info.getTypeName())
			                  + "' in '" +  ((clazz == nullptr) ? "global" : std::string(clazz->GetName())) + "' scope.");
			return nullptr;
		}
		/* else
		{
			Toolbox::logInfo("Resolved '" + trueType
			                 + "' from '" + std::string(info.getName())
			                 + "' with type '" + std::string(info.getTypeName())
			                 + "' in '" +  ((clazz == nullptr) ? "global" : std::string(clazz->GetName())) + "' scope.");
		   }
		*/

		return primitiveProxyMap[trueType](info, clazz);
	}

	TClass* ObjectProxyFactory::getClass(std::string const& typeName)
	{
		DictFuncPtr_t dictFunc = gClassTable->GetDict(typeName.c_str());
		if(dictFunc == nullptr)
		{
			return nullptr;
		}

		return dictFunc();
	}


	TEnumConstant* ObjectProxyFactory::getEnumConstant(std::string const& typeName)
	{
		// gROOT->GetListOfEnums()->Find();

		return nullptr;
	}

	bool ObjectProxyFactory::resolveTypeName(MetaInfo &info, std::string &trueType)
	{
		std::string stdTypeName(info.getTypeName());
		TDataType* type = Types::getTypeByName(stdTypeName);
		if(type == nullptr)
		{
			return false;
		}

		TString typeName = type->GetTypeName().Data();
		if(typeName.IsNull())
		{
			return false;
		}

		trueType = std::string(typeName.Data());
		return true;
	}

	void ObjectProxyFactory::initializeProxyMap()
	{

		primitiveProxyMap["int"]                = &NumberProxy::intConstruct;
		primitiveProxyMap["unsigned int"]       = &NumberProxy::uintConstruct;

		primitiveProxyMap["double"]             = &NumberProxy::doubleConstruct;
		primitiveProxyMap["long double"]        = &NumberProxy::ldoubleConstruct;

		primitiveProxyMap["short"]              = &NumberProxy::shortConstruct;
		primitiveProxyMap["unsigned short"]     = &NumberProxy::ushortConstruct;

		primitiveProxyMap["long"]               = &NumberProxy::longConstruct;
		primitiveProxyMap["long long"]          = &NumberProxy::llongConstruct;

		primitiveProxyMap["unsigned long"]      = &NumberProxy::ulongConstruct;
		primitiveProxyMap["unsigned long long"] = &NumberProxy::ullongConstruct;

		primitiveProxyMap["float"]              = &NumberProxy::floatConstruct;

		primitiveProxyMap["char"]               = &StringProxy::charConstruct;
		primitiveProxyMap["unsigned char"]      = &StringProxy::charConstruct;
		primitiveProxyMap["char*"]              = &StringProxy::charConstruct;

		primitiveProxyMap["string"]             = &StringProxy::stringConstruct;	// = std::string

		primitiveProxyMap["bool"]               = &BooleanProxy::boolConstruct;

		primitiveProxyMap["void"]               = &VoidPointerProxy::voidConstruct;

		// Special typedefs
		primitiveProxyMap["Double32_t"]         = &NumberProxy::doubleConstruct;
		primitiveProxyMap["Float16_t"]          = &NumberProxy::floatConstruct;
		primitiveProxyMap["Long64_t"]           = &NumberProxy::llongConstruct;
		primitiveProxyMap["ULong64_t"]          = &NumberProxy::ullongConstruct;

	}

}
