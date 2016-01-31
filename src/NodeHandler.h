#ifndef NODE_HANDLER_H
#define NODE_HANDLER_H

#include <stack>
#include <v8.h>
#include <node.h>
#include <TClassRef.h>

namespace rootJS {

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
			std::stack<std::string> splitClassName (std::string);

			void exposeROOT();
			void exposeGlobalFunctions();
			void exposeGlobals();
			void exposeMacros();
			void exposeClasses();
			//TODO check if this method is still needed
			void exposeClass(TClassRef klass);
			v8::Local<v8::FunctionTemplate> exposeClassRec(std::string,std::stack<std::string>&);

		public:
			static void initialize(v8::Local<v8::Object>, v8::Local<v8::Object>);
			v8::Local<v8::Object> getExports(void);
	};

	NODE_MODULE(rootjs, NodeHandler::initialize);
}

#endif /* NODE_HANDLER_H */
