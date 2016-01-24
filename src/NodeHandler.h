#ifndef NODE_HANDLER_H
#define NODE_HANDLER_H

#include <v8.h>
#include <node.h>
#include <TClassRef.h>

namespace RootJS {

	class NodeHandler {
	private:
		/**
		 * Has the NodeHandler been initialized yet?
		 */
		static bool initialized;
		v8::Persistent<v8::Object> rootJS;
		/**
		 * Singleton holder for NodeHandler
		 */
		static NodeHandler *instance;
		/**
		 * THe exports object to be sent back to node
		 */
		v8::Local<v8::Object> exports;

		/* Private constructor - Singleton */
		NodeHandler(v8::Local<v8::Object>);

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
