#include "NodeHandler.h"

namespace RootJS {
  void NodeHandler::initialize(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
    if(!initialized) {
      instance = new NodeHandler(exports);
      instance->exposeROOT();
    } else {
      v8::ThrowException(v8::String::New("The NodeHandler can only be initialized once."));
    }
  }

  NodeHandler::NodeHandler(v8::Local<v8::Object> exports) {
    this->exports = exports;
  }

  void NodeHandler::exposeROOT() {
    exposeGlobals();
  }

  void NodeHandler::exposeGlobals() {
    
  }
}
