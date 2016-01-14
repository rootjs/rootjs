#include "NodeHandler.h"

namespace RootJS {

  NodeHandler *NodeHandler::instance;  
  bool NodeHandler::initialized;

  void NodeHandler::initialize(v8::Local<v8::Object> exports, v8::Local<v8::Object> module) {
    if(!initialized) {
      instance = new NodeHandler(exports);
      instance->exposeROOT();
    } else {
      v8::Isolate::GetCurrent()->ThrowException(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "The NodeHandler can only be initialized once."));
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
