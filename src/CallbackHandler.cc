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

	void CallbackHandler::globalGetterCallback(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value>& info)
	{
		std::string propertyName = Toolbox::Stringv8toStd(property);

		if(globalObjectMap.find(propertyName) == globalObjectMap.end())
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("Property '" + propertyName + "' not found.");
			return;
		}

		info.GetReturnValue().Set(globalObjectMap[propertyName]->get());
	}

	void CallbackHandler::globalSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void>& info)
	{
		std::string propertyName = Toolbox::Stringv8toStd(property);

		if(globalObjectMap.find(propertyName) == globalObjectMap.end())
		{
			Toolbox::throwException("Property '" + propertyName + "' not found.");
			return;
		}

		if(globalObjectMap[propertyName]->isConst())
		{
			Toolbox::throwException("Property '" + propertyName + "' is a constant. Its value may not be changed.");
			return;
		}

		globalObjectMap[propertyName]->setValue(value);
	}

	void CallbackHandler::globalFunctionCallback(const Nan::FunctionCallbackInfo<v8::Value>& info)
	{
		std::string name;			// function name
		TClass *scope = nullptr;	// function scope

		try
		{
			name  = resolveCallbackName(info.Data());
			scope = resolveCallbackScope(info.Data(), true);
		}
		catch(const std::invalid_argument& e)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException(e.what());
			return;
		}

		v8::Local<v8::Function> callback;
		v8::Local<v8::Array> args = getInfoArgs(&callback, info);

		FunctionProxy* proxy = FunctionProxyFactory::fromArgs(name, scope, args);
		if(proxy == nullptr)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("No suitable global method named '" + name + "' found for the supplied arguments.");
			return;
		}

		if(callback.IsEmpty())
		{
			proxy->prepareCall(args);
			ObjectProxyBuilder builder;
			proxy->call(nullptr, builder);
			if(builder.isValid())
			{
				ObjectProxy *resultProxy = builder.createObjectProxy();
				if(resultProxy)
				{
					if(Types::isV8Primitive(resultProxy->get()) || resultProxy->isPrimitive())
					{
						info.GetReturnValue().Set(resultProxy->get());
						delete resultProxy;
					}
					else
					{
						info.GetReturnValue().Set(resultProxy->get());
					}
				}
			}
			delete proxy;
		}
		else
		{
			AsyncCallParam *asyncCallParam = new AsyncCallParam();
			Nan::Persistent<v8::Array> persistentArgs(args);

			/*
			 * Instead of member calls: we do not need to clone the proxy here because
			 * it is being created in this callback (not during initialization)
			 */
			asyncCallParam->params.Reset(persistentArgs);
			asyncCallParam->proxy = proxy;
			asyncCallParam->selfAddress = nullptr;
			proxy->prepareCall(args);
			AsyncRunner *runner = new AsyncRunner(&asyncMemberCall, asyncCallParam, callback);
			runner->run();
		}
	}


	v8::Local<v8::Value> CallbackHandler::registerStaticObject(const std::string &name, TClass *scope, ObjectProxy* proxy)
	{
		Nan::EscapableHandleScope handle_scope;

		if(scope == nullptr || !scope->IsLoaded())
		{
			staticObjectMap[name] = proxy;
			return handle_scope.Escape(Nan::New(name).ToLocalChecked());
		}
		else
		{
			staticObjectMap[std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + name)] = proxy;
			return handle_scope.Escape(Nan::New(std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + name)).ToLocalChecked());
		}
	}

	void CallbackHandler::staticGetterCallback(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value>& info) {
		std::string propertyName(Toolbox::Stringv8toStd(info.Data()->ToString()));
		info.GetReturnValue().Set(staticGetterCallback(property, propertyName));
	}

	#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
	void CallbackHandler::staticGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info){
		std::string propertyName(Toolbox::Stringv8toStd(info.Data()->ToString()));
		info.GetReturnValue().Set(staticGetterCallback(property, propertyName));
	}
	#endif

	v8::Local<v8::Value> CallbackHandler::staticGetterCallback(v8::Local<v8::String> property, const std::string& propertyName)
	{
		if(staticObjectMap.find(propertyName) == staticObjectMap.end())
		{
			Toolbox::throwException("Property '" + propertyName + "' not found.");
			return Nan::Undefined();
		}

		return staticObjectMap[propertyName]->get();
	}

	void CallbackHandler::staticSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void>& info) {
		std::string propertyName(Toolbox::Stringv8toStd(info.Data()->ToString()));
		staticSetterCallback(property, value, propertyName);
	}

	#if (NODE_MODULE_VERSION > NODE_0_10_MODULE_VERSION)
	void CallbackHandler::staticSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
		std::string propertyName(Toolbox::Stringv8toStd(info.Data()->ToString()));
		staticSetterCallback(property, value, propertyName);
	}
	#endif

	void CallbackHandler::staticSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const std::string& propertyName)
	{
		if(staticObjectMap.find(propertyName) == staticObjectMap.end())
		{
			Toolbox::throwException("Property '" + propertyName + "' not found.");
			return;
		}

		if(staticObjectMap[propertyName]->isConst())
		{
			Toolbox::throwException("Property '" + propertyName + "' is a constant. Its value may not be changed.");
			return;
		}
		staticObjectMap[propertyName]->setValue(value);
	}

	void CallbackHandler::staticFunctionCallback(const Nan::FunctionCallbackInfo<v8::Value>& info)
	{
		std::string name;			// function name
		TClass *scope = nullptr;	// function scope

		try
		{
			name  = resolveCallbackName(info.Data());
			scope = resolveCallbackScope(info.Data(), false);
		}
		catch(const std::invalid_argument& e)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException(e.what());
			return;
		}

		v8::Local<v8::Function> callback;
		v8::Local<v8::Array> args = getInfoArgs(&callback, info);

		FunctionProxy* proxy = FunctionProxyFactory::fromArgs(name, scope, args);
		if(proxy == nullptr)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("No suitable method named '" + name + "' found for the supplied arguments in '" + std::string(scope->GetName()) + "'.");
			return;
		}


		if(callback.IsEmpty())
		{
			proxy->prepareCall(args);
			ObjectProxyBuilder builder;
			proxy->call(nullptr, builder);
			delete proxy;

			if(builder.isValid())
			{
				ObjectProxy *resultProxy = builder.createObjectProxy();
				if(resultProxy)
				{
					if(Types::isV8Primitive(resultProxy->get()) || resultProxy->isPrimitive())
					{
						info.GetReturnValue().Set(resultProxy->get());
						delete resultProxy;
					}
					else
					{
						info.GetReturnValue().Set(resultProxy->get());
					}
				}
			}
		}
		else
		{
			AsyncCallParam *asyncCallParam = new AsyncCallParam();
			Nan::Persistent<v8::Array> persistentArgs(args);

			/*
			 * Clone the FunctionProxy because when the same function is called multiple times
			 * prepareCall might be called with new data before the old call finished,
			 * which leads to unexpected results (while testing: multiple frees of the param buffer)
			 */
			FunctionProxy* cloneProxy = proxy->clone();

			asyncCallParam->params.Reset(persistentArgs);
			asyncCallParam->proxy = cloneProxy;
			asyncCallParam->selfAddress = nullptr;
			cloneProxy->prepareCall(args);
			AsyncRunner *runner = new AsyncRunner(&asyncMemberCall, asyncCallParam, callback);
			runner->run();
		}
	}


	void CallbackHandler::ctorCallback(const Nan::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Local<v8::Object> instance = info.This();

		if (!info.IsConstructCall())
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("Can not call this constructor as plain function. Use the new operator.");
			return;
		}

		if(instance->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("Unexpected internal field count.");
			return;
		}

		std::string name;
		TClass *clazz = nullptr;
		try
		{
			name  = resolveCallbackName(info.Data());
			clazz = resolveCallbackScope(info.Data(), false);
		}
		catch(const std::invalid_argument& e)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException(e.what());
			return;
		}

		if(clazz->Property() & kIsAbstract) {
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("This class is abstract.");
			return;
		}

		v8::Local<v8::Function> callback;
		v8::Local<v8::Array> args = getInfoArgs(&callback, info);

		FunctionProxy *funcProxy = FunctionProxyFactory::fromArgs(name, clazz, args);

		if(funcProxy == nullptr)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("No suitable constructor found for the supplied arguments. Could not create a new '" + std::string(clazz->GetName()) + "'.");
			return;
		}


		if(callback.IsEmpty())
		{	// create object on current thread
			funcProxy->prepareCall(args);
			ObjectProxyBuilder builder;
			builder.setBaseInstance(instance);
			funcProxy->call(nullptr, builder, true);
			delete funcProxy;

			if(!builder.isValid())
			{
				info.GetReturnValue().Set(Nan::Undefined());
				Toolbox::throwException("Constructor call failed. Could not create a new '" + std::string(clazz->GetName()) + "'.");
				return;
			}
			ObjectProxy *proxy = builder.createObjectProxy();
			if(proxy)
			{
				info.GetReturnValue().Set(proxy->getWeakPeristent());
			}
			return;
		}
		else
		{
			AsyncCallParam *asyncCallParam = new AsyncCallParam();
			Nan::Persistent<v8::Array> persistentArgs(args);

			/*
			 * Clone the FunctionProxy because when the same function is called multiple times
			 * prepareCall might be called with new data before the old call finished,
			 * which leads to unexpected results (while testing: multiple frees of the param buffer)
			 */
			FunctionProxy* cloneProxy = funcProxy->clone();

			asyncCallParam->params.Reset(persistentArgs);
			asyncCallParam->proxy = cloneProxy;
			asyncCallParam->selfAddress = nullptr;
			asyncCallParam->construction = true;
			//asyncCallParam->instance = &instance;
			cloneProxy->prepareCall(args);
			AsyncRunner *runner = new AsyncRunner(&asyncMemberCall, asyncCallParam, callback);
			runner->run();
		}
	}

	void CallbackHandler::memberGetterCallback(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value>& info)
	{
		v8::Local<v8::Object> instance = info.This();

		if(instance->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("Unexpected internal field count.");
			return;
		}

		std::map<std::string, ObjectProxy*> *propertyMap = (std::map<std::string, ObjectProxy*>*) Nan::GetInternalFieldPointer(instance, Toolbox::PropertyMapPtr);
		std::string propertyName(Toolbox::Stringv8toStd(property));

		if(propertyMap->find(propertyName) == propertyMap->end())
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("Property '" + propertyName + "' not found.");
			return;
		}

		info.GetReturnValue().Set((*propertyMap)[propertyName]->get());
	}

	void CallbackHandler::memberSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void>& info)
	{
		v8::Local<v8::Object> instance = info.This();

		if(instance->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("Unexpected internal field count.");
			return;
		}

		std::map<std::string, ObjectProxy*> *propertyMap = (std::map<std::string, ObjectProxy*>*) Nan::GetInternalFieldPointer(instance, Toolbox::PropertyMapPtr);
		std::string propertyName(Toolbox::Stringv8toStd(property));

		if(propertyMap->find(propertyName) == propertyMap->end())
		{
			Toolbox::throwException("Property '" + propertyName + "' not found.");
			return;
		}

		if((*propertyMap)[propertyName]->isConst())
		{
			Toolbox::throwException("Property '" + propertyName + "' is a constant. Its value may not be changed.");
			return;
		}

		(*propertyMap)[propertyName]->setValue(value);
	}

	void CallbackHandler::memberFunctionCallback(const Nan::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Local<v8::Object> instance = info.This();

		if(instance->InternalFieldCount() < Toolbox::INTERNAL_FIELD_COUNT)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("Unexpected internal field count.");
			return;
		}

		std::string name;
		TClass *scope = nullptr;
		try
		{
			name  = resolveCallbackName(info.Data());
			scope = resolveCallbackScope(info.Data(), false);
		}
		catch(const std::invalid_argument& e)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException(e.what());
			return;
		}

		ObjectProxy* holder = (ObjectProxy*) Nan::GetInternalFieldPointer(instance, Toolbox::ObjectProxyPtr);
		if(holder == nullptr)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("Could not resolve enclosing '" + std::string(scope->GetName()) + "' instance to call '" + name + "' on.");
			return;
		}

		v8::Local<v8::Function> callback;
		v8::Local<v8::Array> args = getInfoArgs(&callback, info);

		FunctionProxy* proxy = FunctionProxyFactory::fromArgs(name, scope, args);
		if(proxy == nullptr)
		{
			info.GetReturnValue().Set(Nan::Undefined());
			Toolbox::throwException("No suitable method named '" + name + "' found for the supplied arguments in '" + std::string(scope->GetName()) + "'.");
			return;
		}

		if(callback.IsEmpty())
		{
			proxy->prepareCall(args);
			ObjectProxyBuilder builder;
			proxy->call(holder->getAddress(), builder);
			delete proxy;
			if(builder.isValid())
			{
				ObjectProxy *resultProxy = builder.createObjectProxy();
				if(resultProxy)
				{
					if(Types::isV8Primitive(resultProxy->get()) || resultProxy->isPrimitive())
					{
						info.GetReturnValue().Set(resultProxy->get());
						delete resultProxy;
					}
					else
					{
						info.GetReturnValue().Set(resultProxy->get());
					}
				}
			}
		}
		else
		{
			AsyncCallParam *asyncCallParam = new AsyncCallParam();
			Nan::Persistent<v8::Array> persistentArgs(args);

			/*
			 * Clone the FunctionProxy because when the same function is called multiple times
			 * prepareCall might be called with new data before the old call finished,
			 * which leads to unexpected results (while testing: multiple frees of the param buffer)
			 */
			FunctionProxy* cloneProxy = proxy->clone();

			asyncCallParam->params.Reset(persistentArgs);
			asyncCallParam->proxy = cloneProxy;
			asyncCallParam->selfAddress = holder->getAddress();
			cloneProxy->prepareCall(args);
			AsyncRunner *runner = new AsyncRunner(&asyncMemberCall, asyncCallParam, callback);
			runner->run();
		}
	}

	void CallbackHandler::asyncMemberCall(AsyncRunner *runner, void *param)
	{
		AsyncCallParam *asyncCallParam = (AsyncCallParam*)param;

		std::vector<ObjectProxyBuilder> resultVector;
		ObjectProxyBuilder *builder = new ObjectProxyBuilder();
		builder->bindAllocatedMemory(builder);
		if(asyncCallParam->instance != nullptr)
		{
			builder->setBaseInstance(*(asyncCallParam->instance));
		}

		asyncCallParam->proxy->call(asyncCallParam->selfAddress, *builder, asyncCallParam->construction);

		resultVector.push_back(*builder);
		runner->setResult(resultVector);

		delete asyncCallParam->proxy;
		delete asyncCallParam;
	}


	v8::Local<v8::Value> CallbackHandler::createFunctionCallbackData(std::string functionName, TClass *scope)
	{
		Nan::EscapableHandleScope handle_scope;

		if(scope != nullptr && scope->IsLoaded())
		{
			return handle_scope.Escape(Nan::New(std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + functionName)).ToLocalChecked());
		}
		else
		{
			return handle_scope.Escape(Nan::New(functionName).ToLocalChecked());
		}
	}

	v8::Local<v8::Value> CallbackHandler::createFunctionCallbackData(TClass *scope)
	{
		Nan::EscapableHandleScope handle_scope;

		if(scope == nullptr || !scope->IsLoaded())
		{
			return handle_scope.Escape(Nan::Undefined());
		}

		std::string className(scope->GetName());
		std::size_t idx = className.find_last_of("::");

		if(idx != std::string::npos)
		{
			className = className.substr(idx+1);
		}

		return handle_scope.Escape(Nan::New(std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + className)).ToLocalChecked());
	}

	TClass* CallbackHandler::resolveCallbackScope(v8::Local<v8::Value> data, bool allowNull) throw(std::invalid_argument)
	{
		std::string scopeName = Toolbox::Stringv8toStd(data->ToString());
		std::size_t idx = scopeName.find_last_of(CALLBACK_DATA_DELIMITER);

		DictFuncPtr_t dictPtr = nullptr;
		if(idx != std::string::npos)
		{
			scopeName = scopeName.substr(0, idx);
			dictPtr = gClassTable->GetDict(scopeName.c_str());
		}
		else if (allowNull)
		{ // global found
			return nullptr;
		}
		else
		{
			throw std::invalid_argument(std::string("No Delimiter in '" + scopeName + "' was found."));
		}

		if(dictPtr == nullptr)
		{
			throw std::invalid_argument(std::string("No scope named '" + scopeName + "' was found."));
		}

		TClass *scope = dictPtr();

		if(!allowNull)
		{
			if(scope == nullptr)
			{
				throw std::invalid_argument(std::string("The scope named '" + scopeName + "' is null."));
			}
			else if(!scope->IsLoaded())
			{
				throw std::invalid_argument(std::string("The scope named '" + scopeName + "' is not loaded."));
			}
		}

		return scope;
	}

	std::string CallbackHandler::resolveCallbackName(v8::Local<v8::Value> data) throw(std::invalid_argument)
	{
		std::string functionName = Toolbox::Stringv8toStd(data->ToString());
		std::size_t idx = functionName.find_last_of(CALLBACK_DATA_DELIMITER);

		if(idx != std::string::npos)
		{
			functionName = functionName.substr(idx+1);
		}

		return functionName;
	}

	v8::Local<v8::Array> CallbackHandler::getInfoArgs(v8::Local<v8::Function> *callback, Nan::FunctionCallbackInfo<v8::Value> const& info)
	{
		Nan::EscapableHandleScope handle_scope;

		if(info.Length() < 1)
		{
			return handle_scope.Escape(Nan::New<v8::Array>(0));
		}

		int endIndex = info.Length() - 1;
		if(info[endIndex]->IsFunction())
		{
			*callback = v8::Local<v8::Function>::Cast(info[endIndex]);
			endIndex--;
		}

		v8::Local<v8::Array> args = Nan::New<v8::Array>(endIndex + 1);

		for(int i = 0; i <= endIndex; i++)
		{
			args->Set(i, info[i]);
		}

		return handle_scope.Escape(args);
	}
}
