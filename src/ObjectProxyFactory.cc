#include "ObjectProxyFactory.h"

#include "Proxy.h"
#include "NumberProxy.h"
#include "StringProxy.h"
#include "BooleanProxy.h"

#include "MemberInfo.h"
#include "GlobalInfo.h"

#include "TemplateFactory.h"
#include "CallbackHandler.h"

#include <TClassTable.h>
#include <TDataMember.h>
#include <TDataType.h>
#include <TList.h>
#include <TMethod.h>

namespace rootJS {

	std::map<std::string, ProxyInitializator> ObjectProxyFactory::proxyMap;

	std::map<std::string, Proxy*> *ObjectProxyFactory::createObjectProxyVector(MetaInfo &info, TClass *clazz) {
		std::map<std::string, Proxy*> *result = new std::map<std::string, Proxy*>();

		if(clazz == nullptr) {
			return nullptr;
		}

		const TList *methodList = clazz->GetListOfAllPublicMethods();
		TIter nextMethod(methodList);
		TMethod *method;

		while ((method = (TMethod*)nextMethod())) {
			FunctionProxy *proxy = FunctionProxyFactory::createFunctionProxy(method, clazz);
			(*result)[std::string(method->GetName())] = proxy;
			proxy->setSelfAddress(info.getAddress());
		}

		return result;
	}

	/*void ObjectProxyFactory::traverseClass(TClassRef & classRef, ObjectProxy & proxy) {
		TClass *klass = classRef.GetClass();

		TList *propertyList = klass->GetListOfAllPublicDataMembers();
		TIter nextProperty(propertyList);
		TDataMember *member;

		while ((member = (TDataMember*)nextProperty())) {
			v8::Local<v8::Object> nodeObject = proxy.getProxy();
			ObjectProxy *memberProxy = ObjectProxyFactory::createObjectProxy(*member, classRef, proxy);
			if(memberProxy != nullptr) {
				nodeObject->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), member->GetName()), memberProxy->get());
			}
		}

		const TList *methodList = klass->GetListOfAllPublicMethods();
		TIter nextMethod(methodList);
		TMethod *method;

		while ((method = (TMethod*)nextMethod())) {
			v8::Local<v8::Object> nodeObject = proxy.getProxy();
			NODE_SET_METHOD(nodeObject, method->GetName(), CallbackHandler::memberFunctionCallback);
		}
	}*/

	ObjectProxy* ObjectProxyFactory::createObjectProxy(MetaInfo &info, TClass *scope) {
		ObjectProxy* nonObjectProxy = determineProxy(info, scope);

		if(nonObjectProxy) {
			return nonObjectProxy;
		}


		std::string className = info.getTypeName();
		DictFuncPtr_t dictFunc = gClassTable->GetDict(className.c_str());
		if(dictFunc == nullptr)
		{
			return nullptr;
		}
		TClass *clazz = dictFunc();

		v8::Local<v8::Object> instance = TemplateFactory::getInstance(clazz);
		if(instance.IsEmpty()) {
			return nullptr;
		}
		instance->SetAlignedPointerInInternalField(0, createObjectProxyVector(info, clazz));

		ObjectProxy* proxy = new ObjectProxy(info, scope);
		proxy->setProxy(instance);

		return proxy;
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(TGlobal &global) {
		if(!global.IsValid() || !global.GetAddress()) {
			return nullptr;
		}

		GlobalInfo gMode(global);
		return createObjectProxy(gMode, nullptr);
	}

	ObjectProxy* ObjectProxyFactory::createObjectProxy(TDataMember const& type, TClass *scope, ObjectProxy &holder)
	{
		/*
		 * It is not possible to do pointer arithmetic on void pointers.
		 * To add the offset to the object we cast the pointer to char* before.
		 */
		void *object = static_cast<void*>(
		                   (static_cast<char*>(holder.getAddress()) + type.GetOffsetCint())
		               );

		MemberInfo mode(type, object);

		return createObjectProxy(mode, scope);
	}


	ObjectProxy* ObjectProxyFactory::createObjectProxy(void *address, TClass *type, v8::Local<v8::Object> proxy)
	{
		// TODO
		return nullptr;
	}

	ObjectProxy* ObjectProxyFactory::determineProxy(MetaInfo &info, TClass* clazz)
	{
		std::string typeString = std::string(info.getTypeName());
		if(proxyMap.find(typeString) == proxyMap.end())
		{
			return nullptr;
		}

		return proxyMap[typeString](info, clazz);
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
		proxyMap["const char*"] = &StringProxy::charConstruct;
		proxyMap["std::string"] = &StringProxy::stringConstruct;
		proxyMap["TString"] = &StringProxy::tStringConstruct;

		proxyMap["Text_t"] = &StringProxy::charConstruct;
		proxyMap["Option_t"] = &StringProxy::charConstruct;

		proxyMap["Bool_t"] = &BooleanProxy::boolConstruct;
		proxyMap["bool"] = &BooleanProxy::boolConstruct;
	}

}
