#include "CallbackHandler.h"
#include "Toolbox.h"

#include "Rtypes.h"
#include "TClass.h"
#include "TClassRef.h"
#include "TClassTable.h"

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
		if(proxy)
		{
			info.GetReturnValue().Set(proxy->get());
		}
	}

	void CallbackHandler::globalSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{
		ObjectProxy* proxy = globalObjectMap[std::string(*v8::String::Utf8Value(property->ToString()))];
		if(proxy)
		{
			proxy->setValue(value);
		}
	}

	void CallbackHandler::globalFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();

		std::string name;			// function name
		TClass *scope = nullptr;	// function scope

		try
		{
			name  = resolveCallbackName(args.Data());
			scope = resolveCallbackScope(args.Data(), true);
		}
		catch(const std::invalid_argument& e)
		{
			Toolbox::throwException(e.what());
			args.GetReturnValue().Set(v8::Undefined(isolate));
			return;
		}

		/*
		 * TODO:
		 * v8::Local<v8::Function> *callback = nullptr;
		 *v8::Local<v8::Array> argss = getInfoArgs(callback, args);
		*/

		FunctionProxy* proxy = FunctionProxyFactory::fromArgs(name, scope, args);
		if(proxy != nullptr)
		{
			args.GetReturnValue().Set(proxy->call(args));
			delete proxy;
		}
		else
		{
			Toolbox::throwException("The method could not be determined.");
		}
	}


	void CallbackHandler::registerStaticObject(const std::string &name, TClass *scope, ObjectProxy* proxy)
	{
		if(scope == nullptr || !scope->IsLoaded())
		{
			staticObjectMap[name] = proxy;
		}
		else
		{
			staticObjectMap[std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + name)] = proxy;
		}
	}

	void CallbackHandler::staticGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
	{}

	void CallbackHandler::staticSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{}

	void CallbackHandler::staticFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();

		std::string name;			// function name
		TClass *scope = nullptr;	// function scope

		try
		{
			name  = resolveCallbackName(args.Data());
			scope = resolveCallbackScope(args.Data(), true);
		}
		catch(const std::invalid_argument& e)
		{
			Toolbox::throwException(e.what());
			args.GetReturnValue().Set(v8::Undefined(isolate));
			return;
		}

		FunctionProxy* proxy = FunctionProxyFactory::fromArgs(name, scope, args);
		if(proxy != nullptr)
		{
			args.GetReturnValue().Set(proxy->call(args));
			delete proxy;
		}
		else
		{
			Toolbox::throwException("The method could not be determined.");
		}
	}


	void CallbackHandler::ctorCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Isolate* isolate = info.GetIsolate();
		v8::Local<v8::Object> instance = info.This();

		if(instance->InternalFieldCount() < 1)
		{
			Toolbox::throwException("Unexpected internal field count.");
			info.GetReturnValue().Set(v8::Undefined(isolate));
			return;
		}

		if (!info.IsConstructCall())
		{
			Toolbox::throwException("Can not call this constructor as plain function. Use the new operator.");
			info.GetReturnValue().Set(v8::Undefined(isolate));
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
			Toolbox::throwException(e.what());
			info.GetReturnValue().Set(v8::Undefined(isolate));
			return;
		}

		v8::Local<v8::Function> *callback = nullptr;
		v8::Local<v8::Array> args = getInfoArgs(callback, info);

		if(callback == nullptr)	// create object on current thread
		{
			// try
			void *address = FunctionProxyFactory::createInstance(name, clazz, args);
			// catch

			if(address == nullptr)
			{
				std::string msg("No suitable constructor found for the supplied arguments. Could not create a new: ");
				msg.append(clazz->GetName());
				msg.append(".");

				Toolbox::throwException(msg);
				info.GetReturnValue().Set(v8::Undefined(isolate));
				return;
			}

			//try
			/* ObjectProxy proxy* = */ ObjectProxyFactory::createObjectProxy(address, clazz, instance); /*address, type, holder*/
			//catc

			info.GetReturnValue().Set(instance);
			return;
		}

		// create object asynchronously

		// TODO: AsyncRunner...

		info.GetReturnValue().Set(v8::Undefined(isolate)); // TODO remove this line
	}

	void CallbackHandler::memberGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
	{}

	void CallbackHandler::memberSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{}

	void CallbackHandler::memberFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{}


	v8::Local<v8::Value> CallbackHandler::createFunctionCallbackData(std::string functionName, TClass *scope)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		if(scope != nullptr && scope->IsLoaded())
		{
			return handle_scope.Escape(v8::String::NewFromUtf8(isolate, std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + functionName).c_str()));
		}
		else
		{
			return handle_scope.Escape(v8::String::NewFromUtf8(isolate, functionName.c_str()));
		}
	}

	v8::Local<v8::Value> CallbackHandler::createFunctionCallbackData(TClass *scope)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		if(scope == nullptr || !scope->IsLoaded())
		{
			return handle_scope.Escape(v8::Undefined(isolate));
		}

		std::string className(scope->GetName());
		std::size_t idx = className.find_last_of("::");

		if(idx != std::string::npos)
		{
			className = className.substr(idx+1);
		}

		return handle_scope.Escape(v8::String::NewFromUtf8(isolate, std::string(scope->GetName() + CALLBACK_DATA_DELIMITER + className).c_str()));
	}

	TClass* CallbackHandler::resolveCallbackScope(v8::Local<v8::Value> data, bool allowNull) throw(std::invalid_argument)
	{
		std::string scopeName = toString(data);
		std::size_t idx = scopeName.find_last_of(CALLBACK_DATA_DELIMITER);

		DictFuncPtr_t dictPtr = nullptr;
		if(idx != std::string::npos)
		{
			scopeName = scopeName.substr(0, idx);
			DictFuncPtr_t dictPtr = gClassTable->GetDict(scopeName.c_str());
		}
		else if (allowNull) // global found
		{
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
			else if(scope->IsLoaded())
			{
				throw std::invalid_argument(std::string("The scope named '" + scopeName + "' is not loaded."));
			}
		}

		return scope;
	}

	std::string CallbackHandler::resolveCallbackName(v8::Local<v8::Value> data) throw(std::invalid_argument)
	{
		std::string functionName = toString(data);
		std::size_t idx = functionName.find_last_of(CALLBACK_DATA_DELIMITER);

		if(idx != std::string::npos)
		{
			functionName = functionName.substr(idx+1);
		}

		return functionName;
	}

	std::string CallbackHandler::toString(v8::Local<v8::Value> data) throw(std::invalid_argument)
	{
		const char *str = *v8::String::Utf8Value(data);

		if(str == nullptr)
		{
			throw std::invalid_argument(std::string("String conversion failed."));
		}

		return std::string(str);
	}

	v8::Local<v8::Array> CallbackHandler::getInfoArgs(v8::Local<v8::Function> *callback, v8::FunctionCallbackInfo<v8::Value> const& info)
	{
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		v8::EscapableHandleScope handle_scope(isolate);

		if(info.Length() < 1)
		{
			return handle_scope.Escape(v8::Array::New(isolate, 0));
		}

		int endIndex = info.Length() - 1;
		if(info[endIndex]->IsFunction())
		{
			*callback = v8::Local<v8::Function>::Cast(info[endIndex]);
			endIndex--;
		}

		v8::Local<v8::Array> args = v8::Array::New(isolate, endIndex + 1);

		for(int i = 0; i <= endIndex; i++)
		{
			args->Set(i, info[i]);
		}

		return handle_scope.Escape(args);
	}
}
