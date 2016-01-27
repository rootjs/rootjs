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

	public:
		ProxyMode(const TObject &foo);
		virtual ~ProxyMode();

		virtual bool isGlobal();
		virtual Long_t GetOffset();

	protected:
	private:
	};
}

#endif /* SRC_PROXYMODE_H_ */
