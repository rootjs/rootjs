#ifndef NODE_HANDLER_H
#define NODE_HANDLER_H

#include <queue>
#include <v8.h>
#include <node.h>
#include <TClassRef.h>
#include <map>
#include <queue>

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
			 * The exports object to be sent back to node
			 */
			v8::Local<v8::Object> exports;

			/* Private constructor - Singleton */
			NodeHandler(v8::Local<v8::Object>);


			std::queue<std::string> splitClassName (std::string);
			void exposeROOT();
			void exposeGlobalFunctions();
			void exposeGlobals();
			void exposeMacros();
			void exposeClasses();
			//TODO check if this method is still needed
			void exposeClass(TClassRef klass);
			v8::Local<v8::ObjectTemplate> exposeClassRec(TClass*,std::queue<std::string>&,std::map<TClass*,v8::Local<v8::ObjectTemplate>>&);
			//TODO move this to the TemplateFactory?
			v8::Local<v8::ObjectTemplate> createNamespace(TClass*,std::map<TClass*,v8::Local<v8::ObjectTemplate>>&);

		public:
			static void initialize(v8::Local<v8::Object>, v8::Local<v8::Object>);
			v8::Local<v8::Object> getExports(void);
	};

	NODE_MODULE(rootjs, NodeHandler::initialize);
}

#endif /* NODE_HANDLER_H */
