#ifndef SRC_FUNCTION_MODE_H_
#define SRC_FUNCTION_MODE_H_
#include <TFunction.h>
#include "ProxyMode.h"

namespace rootJS {

	class FunctionMode: public ProxyMode {
		public:
			FunctionMode(TFunction* func) : ProxyMode(TObject(), nullptr) {
				this->func = func;
			};
			~FunctionMode() {
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
			virtual ProxyMode* clone() {
				return new FunctionMode(func);
			};
		protected:
			TFunction* func;
	};
}

#endif /* SRC_POINTER_MODE_H_ */
