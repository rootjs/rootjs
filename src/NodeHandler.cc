#include "NodeHandler.h"

#include "NodeApplication.h"
#include "ClassExposer.h"
#include "GlobalInfo.h"
#include "Toolbox.h"
#include "Types.h"

#include <iostream>

#include <TClassTable.h>
#include <TSystem.h>

namespace rootJS
{

	NodeHandler *NodeHandler::instance;
	bool NodeHaer::initialized;

	void NodeHandler::initialize(v8::Local<v8::Object> exports, v8::Local<v8::Object> module)
	{
		if (!initialized)
		{

			NodeApplication::CreateNodeApplication();
			ObjectProxyFactory::initializeProxyMap();
			instance = new NodeHandler(exports);
			instance->exposeROOT();

		}
		else
		{
			Toolbox::throwException("The NodeHandler can only be initialized once.");
		}
	}

	NodeHandler::NodeHandler(v8::Local<v8::Object> exports)
	{
		this->exportPersistent.Reset(v8::Isolate::GetCurrent(),exports);
		this->exports = exports;
		initialized = true;
	}


	NodeHandler* NodeHandler::getInstance(){
		if(initialized){
			return NodeHandler::instance;
		} else {throw std::invalid_argument("NodeHandler not loaded yet"); }
	}

	void NodeHandler::exposeROOT()
	{
		gInterpreter->SetClassAutoloading(kTRUE);

		try
		{
			exposeGlobals();
			exposeGlobalFunctions();
			exposeClasses();
			exposeInterfaceFunctions();
		}
		catch (const std::invalid_argument& e)
		{
			Toolbox::throwException(e.what());
			return;
		}
	}

	void NodeHandler::exposeGlobals() throw (std::invalid_argument)
	{
		TCollection *globals = gROOT->GetListOfGlobals(kTRUE);
		TIter next(globals);
		v8::Local<v8::Object> exportsLocal = v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(),exportPersistent);
		while (TGlobal *global = (TGlobal*) next())
		{
			if( (!global->IsValid()) || (global->GetAddress() == nullptr))
			{
				Toolbox::logError("Invalid global instance found.");
				continue;
			}

			GlobalInfo info(*global);
			ObjectProxy *proxy = ObjectProxyFactory::createObjectProxy(info, nullptr);
			if (proxy != nullptr)
			{
				CallbackHandler::registerGlobalObject(std::string(global->GetName()), proxy);
				exportsLocal->SetAccessor(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), global->GetName()),
				                           CallbackHandler::globalGetterCallback, CallbackHandler::globalSetterCallback);
			}
		}
	}

	void NodeHandler::exposeGlobalFunctions() throw (std::invalid_argument)
	{
		v8::Local<v8::Object> exportsLocal = v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(),exportPersistent);
		TCollection *functions = gROOT->GetListOfGlobalFunctions(kTRUE);

		TIter next(functions);
		while (TFunction *function = (TFunction*) next())
		{
			if (!function->IsValid())
			{
				Toolbox::logError("Invalid global function found.");
				continue;
			}

			v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(function->GetName(), nullptr);
			exportsLocal->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), function->GetName()),
			             v8::Function::New(v8::Isolate::GetCurrent(), CallbackHandler::globalFunctionCallback, data));
		}
	}

	void NodeHandler::exposeClasses() throw (std::invalid_argument)
	{	//TODO implement maps for dynamic loading
		v8::Local<v8::Object> exportsLocal = v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(),exportPersistent);
		for (int i = 0; i < gClassTable->Classes(); i++)
		{
			DictFuncPtr_t funcPtr = gClassTable->GetDict(gClassTable->At(i));
			if (funcPtr == nullptr)
			{
				throw std::invalid_argument(
				    std::string("Specified class is null."));
			}

			TClass *clazz = funcPtr(); // call dictionary function on class
			if (clazz == nullptr || !clazz->IsLoaded())
			{
				throw std::invalid_argument(
				    std::string("Specified class is not loaded."));
			}
			if ((((std::string) clazz->GetName()).find(":") == std::string::npos) && (!exportsLocal->Has(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),clazz->GetName())))){
				if ((clazz->Property() & kIsClass))
				{
					Toolbox::logInfo(std::string("loading class ").append(clazz->GetName()));
					exportsLocal->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), clazz->GetName()), TemplateFactory::getConstructor(clazz));
					continue;
				}
				if((clazz->Property() & kIsNamespace))
				{
					Toolbox::logInfo(std::string("loading namespace ").append(clazz->GetName()));
					exportsLocal->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),clazz->GetName()),TemplateFactory::getInstance(clazz));
					continue;
				}
			}
			else
			{
				ClassExposer::expose(clazz, exportsLocal);
			}
		}
	}


	void NodeHandler::loadlibraryCallback(const v8::FunctionCallbackInfo<v8::Value> &info) throw (std::invalid_argument) {
		v8::Local<v8::Value> arg;
		if(!((info.Length() == 1) && ((arg = info[0])->IsString())))
		{
			throw std::invalid_argument("Usage: Pass name of the library");
		}
		std::string libname = Toolbox::Stringv8toStd(v8::Local<v8::String>::Cast(arg));
		int rcode = gSystem->Load(libname.c_str());
		info.GetReturnValue().Set(rcode);
		if(rcode >= 0) {
			NodeHandler::getInstance()->refreshExports();
		}
		}

	void NodeHandler::refreshExportsCallback(const v8::FunctionCallbackInfo<v8::Value> &info) throw (std::invalid_argument){
		if(info.Length() != 0)
		{
			throw std::invalid_argument("Usage: No arguments. Refreshes the exported functions");
		}
		NodeHandler::getInstance()->refreshExports();
	}

	void NodeHandler::refreshExports() {
		exposeClasses(); //crashes
	}


	void NodeHandler::exposeInterfaceFunctions() {
		v8::Local<v8::Object> exportsLocal = v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(),exportPersistent);
		exportsLocal->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),"loadlibrary"),
					 v8::Function::New(v8::Isolate::GetCurrent(), NodeHandler::loadlibraryCallback));
		exportsLocal->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),"refreshExports"),v8::Function::New(v8::Isolate::GetCurrent(),NodeHandler::refreshExportsCallback));
	}
}
