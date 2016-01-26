#include "CallbackHandler.h"

namespace RootJS
{

	std::map<std::string, ObjectProxy*> CallbackHandler::globalProxyMap;

	void CallbackHandler::setGlobalProxy(const std::string &name, ObjectProxy* proxy)
	{
		globalProxyMap[name] = proxy;
	}

	void CallbackHandler::globalGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
	{
		ObjectProxy* proxy = globalProxyMap[std::string(*v8::String::Utf8Value(property->ToString()))];
		if(proxy)
		{
			info.GetReturnValue().Set(proxy->get());
		}
	}

	void CallbackHandler::globalSetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{
		ObjectProxy* proxy = globalProxyMap[std::string(*v8::String::Utf8Value(property->ToString()))];
		if(proxy)
		{
			proxy->setValue(value);
		}
	}

	void CallbackHandler::ctorCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Isolate* isolate = info.GetIsolate();
		v8::Local<v8::Object> instance = info.This();

		if(instance->InternalFieldCount() < 1)
		{
			return;
		}

		if (!info.IsConstructCall())
		{
			/*
			// Invoked as plain function `MyObject(...)`, turn into constructor call
			const int argc = args.Length();
			v8::Local<v8::Value> argv[argc];

			for (int i = 0; i < argc; i++)
				argv[i] = args[i];

			v8::Local<v8::Function> ctor = args.Callee();
			args.GetReturnValue().Set(ctor->NewInstance(argc, argv));
			*/
			return;
		}

		// retrieve TClass
		DictFuncPtr_t dictPtr = gClassTable->GetDict(*v8::String::Utf8Value(info.Data()));
		if(dictPtr == nullptr)
		{
			return;
		}

		TClass *clazz = dictPtr();
		if(clazz == nullptr)
		{
			return;
		}

		v8::Local<v8::Function> *callback = nullptr;
		int endIndex = 0;

		if(info.Length() > 0)
		{
			if(info[info.Length() - 1]->IsFunction())
			{
				*callback = v8::Local<v8::Function>::Cast(info[info.Length() - 1]);
				endIndex = (info.Length() > 1) ? info.Length() - 1 : 0;
			}
			else
			{
				endIndex = info.Length() - 1;
			}
		}

		TClassRef classRef(clazz);
		v8::Local<v8::Array> args = getInfoArgs(0, endIndex, info);

		if(callback == nullptr)	// create object on current thread
		{

			// try
			void *address = FunctionProxyFactory::createInstance(classRef, args);
			// catch

			if(address == nullptr)
			{
				std::string msg("No suitable constructor found for the supplied arguments. Could not create a new: ");
				msg.append(clazz->GetName());
				isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, msg.c_str())));
			}

			//try
			/* ObjectProxy proxy* = */ ObjectProxyFactory::createObjectProxy(address, classRef, instance); /*address, type, holder*/
			//catch
			/* instance->SetAlignedPointerInInternalField(0, proxy); */

			info.GetReturnValue().Set(instance);
		}

		// create object asynchronously

		// TODO: AsyncRunner...

		info.GetReturnValue().Set(instance); // TODO: remove this line
	}

	void CallbackHandler::getterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
	{
		// info.Holder()->Get(property) == info.This(); ????
	}

	void CallbackHandler::setterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
	{ }

	v8::Local<v8::Array> CallbackHandler::getInfoArgs(int beginIndex, int endIndex, const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Local<v8::Array> args;

		if(info.Length() < 1)
		{
			return v8::Array::New(info.GetIsolate(), 0);
		}

		if(beginIndex >= info.Length() || beginIndex < 0)
		{
			beginIndex = 0;
		}

		if(endIndex >= info.Length() || endIndex < 0)
		{
			endIndex = info.Length() - 1;
		}

		if(endIndex >= beginIndex)
		{
			args = v8::Array::New(info.GetIsolate(), endIndex - beginIndex + 1);

			for(int i = beginIndex; i <= endIndex; i++)
			{
				args->Set(i - beginIndex, info[i]);
			}
		}
		else
		{
			args = v8::Array::New(info.GetIsolate(), beginIndex - endIndex + 1);

			for(int i = beginIndex; i <= endIndex; i--)
			{
				args->Set(beginIndex - i, info[i]);
			}
		}

		return args;
	}
}

