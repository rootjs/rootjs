#include "ObjectProxyFactory.h"

#include "Proxy.h"
#include <iostream>
#include <map>
#include <v8.h>
#include <TClass.h>
#include <TClassRef.h>
#include <TClassTable.h>
#include <TDataMember.h>
#include <TDataType.h>
#include <TList.h>

#include "GlobalInfo.h"
#include "MemberInfo.h"

/* Includes for proxyMap */
#include "NumberProxy.h"

namespace rootJS
{

	std::map<std::string, PrimitiveProxyInitializator> ObjectProxyFactory::primitiveProxyMap;

	/*
	std::map<std::string, MemberProxyInitializator> ObjectProxyFactory::memberProxyMap;
	std::map<std::string, GlobalProxyInitializator> ObjectProxyFactory::globalProxyMap;
	*/

	void ObjectProxyFactory::traverseClass(TClass *clazz, ObjectProxy &holder)
	{
		TList *propertyList = clazz->GetListOfAllPublicDataMembers();
		TIter nextProperty(propertyList);
		TDataMember *member;

		while ((member = (TDataMember*) nextProperty()))
		{
			v8::Local<v8::Object> proxy = holder.getProxy();
			ObjectProxy *memberProxy = ObjectProxyFactory::createObjectProxy(holder, new MemberInfo(member), clazz);
			proxy->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), member->GetName()), memberProxy->get());
		}
	}


	ObjectProxy* ObjectProxyFactory::createObjectProxy(void *address, MetaInfo *type, TClass *scope)
	{
		if(address == nullptr || scope == nullptr)
		{
			return nullptr;
		}

		ObjectProxy *proxy = determineProxy(type, scope);
		if(proxy)
		{
			proxy->setAddress(address);
		}
		else
		{
			/*
			 * TODO
			 */

			proxy = new ObjectProxy(address, type, scope);
		}

		return proxy;

	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(ObjectProxy &holder, MetaInfo *type, TClass *scope)
	{
		/*
		 * It is not possible to do pointer arithmetic on void pointers.
		 * To add the offset to the object we cast the pointer to char* before.
		 */
		void *address = static_cast<void*>( (static_cast<char*>(holder.getAddress()) + type->getOffset()));;

		ObjectProxy *proxy = determineProxy(type, scope);
		if(proxy)
		{
			proxy->setAddress(address);
		}
		else
		{
			/*
			 * TODO
			 */
			proxy = new ObjectProxy(address, type, scope);
		}

		return proxy;
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(void *address, MetaInfo *type, TClass *scope, v8::Local<v8::Object> proxy)
	{
		if(proxy->InternalFieldCount() < 1)
		{
			// TODO throw exception
			return nullptr;
		}

		//	std::map<std::string, *ObjectProxy> proxyMap = new std::map<std::string, *ObjectProxy>();
		//	proxy->SetAlignedPointerInInternalField(0, proxyMap);

		return nullptr;
	}

	const char* ObjectProxyFactory::getClassNameFromType(const char *type)
	{
		std::string typeString = std::string(type);
		std::string className = typeString.substr(0, typeString.length()-1);
		return className.c_str();
	}

	ObjectProxy* ObjectProxyFactory::determineProxy(MetaInfo *type, TClass *scope)
	{
		std::string typeString = std::string(type->getTypeName());
		if(primitiveProxyMap.find(typeString) == primitiveProxyMap.end())
		{
			return nullptr;
		}

		return primitiveProxyMap[typeString](type, scope);
	}

	void ObjectProxyFactory::initializeProxyMap()
	{
		//Int_t
		primitiveProxyMap["Int_t"] = &NumberProxy::intConstruct;

		//Double_t
		primitiveProxyMap["Double_t"] = &NumberProxy::doubleConstruct;

		/*
		//Int_t
		memberProxyMap["Int_t"] = &NumberProxy::intConstruct;
		globalProxyMap["Int_t"] = &NumberProxy::intConstruct;

		//Double_t
		memberProxyMap["Double_t"] = &NumberProxy::doubleConstruct;
		globalProxyMap["Double_t"] = &NumberProxy::doubleConstruct;
		*/
	}

}
