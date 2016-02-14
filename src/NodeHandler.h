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
			v8::Persistent<v8::Object> exportPersistent;
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

			/**
			 * This method exposes all the contents of the given TClass so that rootJS has access to the contents.
			 * The namespace hierarchy is preserved and mapped to the exported node.js object. Namespaces that don"t
			 * have a TClass are generated as stubs and inserted at the appropriate position.
			 *
			 * @param clazz
			 *              The class which will be exposed
			 * @param exports
			 *              The exports Local<Object> will be examined to see if the class has already has been exposed.
			 *              If it hasn't been exposed, then the ClassExposer will expose the contents of the class.
			 */
			void exposeHierarchy(TClass*,v8::Local<v8::Object>) throw(std::invalid_argument);
			std::vector<std::string> splitClassName(std::string name, std::vector<std::string>& vec);
			void exposeROOT();
			
			void exposeGlobals() throw(std::invalid_argument);
			void exposeGlobalFunctions() throw(std::invalid_argument);
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

			/**
			* CallbackMethod for user initiated library loading.
			* the node method forwards the int return value of gSystem->Load()
			*
			* @param info
			*              the arguments passed in node.js
			*
			*/
			static void loadlibraryCallback(const v8::FunctionCallbackInfo<v8::Value> &info) throw (std::invalid_argument);

			/**
			* CallbackMethod for user initiated refreshing of the exposed functions
			*
			* @param info
			*              the arguments passed in node.js (expected are none)
			*
			*/
			static void refreshExportsCallback(const v8::FunctionCallbackInfo<v8::Value> &info) throw (std::invalid_argument);
			static void unloadlibraryCallback(const v8::FunctionCallbackInfo<v8::Value> &info) throw (std::invalid_argument);
			static NodeHandler* getInstance();

			void exposeInterfaceFunctions();
	};

	NODE_MODULE(rootjs, NodeHandler::initialize)
}

#endif
