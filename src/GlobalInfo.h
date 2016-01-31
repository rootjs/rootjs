#ifndef GLOBALINFO_H_
#define GLOBALINFO_H_

#include "TGlobal.h"
#include "MetaInfo.h"

namespace rootJS
{

	class GlobalInfo : public MetaInfo
	{
		public:
			GlobalInfo(TGlobal *global);
			virtual ~GlobalInfo();

			virtual TGlobal* getInfo();

			virtual Long_t getOffset();
			virtual Long_t getProperty();

			virtual std::string getName();
			virtual std::string getTypeName();
			virtual std::string getFullTypeName();

			virtual TClass* getClass();

		private:
			TGlobal *global;
	};

}

#endif
