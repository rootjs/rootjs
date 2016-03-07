#include "EnumConstInfo.h"

namespace rootJS
{

	EnumConstInfo::EnumConstInfo(const  TEnumConstant& type) : MetaInfo(nullptr, 2), type(type)
	{
		baseAddress = type.GetAddress();
	}

	EnumConstInfo::~EnumConstInfo()
	{}

	Long_t EnumConstInfo::GetOffset()
	{
		return 0;
	}

	bool EnumConstInfo::isGlobal()
	{
		return false;
	}

	bool EnumConstInfo::isConst()
	{
		return true; // (type.Property() & kIsConstant);
	}

	bool EnumConstInfo::isStatic()
	{
		return (type.Property() & kIsStatic);
	}

	const char* EnumConstInfo::getFullTypeName()
	{
		return type.GetFullTypeName();
	}

	const char* EnumConstInfo::getTypeName()
	{
		return type.GetTypeName();
	}

	const char* EnumConstInfo::getName()
	{
		return type.GetName();
	}

	EnumConstInfo* EnumConstInfo::clone()
	{
		return new EnumConstInfo(type);
	};

}
