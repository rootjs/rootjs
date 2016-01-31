#ifndef METAINFO_H_
#define METAINFO_H_

#include "TObject.h"
#include "RtypesCore.h"

namespace rootJS
{

	class MetaInfo
	{
		public:
			virtual ~MetaInfo();

			virtual TObject* getInfo() = 0;

			virtual Long_t getOffset() = 0;
			virtual Long_t getProperty();

			virtual std::string getName() = 0;
			virtual std::string getTypeName() = 0;
			virtual std::string getFullTypeName() = 0;

			virtual TClass* getClass() = 0;

			/*
			virtual bool isPrimitive() = 0;
			virtual bool isConst() = 0;
			virtual bool isStatic() = 0;

			virtual bool isGlobal() = 0;
			*/

		protected:
			// TObject *property;

			MetaInfo(/* TObject *property */);
	};

}

#endif
