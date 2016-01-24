#ifndef SRC_PROXYMODE_H_
#define SRC_PROXYMODE_H_

namespace RootJS {

/**
 * This class encapsulates the differences in behavoiur between TMember and TGlobal
 * */
class ProxyMode {
	ProxyMode(TObject& current);
	virtual ~ProxyMode();
public:
	virtual bool isGlobal() = 0;
	TObject getCurrentObject() {return currentObject;};
protected:
private:
	TObject currentObject;

};
}

#endif /* SRC_PROXYMODE_H_ */

