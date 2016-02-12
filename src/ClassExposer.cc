/*
 * ClassExposer.cc
 *
 *  Created on: Feb 11, 2016
 *      Author: theo
 */
#include <vector>
#include <queue>
#include "ClassExposer.h"
#include "TROOT.h"
#include "TemplateFactory.h"
#include <TGlobal.h>
#include <TClass.h>
#include <TClassTable.h>
namespace rootJS {

ClassExposer::~ClassExposer() {
}

void ClassExposer::expose(TClass* clazz,v8::Local<v8::Object> exports) {
	std::vector<std::string> vec;
	std::string name(clazz->GetName());
	if(name.find('<') != std::string::npos){
		return;
	}
	splitClassName(name, vec);//ROOT::Math::Class
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

	v8::Local<v8::Object> obj;
		while(!nameque.empty()){
			obj = v8::Local<v8::Object>::Cast(exports->Get(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),nameque.front().c_str())));
			if(obj.IsEmpty()){ //if there was no object set yet create one and set it
				DictFuncPtr_t funcPtr(gClassTable->GetDict(pathque.front().c_str()));
				if(funcPtr == nullptr) {
					//TODO throw something
				}
				TClass* curclazz = funcPtr();
				if(curclazz->Property() & kIsNamespace) {
					exports->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str()),
								 TemplateFactory::getInstance(curclazz));
				}
				if(curclazz->Property() & kIsClass) {
					exports->Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), nameque.front().c_str()),
								 TemplateFactory::getConstructor(curclazz));
				}
				//TODO figure out if there are other cases as well
				}
			exports = obj;
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
