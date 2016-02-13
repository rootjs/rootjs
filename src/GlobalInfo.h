#ifndef GLOBALINFO_H_
#define GLOBALINFO_H_

#include "MetaInfo.h"

#include <TGlobal.h>

namespace rootJS
{

	/**
	 * This class contains the info for a TGlobal
	 */
	class GlobalInfo: public MetaInfo
	{
		public:

			GlobalInfo(const TGlobal & type);
			~GlobalInfo();

			virtual Long_t GetOffset();

			virtual bool isArray();
			virtual bool isGlobal();
			virtual bool isConst();
			virtual bool isStatic();

			virtual int getArrayLength();
			virtual const char* getTypeName();
			virtual const char* getName();

			virtual GlobalInfo* clone();

		private:
			/** The type the GlobalInfo is holding.  */
			const TGlobal &type;
	};
}

#endif
