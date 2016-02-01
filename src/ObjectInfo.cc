#include "ObjectInfo.h"

namespace rootJS
{

	const std::string ObjectInfo::defaultName("undefined");

	ObjectInfo::ObjectInfo(std::string name, TClass* clazz) : name(name), classRef(clazz)
	{}

	ObjectInfo::ObjectInfo(TClass* clazz) : ObjectInfo(defaultName, clazz)
	{}

	ObjectInfo::~ObjectInfo()
	{}

	TClass* ObjectInfo::getInfo()
	{
		return classRef.GetClass();
	}

	Long_t ObjectInfo::getOffset()
	{
		return 0;
	}

	Long_t ObjectInfo::getProperty()
	{
		return classRef.GetClass()->Property();
	}

	std::string ObjectInfo::getName()
	{
		return name;
	}

	std::string ObjectInfo::getTypeName()
	{
		return std::string(classRef.GetClassName());
	}

	std::string ObjectInfo::getFullTypeName()
	{
		return getTypeName();
	}

	TClass* ObjectInfo::getClass()
	{
		return getInfo();
	}

}
