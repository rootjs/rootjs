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
#include "StringProxy.h"
#include "BooleanProxy.h"

namespace rootJS
{

	std::map<std::string, ProxyInitializator> ObjectProxyFactory::proxyMap;

	void ObjectProxyFactory::traverseClass(TClassRef & classRef, ObjectProxy & proxy)
	{
		TClass *klass = classRef.GetClass();

		TList *propertyList = klass->GetListOfAllPublicDataMembers();
		TIter nextProperty(propertyList);
		TDataMember *member;

		while ((member = (TDataMember*)nextProperty()))
		{
			v8::Local<v8::Object> nodeObject = proxy.getProxy();
			ObjectProxy *memberProxy = ObjectProxyFactory::createObjectProxy(*member, classRef, proxy);
			nodeObject->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), member->GetName()), memberProxy->get());
		}
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(TGlobal & object)
	{
		if(!object.IsValid() || !object.GetAddress())
		{
			return nullptr;
		}
		GlobalInfo gMode(object);
		ObjectProxy* nonObjectProxy = determineProxy(gMode, TClassRef());

		if(nonObjectProxy)
		{
			return nonObjectProxy;
		}


		std::string className = getClassNameFromType(object.GetTypeName());
		DictFuncPtr_t dictFunc = gClassTable->GetDict(className.c_str());
		if(dictFunc == nullptr)
		{
			return nullptr;
		}
		TClass *klass = dictFunc();

		TClassRef classRef = TClassRef(klass);
		GlobalInfo mode(object);
		ObjectProxy *proxy = new ObjectProxy(mode, classRef);
		//Set an empty proxy and fill iit in the following loops
		proxy->setProxy(v8::Object::New(v8::Isolate::GetCurrent()));

		traverseClass(classRef, *proxy);

		return proxy;
	}

	std::string ObjectProxyFactory::getClassNameFromType(const char* type)
	{
		std::string typeString = std::string(type);
		std::string className = typeString.substr(0, typeString.length()-1);
		return className;
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(const TDataMember & type, TClassRef scope, ObjectProxy & holder)
	{
		/*
		 * It is not possible to do pointer arithmetic on void pointers.
		 * To add the offset to the object we cast the pointer to char* before.
		 */
		void *object = static_cast<void*>(
		                   (static_cast<char*>(holder.getAddress()) + type.GetOffsetCint())
		               );

		MemberInfo mode(type, object);

		ObjectProxy *memberProxy = determineProxy(mode, scope);
		if(memberProxy)
		{
			memberProxy->setAddress(object);
		}
		else
		{
			//TODO object?
			MemberInfo mode(type, object);
			memberProxy = new ObjectProxy(mode, scope);
			memberProxy->setProxy(v8::Object::New(v8::Isolate::GetCurrent()));
		}

		return memberProxy;
	}


	ObjectProxy* ObjectProxyFactory::createObjectProxy(MetaInfo &info, TClass *scope)
	{
		// TODO
		return nullptr;
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(void *address, TClass *type, v8::Local<v8::Object> proxy)
	{
		// TODO
		return nullptr;
	}

	ObjectProxy* ObjectProxyFactory::determineProxy(MetaInfo& type, TClassRef ref)
	{
		std::string typeString = std::string(type.getTypeName());
		if(proxyMap.find(typeString) == proxyMap.end())
		{
			return nullptr;
		}

		return proxyMap[typeString](type, ref);
	}

	void ObjectProxyFactory::initializeProxyMap()
	{
		proxyMap["Int_t"] = &NumberProxy::intConstruct;
		proxyMap["UInt_t"] = &NumberProxy::uintConstruct;
		proxyMap["int"] = &NumberProxy::intConstruct;
		proxyMap["unsigned int"] = &NumberProxy::uintConstruct;

		proxyMap["Seek_t"] = &NumberProxy::intConstruct;
		proxyMap["Ssiz_t"] = &NumberProxy::intConstruct;

		proxyMap["Double_t"] = &NumberProxy::doubleConstruct;
		proxyMap["LongDouble_t"] = &NumberProxy::ldoubleConstruct;
		proxyMap["double"] = &NumberProxy::doubleConstruct;
		proxyMap["long double"] = &NumberProxy::ldoubleConstruct;

		proxyMap["Axis_t"] = &NumberProxy::doubleConstruct;
		proxyMap["Stat_t"] = &NumberProxy::doubleConstruct;
		proxyMap["Coord_t"] = &NumberProxy::doubleConstruct;

		proxyMap["Short_t"] = &NumberProxy::shortConstruct;
		proxyMap["UShort_t"] = &NumberProxy::ushortConstruct;
		proxyMap["short"] = &NumberProxy::shortConstruct;
		proxyMap["unsigned short"] = &NumberProxy::ushortConstruct;

		proxyMap["Version_t"] = &NumberProxy::shortConstruct;
		proxyMap["Font_t"] = &NumberProxy::shortConstruct;
		proxyMap["Style_t"] = &NumberProxy::shortConstruct;
		proxyMap["Marker_t"] = &NumberProxy::shortConstruct;
		proxyMap["Width_t"] = &NumberProxy::shortConstruct;
		proxyMap["Color_t"] = &NumberProxy::shortConstruct;
		proxyMap["SCoord_t"] = &NumberProxy::shortConstruct;

		proxyMap["Long_t"] = &NumberProxy::longConstruct;
		proxyMap["ULong_t"] = &NumberProxy::ulongConstruct;
		proxyMap["long"] = &NumberProxy::longConstruct;
		proxyMap["unsigned long"] = &NumberProxy::ulongConstruct;

		proxyMap["Long64_t"] = &NumberProxy::llongConstruct;
		proxyMap["ULong64_t"] = &NumberProxy::ullongConstruct;
		proxyMap["long long"] = &NumberProxy::llongConstruct;
		proxyMap["unsigned long long"] = &NumberProxy::ullongConstruct;
		proxyMap["__int64"] = &NumberProxy::llongConstruct;
		proxyMap["unsigned __int64"] = &NumberProxy::ullongConstruct;

		proxyMap["Float_t"] = &NumberProxy::floatConstruct;
		proxyMap["float"] = &NumberProxy::floatConstruct;

		proxyMap["Real_t"] = &NumberProxy::floatConstruct;
		proxyMap["Angle_t"] = &NumberProxy::floatConstruct;

		proxyMap["Size_t"] = &NumberProxy::floatConstruct;
		proxyMap["char"] = &StringProxy::charConstruct;
		proxyMap["char*"] = &StringProxy::charConstruct;
		proxyMap["std::string"] = &StringProxy::stringConstruct;
		proxyMap["TString"] = &StringProxy::tStringConstruct;

		proxyMap["Text_t"] = &StringProxy::charConstruct;
		proxyMap["Option_t"] = &StringProxy::charConstruct;

		proxyMap["Bool_t"] = &BooleanProxy::boolConstruct;
		proxyMap["bool"] = &BooleanProxy::boolConstruct;
	}

}
