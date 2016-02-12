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
#include <TGlobal.h>
#include <TClass.h>
#include <TClassTable.h>
namespace rootJS {

ClassExposer::~ClassExposer() {
}

void ClassExposer::expose(TClass* clazz,v8::Local<v8::Object> exports) {
	std::vector<std::string> vec;
	splitClassName(std::string(clazz->GetName()), vec);//ROOT::Math::Class
		std::queue<std::string> pathque;
		std::queue<std::string> nameque;

		std::string buff = vec[0];
		nameque.push(buff);
		pathque.push(buff);

		//setting up the names under which the object are exported and their names in ROOT
		for(int i = 1; i < vec.size();i++){
			std::string buff = vec[i];
			nameque.push(buff);
			std::string pathbuff = pathque.back();
			pathbuff.append("::");
			pathbuff.append(buff);
			pathque.push(pathbuff);
		}

	v8::Local<v8::Object> obj;
		while(!nameque.empty()){
			obj = exports->Get(	v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),nameque.front().c_str()));
			if(obj.IsEmpty()){
				DictFuncPtr_t funcPtr = gClassTable->GetDict(pathque.front().c_str());
				if(funcPtr = nullptr) {
					//TODO throw something
				}
				
			}

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
