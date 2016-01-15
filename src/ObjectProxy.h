
#ifndef SRC_OBJECTPROXY_H_
#define SRC_OBJECTPROXY_H_

#include "Proxy.h"

#include <v8.h>
#include <TObject.h>

namespace RootJS {

class ObjectProxy : public Proxy {
public:
	ObjectProxy();
  ObjectProxy(const TObject * object);
	~ObjectProxy();

  v8::Local<v8::Object> get(void);

  virtual bool isTemplate();
  virtual bool isGlobal();
  virtual bool isConst();
  virtual bool isStatic();
};

}

#endif /* SRC_OBJECTPROXY_H_ */
