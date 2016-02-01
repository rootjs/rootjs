#ifndef OBJECTINFO_H_
#define OBJECTINFO_H_

#include "TClassRef.h"
#include "TClass.h"
#include "MetaInfo.h"

namespace rootJS
{

	class ObjectInfo : public MetaInfo
	{
		public:
			ObjectInfo(std::string name, TClass *clazz);
			ObjectInfo(TClass *clazz);

			virtual ~ObjectInfo();

			virtual TClass* getInfo();

			virtual Long_t getOffset();
			virtual Long_t getProperty();

			virtual std::string getName();
			virtual std::string getTypeName();
			virtual std::string getFullTypeName();

			virtual TClass* getClass();

		private:
			static const std::string defaultName;

			const std::string name;
			TClassRef classRef;
	};

}

#endif
