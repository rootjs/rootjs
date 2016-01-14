#ifndef NODE_HANDLER_H
#define NODE_HANDLER_H

#include <v8.h>
#include <node.h>
#include <TClassRef.h>

namespace RootJS {

  class NodeHandler {
  private:
    bool initialized;
    v8::Persistent<v8::Object> rootJS;
    NodeHandler instance;
    v8::Local<v8::Object> exports;

    /* Private constructor - Singleton */
    NodeHandler(void);

    void exposeROOT();
    void exposeGlobalFunctions();
    void exposeGlobals();
    void exposeMacros();
    void exposeClasses();
    void exposeClass(TClassRef klass);

  public:
    static void initialize(v8::Local<v8::Object>, v8::Local<v8::Object>);
    v8::Local<v8::Object> getExports(void);
  };

  NODE_MODULE(rootjs, NodeHandler::initialize);
}

#endif /* NODE_HANDLER_H */
