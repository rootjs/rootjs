#include "ObjectProxyFactory.h"

#include "Proxy.h"
#include "NumberProxy.h"
#include "StringProxy.h"
#include "BooleanProxy.h"

#include "Toolbox.h"

#include "MemberInfo.h"
#include "GlobalInfo.h"
#include "PointerInfo.h"

#include "TemplateFactory.h"
#include "CallbackHandler.h"

#include <TClassTable.h>
#include <TDataMember.h>
#include <TDataType.h>
#include <TList.h>
#include <TMethod.h>

namespace rootJS
{

	std::map<std::string, ProxyInitializator> ObjectProxyFactory::proxyMap;

	std::map<std::string, ObjectProxy*>* ObjectProxyFactory::createPropertyMap(MetaInfo &info, TClass *scope) throw(std::invalid_argument)
	{
		std::map<std::string, ObjectProxy*> *propertyMap = new std::map<std::string, ObjectProxy*>();

		if(scope == nullptr)
		{
			return nullptr;
		}

		/*
		const TList *methodList = clazz->GetListOfAllPublicMethods();
		TIter nextMethod(methodList);
		TMethod *method;

		while ((method = (TMethod*)nextMethod()))
		{
			FunctionProxy *proxy = FunctionProxyFactory::createFunctionProxy(method, clazz);
			(*result)[std::string(method->GetName())] = proxy;
			proxy->setSelfAddress(info.getAddress());
	}
		*/

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
				MemberInfo memberInfo(*member, info.getAddress());
				// ObjectProxy *memberProxy = ObjectProxyFactory::createObjectProxy(memberInfo, scope);
				(*propertyMap)[std::string(member->GetName())] = nullptr;
			}
		}

		return propertyMap;
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(MetaInfo &info, TClass *scope) throw(std::invalid_argument)
	{
		ObjectProxy* proxy = createPrimitiveProxy(info, scope);

		if(proxy != nullptr)
		{
			return proxy;
		}

		std::string typeName(info.getTypeName());
		TClass *type = getClass(typeName);
		if(type == nullptr)
		{
			return nullptr;
			// throw std::invalid_argument("Type '" + typeName + "' is not supported.");
		}

		v8::Local<v8::Object> instance = TemplateFactory::getInstance(type);
		proxy = new ObjectProxy(info, scope);
		proxy->setProxy(instance);

		std::map<std::string, ObjectProxy*>* propertyMap = createPropertyMap(info, type);

		instance->SetAlignedPointerInInternalField(Toolbox::ObjectProxyPtr, proxy);
		instance->SetAlignedPointerInInternalField(Toolbox::PropertyMapPtr, propertyMap);

		return proxy;
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(TGlobal &global)
	{
		if(!global.IsValid() || !global.GetAddress())
		{
			return nullptr;
		}

		GlobalInfo info(global);
		return createObjectProxy(info, nullptr);
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(TDataMember const& type, TClass *scope, ObjectProxy &holder)
	{
		/*
		 * It is not possible to do pointer arithmetic on void pointers.
		 * To add the offset to the object we cast the pointer to char* before.
		 */
		void *object = static_cast<void*>((static_cast<char*>(holder.getAddress()) + type.GetOffsetCint()));

		MemberInfo info(type, object);
		return createObjectProxy(info, scope);
	}


	ObjectProxy* ObjectProxyFactory::createObjectProxy(void *address, TClass *type, v8::Local<v8::Object> proxy)
	{
		// TODO: populate proxy
		PointerInfo info(address, type->GetName());
		return createObjectProxy(info, type);
	}

	ObjectProxy* ObjectProxyFactory::createPrimitiveProxy(MetaInfo &info, TClass* clazz)
	{
		std::string typeString = std::string(info.getTypeName());
		if(proxyMap.find(typeString) == proxyMap.end())
		{
			return nullptr;
		}

		return proxyMap[typeString](info, clazz);
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

	void ObjectProxyFactory::initializeProxyMap()
	{
		proxyMap["Int_t"]              = &NumberProxy::intConstruct;
		proxyMap["int"]                = &NumberProxy::intConstruct;
		proxyMap["Seek_t"]             = &NumberProxy::intConstruct;
		proxyMap["Ssiz_t"]             = &NumberProxy::intConstruct;

		proxyMap["UInt_t"]             = &NumberProxy::uintConstruct;
		proxyMap["unsigned int"]       = &NumberProxy::uintConstruct;

		proxyMap["double"]             = &NumberProxy::doubleConstruct;
		proxyMap["Axis_t"]             = &NumberProxy::doubleConstruct;
		proxyMap["Double_t"]           = &NumberProxy::doubleConstruct;
		proxyMap["Stat_t"]             = &NumberProxy::doubleConstruct;
		proxyMap["Coord_t"]            = &NumberProxy::doubleConstruct;

		proxyMap["long double"]        = &NumberProxy::ldoubleConstruct;
		proxyMap["LongDouble_t"]       = &NumberProxy::ldoubleConstruct;

		proxyMap["Short_t"]            = &NumberProxy::shortConstruct;
		proxyMap["short"]              = &NumberProxy::shortConstruct;
		proxyMap["Version_t"]          = &NumberProxy::shortConstruct;
		proxyMap["Font_t"]             = &NumberProxy::shortConstruct;
		proxyMap["Style_t"]            = &NumberProxy::shortConstruct;
		proxyMap["Marker_t"]           = &NumberProxy::shortConstruct;
		proxyMap["Width_t"]            = &NumberProxy::shortConstruct;
		proxyMap["Color_t"]            = &NumberProxy::shortConstruct;
		proxyMap["SCoord_t"]           = &NumberProxy::shortConstruct;

		proxyMap["UShort_t"]           = &NumberProxy::ushortConstruct;
		proxyMap["unsigned short"]     = &NumberProxy::ushortConstruct;

		proxyMap["Long_t"]             = &NumberProxy::longConstruct;
		proxyMap["long"]               = &NumberProxy::longConstruct;

		proxyMap["Long64_t"]           = &NumberProxy::llongConstruct;
		proxyMap["long long"]          = &NumberProxy::llongConstruct;
		proxyMap["__int64"]            = &NumberProxy::llongConstruct;

		proxyMap["ULong_t"]            = &NumberProxy::ulongConstruct;
		proxyMap["unsigned long"]      = &NumberProxy::ulongConstruct;
		proxyMap["ULong64_t"]          = &NumberProxy::ullongConstruct;
		proxyMap["unsigned long long"] = &NumberProxy::ullongConstruct;
		proxyMap["unsigned __int64"]   = &NumberProxy::ullongConstruct;

		proxyMap["Float_t"]            = &NumberProxy::floatConstruct;
		proxyMap["float"]              = &NumberProxy::floatConstruct;
		proxyMap["Real_t"]             = &NumberProxy::floatConstruct;
		proxyMap["Angle_t"]            = &NumberProxy::floatConstruct;
		proxyMap["Size_t"]             = &NumberProxy::floatConstruct;

		proxyMap["char"]               = &StringProxy::charConstruct;
		proxyMap["char*"]              = &StringProxy::charConstruct;
		proxyMap["const char*"]        = &StringProxy::charConstruct;
		proxyMap["Text_t"]             = &StringProxy::charConstruct;
		proxyMap["Option_t"]           = &StringProxy::charConstruct;

		proxyMap["std::string"]        = &StringProxy::stringConstruct;

		// proxyMap["TString"]            = &StringProxy::tStringConstruct;

		proxyMap["Bool_t"]             = &BooleanProxy::boolConstruct;
		proxyMap["bool"]               = &BooleanProxy::boolConstruct;
	}

}
