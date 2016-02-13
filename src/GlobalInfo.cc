#include "GlobalInfo.h"

namespace rootJS
{
	GlobalInfo::GlobalInfo(const  TGlobal& type) : MetaInfo(nullptr), type(type)
	{
		baseAddress = type.GetAddress();
	}

	GlobalInfo::~GlobalInfo()
	{}


	Long_t GlobalInfo::GetOffset()
	{
		return 0;
	}

	bool GlobalInfo::isGlobal()
	{
		return true;
	}

	bool GlobalInfo::isConst()
	{
		return (type.Property() & kIsConstant);
	}

	bool GlobalInfo::isStatic()
	{
		return (type.Property() & kIsStatic);	// true
	}

	const char* GlobalInfo::getTypeName()
	{
		return type.GetTypeName();
	}

	const char* GlobalInfo::getName()
	{
		return type.GetName();
	}

	GlobalInfo* GlobalInfo::clone()
	{
		return new GlobalInfo(type);
	};
}
