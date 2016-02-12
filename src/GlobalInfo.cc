#include "GlobalInfo.h"

namespace rootJS
{
	GlobalInfo::GlobalInfo(const  TGlobal& type) : MetaInfo(nullptr), currentObject(type)
	{
		baseAddress = type.GetAddress();
	}

	GlobalInfo::~GlobalInfo()
	{}

	bool GlobalInfo::isGlobal()
	{
		return true;
	}

	Long_t GlobalInfo::GetOffset()
	{
		return 0;
	}

	bool GlobalInfo::isConst()
	{
		if(currentObject.Property() & kIsConstant)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	const char* GlobalInfo::getTypeName()
	{
		return currentObject.GetTypeName();
	}

	const char* GlobalInfo::getName()
	{
		return currentObject.GetName();
	}
}
