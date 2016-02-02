#ifndef SRC_POINTER_INFO_H_
#define SRC_POINTER_INFO_H_
#include "MetaInfo.h"

namespace rootJS {

	class PointerInfo: public MetaInfo {
		public:
			PointerInfo(void* baseAddr, const char* typeName);
			~PointerInfo() {};
			virtual bool isGlobal() {
				return true;
			};
			virtual Long_t GetOffset() {
				return 0;
			};

			virtual bool isConst() {
				return true;
			};
			virtual bool isStatic() {
				return true;
			};
			virtual const char* getTypeName() {
				return typeName;
			};
			virtual MetaInfo* clone() {
				return new PointerInfo(baseAddress, typeName);
			};
		protected:
			const char *typeName;
	};
}

#endif /* SRC_POINTER_MODE_H_ */
