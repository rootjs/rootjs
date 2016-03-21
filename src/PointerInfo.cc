#include <stdlib.h>
#include <string.h>

#include "PointerInfo.h"

namespace rootJS
{
	PointerInfo::PointerInfo(void* baseAddr, const char* typeName, int ptrDepth) : MetaInfo(baseAddr, ptrDepth)
	{
		int len = strlen(typeName);
		this->typeName = (char*)malloc(sizeof(char[len+1]));
		strncpy(this->typeName, typeName, len);
		this->typeName[len] = '\0';
	}

	PointerInfo::~PointerInfo()
	{
		free(this->typeName);
	}

	const char* PointerInfo::getFullTypeName()
	{
		return typeName;
	};

	const char* PointerInfo::getTypeName()
	{
		normalTypeName = std::string(typeName);

		if(normalTypeName.find('*') != std::string::npos) {
			normalTypeName = normalTypeName.substr(0, normalTypeName.find('*'));
		}
		return normalTypeName.c_str();
	};
}
