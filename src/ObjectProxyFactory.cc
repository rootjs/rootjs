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
					Toolbox::throwException("v8 instance of type '" + std::string(scope->GetName())
					                        + "' has no property '" + std::string(member->GetName())
					                        + "' of type '" + std::string(member->GetTypeName()) + "'.");
					continue;
				}

				MemberInfo memberInfo(*member, info.getAddress());
				ObjectProxy *memberProxy = ObjectProxyFactory::createObjectProxy(memberInfo, scope);
				if(memberProxy == nullptr)
				{
					// Delete properties that could not be encapsulated
					holder->getProxy()->Delete(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), member->GetName()));
				}
				else
				{
					(*propertyMap)[std::string(member->GetName())] = memberProxy;
				}
			}
		}

		return propertyMap;
	}

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
		std::string trueTypeName = info.getFullTypeName();

		//Try without resolving the type first:
		ObjectProxy* proxy = createPrimitiveProxy(trueTypeName, info, scope);
		if(proxy != nullptr)
		{
			return proxy;
		}

		resolveTypeName(info, trueTypeName);	// resolve typedefs

		// Try to encapsulate as primitive
		proxy = createPrimitiveProxy(trueTypeName, info, scope);
		if(proxy != nullptr)
		{
			return proxy;
		}

		//Switch to "normalized" type name
		trueTypeName = info.getTypeName();

		// Try to encapsulate as enum
		proxy = createEnumProxy(trueTypeName, info, scope);
		if(proxy != nullptr)
		{
			return proxy;
		}

		// Try to encapsulate as object / struct / union / array
		TClass *type = getClass(std::string(info.getTypeName()));
		if(type == nullptr)
		{
			Toolbox::logError("Resolved Type '" + trueTypeName
			                  + "' from '" + std::string(info.getName())
			                  + "' with type '" + std::string(info.getTypeName())
			                  + "' in scope '" +  ((scope == nullptr) ? "global" : std::string(scope->GetName())) + "' could not be encapsulated.");
			return nullptr;
		}

		v8::Local<v8::Object> instance;
		if(instancePtr != nullptr)
		{
			instance = *instancePtr;
		}
		else
		{
			instance = TemplateFactory::getInstance(type);
		}

		proxy = new ObjectProxy(info, scope);
		proxy->setProxy(instance);
		std::map<std::string, ObjectProxy*>* propertyMap = createPropertyMap(info, type, proxy);

		instance->SetAlignedPointerInInternalField(Toolbox::ObjectProxyPtr, proxy);
		instance->SetAlignedPointerInInternalField(Toolbox::PropertyMapPtr, propertyMap);

		return proxy;
	}

	ObjectProxy* ObjectProxyFactory::createEnumProxy(std::string const& trueTypeName, MetaInfo &info, TClass *scope)
	{
		TEnum *e = TEnum::GetEnum(trueTypeName.c_str());
		if(e != nullptr)
		{
			return NumberProxy::llongConstruct(info, scope);
		}
		return nullptr;
	}

	ObjectProxy* ObjectProxyFactory::createPrimitiveProxy(std::string const& trueTypeName, MetaInfo &info, TClass *scope)
	{
		std::map<std::string, ProxyInitializator>::const_iterator iter = primitiveProxyMap.find(trueTypeName);
		if(iter == primitiveProxyMap.end())
		{
			return nullptr;
		}

		return iter->second(info, scope);
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

	const std::map<std::string, ProxyInitializator> ObjectProxyFactory::primitiveProxyMap
	{

	    {"int", &NumberProxy::intConstruct},
	    {"unsigned int", &NumberProxy::uintConstruct},

	    {"double", &NumberProxy::doubleConstruct},
	    {"long double", &NumberProxy::ldoubleConstruct},

	    {"short", &NumberProxy::shortConstruct},
	    {"unsigned short", &NumberProxy::ushortConstruct},

	    {"unsigned char", &NumberProxy::ucharConstruct},

	    {"long", &NumberProxy::longConstruct},
	    {"long long", &NumberProxy::llongConstruct},

	    {"unsigned long", &NumberProxy::ulongConstruct},
	    {"unsigned long long", &NumberProxy::ullongConstruct},

	    {"float", &NumberProxy::floatConstruct},

	    {"char", &StringProxy::singleCharConstruct},
	    {"char*", &StringProxy::charConstruct},
	    {"const char*", &StringProxy::charConstruct},
	    {"char&", &StringProxy::singleCharConstruct},

	    {"string", &StringProxy::stringConstruct},	// = std::string

	    {"bool", &BooleanProxy::boolConstruct},

	    {"void", &VoidPointerProxy::voidConstruct},

	    // Special typedefs
	    {"Double32_t", &NumberProxy::doubleConstruct},
	    {"Float16_t", &NumberProxy::floatConstruct},
	    {"Long64_t", &NumberProxy::llongConstruct},
	    {"ULong64_t", &NumberProxy::ullongConstruct}

	};

}
