/*
 * ClassExposer.cc
 *
 *  Created on: Feb 11, 2016
 *      Author: theo
 */
#include <vector>
#include <queue>
#include "ClassExposer.h"
#include "stdexcept"
#include "TROOT.h"
#include "TemplateFactory.h"
#include "Toolbox.h"
#include <TGlobal.h>
#include <TClass.h>
#include <TClassTable.h>
#include <iostream>

namespace rootJS {

ClassExposer::~ClassExposer() {
}

void ClassExposer::expose(TClass* clazz,v8::Local<v8::Object> exports)  throw(std::invalid_argument)  {
	std::vector<std::string> vec;
	std::string name(clazz->GetName());
	if(name.find('<') != std::string::npos){
		//TODO Handle templates
		return;
	}
	splitClassName(name, vec);
		std::queue<std::string> pathque;
		std::queue<std::string> nameque;
		std::string buff = vec[0];
		nameque.push(buff);
		pathque.push(buff);

		//setting up the names under which the object are exported and their names in ROOT
		for(uint i = 1; i < vec.size();i++){
			std::string buff = vec[i];
			nameque.push(buff);
			std::string pathbuff = pathque.back();
			pathbuff.append("::");
			pathbuff.append(buff);
			pathque.push(pathbuff);
		}
	v8::Local<v8::Object>& scope = exports;
		while(!nameque.empty()) {
			v8::Local<v8::Object> obj;
			if (!scope->Has(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str()))) {
				DictFuncPtr_t funcPtr(gClassTable->GetDict(pathque.front().c_str()));
				if (funcPtr == nullptr) {
					Toolbox::logInfo(std::string("creating stub namespace: ").append(pathque.front()));
					scope->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str()), obj->New(v8::Isolate::GetCurrent()));
				} else{
				try {
					TClass *curclazz = funcPtr();
					if (curclazz->Property() & kIsNamespace) {
						Toolbox::logInfo(std::string("loading namespace ").append(curclazz->GetName()));
						obj = TemplateFactory::getInstance(curclazz);
						scope->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str()), obj);
					}
					if (curclazz->Property() & kIsClass) {
						Toolbox::logInfo(std::string("loading class ").append(curclazz->GetName()));
						obj = TemplateFactory::getConstructor(curclazz);
						scope->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str()), obj);
					}
				} catch (const std::invalid_argument &e) {
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

std::vector<std::string> ClassExposer::splitClassName(std::string name, std::vector<std::string>& vec) {

		std::string delimiter = "::";
	 	size_t pos = 0;
	 	std::string token;
	 	while ((pos = name.find(delimiter)) != std::string::npos) {
	 		token = name.substr(0, pos);
	 		vec.push_back(token);
	 		name.erase(0, pos + delimiter.length());
	 	}
	 	vec.push_back(name);

	 	return vec;
	 }

} /* namespace rootJS */
