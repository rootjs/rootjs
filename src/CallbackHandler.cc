#include "CallbackHandler.h"

#include "Types.h"
#include "Toolbox.h"

#include <Rtypes.h>
#include <TClass.h>
#include <TClassTable.h>

#include <vector>
#include <TROOT.h>

namespace rootJS
{

	const std::string CallbackHandler::CALLBACK_DATA_DELIMITER = "#";

	std::map<std::string, ObjectProxy*> CallbackHandler::globalObjectMap;
	std::map<std::string, ObjectProxy*> CallbackHandler::staticObjectMap;

	void CallbackHandler::registerGlobalObject(const std::string &name, ObjectProxy* proxy)
	{
		globalObjectMap[name] = proxy;
	}

	void CallbackHandler::globalGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
	{
		ObjectProxy* proxy = globalObjectMap[std::string(*v8::String::Utf8Value(property->ToString()))];
		if(proxy) {
			info.GetReturnValue().Set(proxy->get());
		}
	}

	void CallbackHandler::globalSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{
		ObjectProxy* proxy = globalObjectMap[std::string(*v8::String::Utf8Value(property->ToString()))];
		if(proxy) {
			proxy->setValue(value);
		}
	}

	void CallbackHandler::globalFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();

		std::string name;			// function name
		TClass *scope = nullptr;	// function scope

		try {
			name  = resolveCallbackName(info.Data());
			scope = resolveCallbackScope(info.Data(), true);
		} catch(const std::invalid_argument& e) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException(e.what());
			return;
		}

		v8::Local<v8::Function> callback;
		v8::Local<v8::Array> args = getInfoArgs(&callback, info);

		FunctionProxy* proxy = FunctionProxyFactory::fromArgs(name, scope, args);
		if(proxy == nullptr) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException("No suitable global method named '" + name + "' found for the supplied arguments.");
			return;
		}

		if(callback.IsEmpty()) {
			proxy->prepareCall(args);
			ObjectProxy *resultProxy = proxy->call(nullptr);
			if(resultProxy) {
				if(Types::isV8Primitive(resultProxy->get()) || resultProxy->isPrimitive()) {
					info.GetReturnValue().Set(resultProxy->get());
					delete resultProxy;
				} else {
					info.GetReturnValue().Set(resultProxy->get());
				}
			}
			delete proxy;
		} else {
			AsyncCallParam *asyncCallParam = new AsyncCallParam();
			v8::Persistent<v8::Array, v8::CopyablePersistentTraits<v8::Array>> persistentArgs(v8::Isolate::GetCurrent(), args);
			v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> persistentCallback(v8::Isolate::GetCurrent(), callback);

			/*
			 * Instead of member calls: we do not need to clone the proxy here because
			 * it is being created in this callback (not during initialization)
			 */
			asyncCallParam->params = persistentArgs;
			asyncCallParam->proxy = proxy;
			asyncCallParam->selfAddress = nullptr;
			proxy->prepareCall(args);
			AsyncRunner *runner = new AsyncRunner(&asyncMemberCall, asyncCallParam, persistentCallback);
			runner->run();
		}
	}


	v8::Local<v8::Value> CallbackHandler::registerStaticObject(const std::string &name, TClass *scope, ObjectProxy* proxy)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		if(scope == nullptr || !scope->IsLoaded()) {
			staticObjectMap[name] = proxy;
			return handle_scope.Escape(v8::String::NewFromUtf8(isolate, name.c_str()));
		} else {
			staticObjectMap[std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + name)] = proxy;
			return handle_scope.Escape(v8::String::NewFromUtf8(isolate, std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + name).c_str()));
		}
	}

	void CallbackHandler::staticGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
	{
		std::string propertyName(Toolbox::Stringv8toStd(info.Data()->ToString()));

		if(staticObjectMap.find(propertyName) == staticObjectMap.end()) {
			info.GetReturnValue().Set(v8::Undefined(v8::Isolate::GetCurrent()));
			Toolbox::throwException("Property '" + propertyName + "' not found.");
			return;
		}

		info.GetReturnValue().Set(staticObjectMap[propertyName]->get());
	}

	void CallbackHandler::staticSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{}

	void CallbackHandler::staticFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();

		std::string name;			// function name
		TClass *scope = nullptr;	// function scope

		try {
			name  = resolveCallbackName(info.Data());
			scope = resolveCallbackScope(info.Data(), false);
		} catch(const std::invalid_argument& e) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException(e.what());
			return;
		}

		v8::Local<v8::Function> callback;
		v8::Local<v8::Array> args = getInfoArgs(&callback, info);

		FunctionProxy* proxy = FunctionProxyFactory::fromArgs(name, scope, args);
		if(proxy == nullptr)
		{
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException("No suitable method named '" + name + "' found for the supplied arguments in '" + std::string(scope->GetName()) + "'.");
			return;
		}


		if(callback.IsEmpty())
		{
			proxy->prepareCall(args);
			ObjectProxy *resultProxy = proxy->call(nullptr);
			delete proxy;
			if(Types::isV8Primitive(resultProxy->get()) || resultProxy->isPrimitive()) {
				info.GetReturnValue().Set(resultProxy->get());
				delete resultProxy;
			} else {
				info.GetReturnValue().Set(resultProxy->get());
			}
		} else {
			AsyncCallParam *asyncCallParam = new AsyncCallParam();
			v8::Persistent<v8::Array, v8::CopyablePersistentTraits<v8::Array>> persistentArgs(v8::Isolate::GetCurrent(), args);
			v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> persistentCallback(v8::Isolate::GetCurrent(), callback);

			/*
			 * Clone the FunctionProxy because when the same function is called multiple times
			 * prepareCall might be called with new data before the old call finished,
			 * which leads to unexpected results (while testing: multiple frees of the param buffer)
			 */
			FunctionProxy* cloneProxy = proxy->clone();

			asyncCallParam->params = persistentArgs;
			asyncCallParam->proxy = cloneProxy;
			asyncCallParam->selfAddress = nullptr;
			cloneProxy->prepareCall(args);
			AsyncRunner *runner = new AsyncRunner(&asyncMemberCall, asyncCallParam, persistentCallback);
			runner->run();
		}
	}


	void CallbackHandler::ctorCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Isolate* isolate = info.GetIsolate();
		v8::Local<v8::Object> instance = info.This();

		if (!info.IsConstructCall()) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException("Can not call this constructor as plain function. Use the new operator.");
			return;
		}

		if(instance->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException("Unexpected internal field count.");
			return;
		}

		std::string name;
		TClass *clazz = nullptr;
		try {
			name  = resolveCallbackName(info.Data());
			clazz = resolveCallbackScope(info.Data(), false);
		} catch(const std::invalid_argument& e) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException(e.what());
			return;
		}

		v8::Local<v8::Function> callback;
		v8::Local<v8::Array> args = getInfoArgs(&callback, info);

		if(callback.IsEmpty()) {	// create object on current thread
			FunctionProxy *funcProxy = FunctionProxyFactory::fromArgs(name, clazz, args);

			if(funcProxy == nullptr) {
				info.GetReturnValue().Set(v8::Undefined(isolate));
				Toolbox::throwException("No suitable constructor found for the supplied arguments. Could not create a new '" + std::string(clazz->GetName()) + "'.");
				return;
			}

			funcProxy->prepareCall(args);
			ObjectProxy *proxy = funcProxy->call(nullptr, true, &instance);
			delete funcProxy;

			if(proxy == nullptr) {
				info.GetReturnValue().Set(v8::Undefined(isolate));
				Toolbox::throwException("Constructor call failed. Could not create a new '" + std::string(clazz->GetName()) + "'.");
				return;
			}

			info.GetReturnValue().Set(proxy->getWeakPeristent());
			return;
		} else {
			Toolbox::throwException("Constructors can't be called async.");
			return;
		}
	}

	void CallbackHandler::memberGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::Local<v8::Object> instance = info.This();

		if(instance->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException("Unexpected internal field count.");
			return;
		}

		std::map<std::string, ObjectProxy*> *propertyMap = (std::map<std::string, ObjectProxy*>*) instance->GetAlignedPointerFromInternalField(Toolbox::PropertyMapPtr);
		std::string propertyName(Toolbox::Stringv8toStd(property));

		if(propertyMap->find(propertyName) == propertyMap->end()) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException("Property '" + propertyName + "' not found.");
			return;
		}

		info.GetReturnValue().Set((*propertyMap)[propertyName]->get());
	}

	void CallbackHandler::memberSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{}

	void CallbackHandler::memberFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::Local<v8::Object> instance = info.This();

		if(instance->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException("Unexpected internal field count.");
			return;
		}

		std::string name;
		TClass *scope = nullptr;
		try {
			name  = resolveCallbackName(info.Data());
			scope = resolveCallbackScope(info.Data(), false);
		} catch(const std::invalid_argument& e) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException(e.what());
			return;
		}

		ObjectProxy* holder = (ObjectProxy*) instance->GetAlignedPointerFromInternalField(Toolbox::ObjectProxyPtr);
		if(holder == nullptr) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException("Could not resolve enclosing '" + std::string(scope->GetName()) + "' instance to call '" + name + "' on.");
			return;
		}

		v8::Local<v8::Function> callback;
		v8::Local<v8::Array> args = getInfoArgs(&callback, info);

		FunctionProxy* proxy = FunctionProxyFactory::fromArgs(name, scope, args);
		if(proxy == nullptr) {
			info.GetReturnValue().Set(v8::Undefined(isolate));
			Toolbox::throwException("No suitable method named '" + name + "' found for the supplied arguments in '" + std::string(scope->GetName()) + "'.");
			return;
		}

		if(callback.IsEmpty()) {
			proxy->prepareCall(args);
			ObjectProxy *resultProxy = proxy->call(*(void**)holder->getAddress());
			delete proxy;
			if(resultProxy) {
				if(Types::isV8Primitive(resultProxy->get()) || resultProxy->isPrimitive()) {
					info.GetReturnValue().Set(resultProxy->get());
					delete resultProxy;
				} else {
					info.GetReturnValue().Set(resultProxy->get());
				}
			}
		} else {
			AsyncCallParam *asyncCallParam = new AsyncCallParam();
			v8::Persistent<v8::Array, v8::CopyablePersistentTraits<v8::Array>> persistentArgs(v8::Isolate::GetCurrent(), args);
			v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> persistentCallback(v8::Isolate::GetCurrent(), callback);

			/*
			 * Clone the FunctionProxy because when the same function is called multiple times
			 * prepareCall might be called with new data before the old call finished,
			 * which leads to unexpected results (while testing: multiple frees of the param buffer)
			 */
			FunctionProxy* cloneProxy = proxy->clone();

			asyncCallParam->params = persistentArgs;
			asyncCallParam->proxy = cloneProxy;
			asyncCallParam->selfAddress = *(void**)holder->getAddress();
			cloneProxy->prepareCall(args);
			AsyncRunner *runner = new AsyncRunner(&asyncMemberCall, asyncCallParam, persistentCallback);
			runner->run();
		}
	}

	void CallbackHandler::asyncMemberCall(AsyncRunner *runner, void *param)
	{
		AsyncCallParam *asyncCallParam = (AsyncCallParam*)param;

		std::vector<ObjectProxy*> resultVector;
		ObjectProxy* resultProxy = asyncCallParam->proxy->call(asyncCallParam->selfAddress);

		resultVector.push_back(resultProxy);
		runner->setResult(resultVector);

		delete asyncCallParam->proxy;
		delete asyncCallParam;
	}


	v8::Local<v8::Value> CallbackHandler::createFunctionCallbackData(std::string functionName, TClass *scope)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		if(scope != nullptr && scope->IsLoaded()) {
			return handle_scope.Escape(v8::String::NewFromUtf8(isolate, std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + functionName).c_str()));
		} else {
			return handle_scope.Escape(v8::String::NewFromUtf8(isolate, functionName.c_str()));
		}
	}

	v8::Local<v8::Value> CallbackHandler::createFunctionCallbackData(TClass *scope)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		if(scope == nullptr || !scope->IsLoaded()) {
			return handle_scope.Escape(v8::Undefined(isolate));
		}

		std::string className(scope->GetName());
		std::size_t idx = className.find_last_of("::");

		if(idx != std::string::npos) {
			className = className.substr(idx+1);
		}

		return handle_scope.Escape(v8::String::NewFromUtf8(isolate, std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + className).c_str()));
	}

	TClass* CallbackHandler::resolveCallbackScope(v8::Local<v8::Value> data, bool allowNull) throw(std::invalid_argument)
	{
		std::string scopeName = toString(data);
		std::size_t idx = scopeName.find_last_of(CALLBACK_DATA_DELIMITER);

		DictFuncPtr_t dictPtr = nullptr;
		if(idx != std::string::npos) {
			scopeName = scopeName.substr(0, idx);
			dictPtr = gClassTable->GetDict(scopeName.c_str());
		} else if (allowNull) { // global found
			return nullptr;
		} else {
			throw std::invalid_argument(std::string("No Delimiter in '" + scopeName + "' was found."));
		}

		if(dictPtr == nullptr) {
			throw std::invalid_argument(std::string("No scope named '" + scopeName + "' was found."));
		}

		TClass *scope = dictPtr();

		if(!allowNull) {
			if(scope == nullptr) {
				throw std::invalid_argument(std::string("The scope named '" + scopeName + "' is null."));
			} else if(!scope->IsLoaded()) {
				throw std::invalid_argument(std::string("The scope named '" + scopeName + "' is not loaded."));
			}
		}

		return scope;
	}

	std::string CallbackHandler::resolveCallbackName(v8::Local<v8::Value> data) throw(std::invalid_argument)
	{
		std::string functionName = toString(data);
		std::size_t idx = functionName.find_last_of(CALLBACK_DATA_DELIMITER);

		if(idx != std::string::npos) {
			functionName = functionName.substr(idx+1);
		}

		return functionName;
	}

	std::string CallbackHandler::toString(v8::Local<v8::Value> data) throw(std::invalid_argument)
	{
		return std::string(*v8::String::Utf8Value(data->ToString()));
	}

	v8::Local<v8::Array> CallbackHandler::getInfoArgs(v8::Local<v8::Function> *callback, v8::FunctionCallbackInfo<v8::Value> const& info)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		if(info.Length() < 1) {
			return handle_scope.Escape(v8::Array::New(isolate, 0));
		}

		int endIndex = info.Length() - 1;
		if(info[endIndex]->IsFunction()) {
			*callback = v8::Local<v8::Function>::Cast(info[endIndex]);
			endIndex--;
		}

		v8::Local<v8::Array> args = v8::Array::New(isolate, endIndex + 1);

		for(int i = 0; i <= endIndex; i++) {
			args->Set(i, info[i]);
		}

		return handle_scope.Escape(args);
	}
}
