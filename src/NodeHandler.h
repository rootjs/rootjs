#ifndef NODE_HANDLER_H
#define NODE_HANDLER_H

#include "TemplateFactory.h"

#include <stdexcept>

#include <node.h>
#include <v8.h>

namespace rootJS
{
	/**
	 * The NodeHandler is the main entry point when you require rootJS
	 */
	class NodeHandler
	{
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


			void exposeROOT();
			void exposeGlobals() throw(std::invalid_argument);
			void exposeGlobalFunctions() throw(std::invalid_argument);
			void exposeMacros() throw(std::invalid_argument);
			void exposeClasses() throw(std::invalid_argument);
			void refreshExports();
		public:
			/**
			 * The method which starts rootJS. It instantiates a NodeApplication, a proxyMap from ObjectProxyFactory
             * and a NodeHandler. It also exposes the contents of ROOT.
             *
             * @param exports
             *              The exports sent back to node
             * @param module
             *              The module to be instantiated
			 */
			static void initialize(v8::Local<v8::Object>, v8::Local<v8::Object>);
			static void loadlibrary(const v8::FunctionCallbackInfo<v8::Value> &info) throw (std::invalid_argument);
			static NodeHandler* getInstance();

	};

	NODE_MODULE(rootjs, NodeHandler::initialize)
}

#endif
