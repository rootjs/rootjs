#ifndef SRC_PROXYMODE_H_
#define SRC_PROXYMODE_H_
#include <RConfig.h>
#include <TDataMember.h>
#include <TClassRef.h>
#include <TGlobal.h>
#include <RConfig.h>
namespace rootJS {

	/**
	 * This class encapsulates the differences in behaviour between TMember and
	 * TGlobal
	 * */
	class ProxyMode {
		protected:
			void* baseAddress;
		public:
			ProxyMode(const TObject &foo, void *baseAddres) {
				baseAddress = baseAddress;
			};
			virtual ~ProxyMode() {};

			virtual bool isGlobal() {
				return false;
			};
			virtual Long_t getOffset() {
				return 0;
			};
			virtual bool isConst() = 0;
			virtual bool isStatic() = 0;
			virtual const char* getTypeName() = 0;
			virtual void* getBaseAddress() {
				return baseAddress;
			};
			virtual void* getAddress() {
				return (void*)((char*)getBaseAddress() + getOffset());
			}

			virtual ProxyMode* clone() = 0;

		protected:
		private:
	};
}

#endif /* SRC_PROXYMODE_H_ */
