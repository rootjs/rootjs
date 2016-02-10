#ifndef SRC_GLOBALINFO_H_
#define SRC_GLOBALINFO_H_
#include "MetaInfo.h"

namespace rootJS {

	/**
	 * This class contains the info for a TGlobal
	 */
	class GlobalInfo: public MetaInfo {
		public:
			GlobalInfo(const TGlobal & type);
			~GlobalInfo();
			virtual bool isGlobal();
			virtual Long_t GetOffset();

			virtual bool isConst();
			virtual bool isStatic() {
				return true;
			};
			virtual const char* getTypeName();

			/** The type the GlobalInfo is holding.  */
			const TGlobal &currentObject;

			virtual MetaInfo* clone() {
				return new GlobalInfo(currentObject);
			};
		protected:
	};
}

#endif /* SRC_GLOBALINFO_H_ */
