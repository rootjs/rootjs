#ifndef SRC_POINTER_MODE_H_
#define SRC_POINTER_MODE_H_
#include "ProxyMode.h"

namespace rootJS {

	class PointerMode: public ProxyMode {
		public:
			PointerMode(void* baseAddr, const char* typeName);
			~PointerMode() {};
			virtual bool isGlobal() { return true; };
			virtual Long_t GetOffset() { return 0; };

			virtual bool isConst() { return true; };
			virtual bool isStatic() {
				return true;
			};
			virtual const char* getTypeName() {
				return typeName;
			};
		protected:
			const char *typeName;
	};
}

#endif /* SRC_POINTER_MODE_H_ */
