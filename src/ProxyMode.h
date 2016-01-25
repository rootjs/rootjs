#ifndef SRC_PROXYMODE_H_
#define SRC_PROXYMODE_H_
#include <RConfig.h>
namespace RootJS {

/**
 * This class encapsulates the differences in behaviour between TMember and
 * TGlobal
 * */
class ProxyMode {
  ProxyMode(TObject &current);
  virtual ~ProxyMode();

public:
  virtual bool isGlobal() = 0;
  virtual Long_t GetOffset() = 0;
  TObject getCurrentObject() { return currentObject; };
  TObject &currentObject;

protected:
private:
};
}

#endif /* SRC_PROXYMODE_H_ */
