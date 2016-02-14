#include "EnumInfo.h"

namespace rootJS
{

	EnumInfo::EnumInfo(const  TEnum& type) : MetaInfo(nullptr), type(type)
	{}

	EnumInfo::~EnumInfo()
	{}

	Long_t EnumInfo::GetOffset()
	{
		return 0;
	}

	bool EnumInfo::isGlobal()
	{
		return (type.GetClass() == nullptr);
	}

	bool EnumInfo::isConst()
	{
		return true;
	}

	bool EnumInfo::isStatic()
	{
		return true;
	}

	const char* EnumInfo::getTypeName()
	{
		return type.GetQualifiedName();
	}

	const char* EnumInfo::getName()
	{
		return type.GetName();
	}

	EnumInfo* EnumInfo::clone()
	{
		return new EnumInfo(type);
	};

}
