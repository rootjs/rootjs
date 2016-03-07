#include "EnumConstInfo.h"

namespace rootJS
{

	EnumConstInfo::EnumConstInfo(const  TEnumConstant& type) : EnumConstInfo(type, false)
	{}

	EnumConstInfo::EnumConstInfo(const  TEnumConstant& type, bool isConst) : MetaInfo(nullptr, 2), type(type), mIsConst(isConst)
	{
		baseAddress = type.GetAddress();
	}

	EnumConstInfo::EnumConstInfo(const EnumConstInfo & info) : MetaInfo(info.baseAddress, 2), type(info.type), mIsConst(info.mIsConst)
	{
	}

	/*
	EnumConstInfo& EnumConstInfo::operator=(const EnumConstInfo & info)
	{
		this->baseAddress = info.baseAddress;
		this->type = info.type;
		this->pseudoConst = info.pseudoConst;
		this->ptrDepth = info.ptrDepth;

		return *this;
	} */

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
		return (((bool) (type.Property() & kIsConstant)) || mIsConst);
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
		return new EnumConstInfo(type, mIsConst);
	};

}
