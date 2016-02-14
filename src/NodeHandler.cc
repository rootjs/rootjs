#include "NodeHandler.h"

#include "NodeApplication.h"
#include "GlobalInfo.h"
#include "Toolbox.h"
#include "Types.h"

#include <iostream>
#include <TROOT.h>
#include <TClassTable.h>
#include <TSystem.h>
#include <queue>

namespace rootJS
{

	NodeHandler *NodeHandler::instance;
	bool NodeHandler::initialized;

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


	NodeHandler* NodeHandler::getInstance() {
		if(initialized) {
			return NodeHandler::instance;
		} else {
			throw std::invalid_argument("NodeHandler not loaded yet");
		}
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
				Toolbox::logInfo("Invalid global instance found.",1);
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

		TCollection *functions = gROOT->GetListOfGlobalFunctions(kTRUE);

		TIter next(functions);
		while (TFunction *function = (TFunction*) next())
		{
			if (!function->IsValid())
			{
				Toolbox::logInfo("Invalid global function found.",1);
				continue;
			}

			std::string exposeName(function->GetName());
			if(function->ExtraProperty() & kIsOperator) {
				std::map<std::string, std::string>::const_iterator opNameIt = Types::operatorNames.find(function->GetName());
				if(opNameIt != Types::operatorNames.end()) {
					exposeName = opNameIt->second;
				}
			}

			v8::Local<v8::Value> data = CallbackHandler::createFunctionCallbackData(function->GetName(), nullptr);
			exports->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), exposeName.c_str()),
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
			if ((((std::string) clazz->GetName()).find(":") == std::string::npos) && (!exportsLocal->Has(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),clazz->GetName())))) {
				if ((clazz->Property() & kIsClass))
				{
					Toolbox::logInfo(std::string("loading class ").append(clazz->GetName()),2);
					exportsLocal->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), clazz->GetName()), TemplateFactory::getConstructor(clazz));
					continue;
				}
				if((clazz->Property() & kIsNamespace))
				{
					Toolbox::logInfo(std::string("loading namespace ").append(clazz->GetName()),2);
					exportsLocal->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),clazz->GetName()),TemplateFactory::getInstance(clazz));
					continue;
				}
			}
			else
			{
				exposeHierarchy(clazz, exportsLocal);
			}
		}
	}

	void NodeHandler::exposeHierarchy(TClass* clazz,v8::Local<v8::Object> exports)  throw(std::invalid_argument)
	{
		std::vector<std::string> vec;
		std::string name(clazz->GetName());
		if(name.find('<') != std::string::npos)
		{
			//TODO Handle templates
			// Toolbox::logInfo(std::string("omitting template ").append(clazz->GetName()));
			return;
		}
		splitClassName(name, vec);
		std::queue<std::string> pathque;
		std::queue<std::string> nameque;
		std::string buff = vec[0];
		nameque.push(buff);
		pathque.push(buff);

		//setting up the names under which the object are exported and their names in ROOT
		for(uint i = 1; i < vec.size(); i++)
		{
			std::string buff = vec[i];
			nameque.push(buff);
			std::string pathbuff = pathque.back();
			pathbuff.append("::");
			pathbuff.append(buff);
			pathque.push(pathbuff);
		}
		v8::Local<v8::Object>& scope = exports;
		while(!nameque.empty())
		{
			v8::Local<v8::Object> obj;
			if (!scope->Has(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str())))
			{
				DictFuncPtr_t funcPtr(gClassTable->GetDict(pathque.front().c_str()));
				if (funcPtr == nullptr)
				{
					Toolbox::logInfo(std::string("creating stub namespace: ").append(pathque.front()), 2);
					scope->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str()), obj->New(v8::Isolate::GetCurrent()));
				}
				else
				{
					try
					{
						TClass *curclazz = funcPtr();
						if (curclazz->Property() & kIsNamespace)
						{
							Toolbox::logInfo(std::string("loading namespace ").append(curclazz->GetName()),2);
							obj = TemplateFactory::getInstance(curclazz);
							scope->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str()), obj);
						}
						if (curclazz->Property() & kIsClass)
						{
							Toolbox::logInfo(std::string("loading class ").append(curclazz->GetName()),2);
							obj = TemplateFactory::getConstructor(curclazz);
							scope->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str()), obj);
						}
					}
					catch (const std::invalid_argument &e)
					{
						throw std::invalid_argument(e);
					}
				}
				//TODO figure out if there are other cases as well
			}

			scope = v8::Local<v8::Object>::Cast(scope->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),nameque.front().c_str())));
			pathque.pop();
			nameque.pop();
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

	void NodeHandler::refreshExportsCallback(const v8::FunctionCallbackInfo<v8::Value> &info) throw (std::invalid_argument) {
		if(info.Length() != 0)
		{
			throw std::invalid_argument("Usage: No arguments. Refreshes the exported functions");
		}
		NodeHandler::getInstance()->refreshExports();
	}

	void NodeHandler::refreshExports() {
		exposeClasses();
	}


	void NodeHandler::exposeInterfaceFunctions() {
		v8::Local<v8::Object> exportsLocal = v8::Local<v8::Object>::New(v8::Isolate::GetCurrent(),exportPersistent);
		exportsLocal->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),"loadlibrary"),
		                  v8::Function::New(v8::Isolate::GetCurrent(), NodeHandler::loadlibraryCallback));
		exportsLocal->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),"refreshExports"),v8::Function::New(v8::Isolate::GetCurrent(),NodeHandler::refreshExportsCallback));
	}


	std::vector<std::string> NodeHandler::splitClassName(std::string name, std::vector<std::string>& vec)
	{

		std::string delimiter = "::";
		size_t pos = 0;
		std::string token;
		while ((pos = name.find(delimiter)) != std::string::npos)
		{
			token = name.substr(0, pos);
			vec.push_back(token);
			name.erase(0, pos + delimiter.length());
		}
		vec.push_back(name);

		return vec;
	}


}
