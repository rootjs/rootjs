#ifndef SRC_FUNCTION_INFO_H_
#define SRC_FUNCTION_INFO_H_
#include <TFunction.h>

#include "MetaInfo.h"

namespace rootJS {

	class FunctionInfo: public MetaInfo {
		public:
			FunctionInfo(TFunction* func) : MetaInfo(TObject(), nullptr) {
				this->func = func;
			};
			~FunctionInfo() {
			};
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
				return func->GetName();
			};
			virtual MetaInfo* clone() {
				return new FunctionInfo(func);
			};
		protected:
			TFunction* func;
	};
}

#endif /* SRC_POINTER_MODE_H_ */
