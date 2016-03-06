#include "ObjectProxyFactory.h"

#include "Proxy.h"
#include "NumberProxy.h"
#include "StringProxy.h"
#include "BooleanProxy.h"
#include "EnumConstProxy.h"
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
	std::map<void*, ObjectProxy*> ObjectProxyFactory::propertyCash;

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
				ObjectProxy *memberProxy = ObjectProxyFactory::createObjectProxy(memberInfo, scope, nullptr, true);
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
		return createObjectProxy(info, scope, nullptr, false);
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(MetaInfo &info, TClass *scope, v8::Local<v8::Object> instance) throw(std::invalid_argument)
	{
		return createObjectProxy(info, scope, &instance, false);
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(MetaInfo &info, TClass *scope,  v8::Local<v8::Object>* instancePtr, bool recursiveCall) throw(std::invalid_argument)
	{
		if(recursiveCall)
		{
			// check for cyclic references
			if(info.getAddress() != nullptr && propertyCash[info.getAddress()] != nullptr)
			{
				Toolbox::logInfo("Cyclic reference detected.", 1);
				return propertyCash[info.getAddress()];
			}
		}
		else
		{
			// Clear cash for cyclic reference handling
			propertyCash.clear();
			if(!propertyCash.empty())
			{
				throw std::invalid_argument("Could not clear cash for cyclic reference handling.");
			}
		}

		std::string trueTypeName = info.getFullTypeName();

		//Try without resolving the type first:
		ObjectProxy* proxy = createPrimitiveProxy(trueTypeName, info, scope);
		if(proxy != nullptr)
		{
			return proxy;
		}

		Types::resolveTypeName(info, trueTypeName);	// resolve typedefs

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
			Toolbox::logInfo(std::string("Resolved Type '" + trueTypeName
			                             + "' from '" + std::string(info.getName())
			                             + "' with type '" + std::string(info.getTypeName())
			                             + "' in scope '" +  ((scope == nullptr) ? "global" : std::string(scope->GetName())) + "' could not be encapsulated."), 1);
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

		propertyCash[proxy->getAddress()] = proxy; // add to cash for cyclic reference detection

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
			return new EnumConstProxy(info, scope);
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

	const std::map<std::string, ProxyInitializator> ObjectProxyFactory::primitiveProxyMap
	{

	    {"int",                &NumberProxy::intConstruct
	    },
	    {"unsigned int",       &NumberProxy::uintConstruct},

	    {"double",             &NumberProxy::doubleConstruct},
	    {"long double",        &NumberProxy::ldoubleConstruct},

	    {"short",              &NumberProxy::shortConstruct},
	    {"unsigned short",     &NumberProxy::ushortConstruct},

	    {"unsigned char",      &NumberProxy::ucharConstruct},

	    {"long",               &NumberProxy::longConstruct},
	    {"long long",          &NumberProxy::llongConstruct},

	    {"unsigned long",      &NumberProxy::ulongConstruct},
	    {"unsigned long long", &NumberProxy::ullongConstruct},

	    {"float",              &NumberProxy::floatConstruct},

	    {"char",               &StringProxy::singleCharConstruct},
	    {"char*",              &StringProxy::charConstruct},
	    {"const char*",        &StringProxy::charConstruct},
	    {"char&",              &StringProxy::singleCharConstruct},

	    {"string",             &StringProxy::stringConstruct},    // = std::string

	    {"bool",               &BooleanProxy::boolConstruct},

	    {"void",               &VoidPointerProxy::voidConstruct},

	    // Special typedefs
	    {"Double32_t",         &NumberProxy::doubleConstruct},
	    {"Float16_t",          &NumberProxy::floatConstruct},
	    {"Long64_t",           &NumberProxy::llongConstruct},
	    {"ULong64_t",          &NumberProxy::ullongConstruct}

	};

}
