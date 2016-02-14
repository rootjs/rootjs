#include <stdlib.h>
#include <string.h>
#include <string>

#include "PointerInfo.h"

namespace rootJS
{
	PointerInfo::PointerInfo(void* baseAddr, const char* typeName, int ptrDepth /* = 2 */) : MetaInfo(baseAddr), ptrDepth(ptrDepth)
	{
		int len = strlen(typeName);
		this->typeName = (char*)malloc(sizeof(char[len+1]));
		strncpy(this->typeName, typeName, len);
		this->typeName[len] = '\0';
	}

	PointerInfo::~PointerInfo() {
		free(this->typeName);
	}

	void* PointerInfo::getAddress()
	{
		if(ptrDepth >= 2)
		{
			void* result = &baseAddress;
			for(int i = ptrDepth; i > 2; i--)
			{
				result = (void*)(*(void**)result);
			}
			return result;
		}
		else
		{
			ptr = &baseAddress;
			ptrptr = &ptr;

			if(ptrDepth == 1)
			{
				return ptr;
			}
			else
			{
				return ptrptr;
			}
		}
	}

	const char* PointerInfo::getFullTypeName() {
		return typeName;
	};

	const char* PointerInfo::getTypeName() {
		std::string normalTypeName(typeName);

		if(normalTypeName.find('*') != std::string::npos) {
			normalTypeName = normalTypeName.substr(0, normalTypeName.find('*'));
		}
		return normalTypeName.c_str();
	};
}
