#ifndef NODE_HANDLER_H
#define NODE_HANDLER_H

#include <v8.h>
#include <node.h>

namespace RootJS {

  class NodeHandler {

    public:
      static void initialize(v8::Local<v8::Object>, v8::Local<v8::Object>);
  };

  NODE_MODULE(rootjs, NodeHandler::initialize);
}

#endif /* NODE_HANDLER_H */
