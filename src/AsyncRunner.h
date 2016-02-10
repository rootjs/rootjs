#ifndef ASYNC_RUNNER_H
#define ASYNC_RUNNER_H

#include "ObjectProxy.h"

#include <vector>
#include <v8.h>
#include <uv.h>

namespace rootJS {

	/**
	 * The AsyncRunner provides methods to enable asyncronous function execution.
	 *
	 */
	class AsyncRunner {
			typedef void (*AsyncFunction)(AsyncRunner* runner, void* param);
		private:
			AsyncFunction func;
			void *param;
			v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> callback;
			std::vector<ObjectProxy*> result;

		public:
			/**
			 * Creates a new AsyncRunner.
			 *
			 * @param func The function to be called asynchronously.
			 * @param *param The parameters for the function.
			 * @param callback The callback to be executed in the node thread after the 
			 * 		asynchronous function is finished.
			 */
			AsyncRunner(AsyncFunction func, void *param, v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> callback);

			/**
			 * Runs the function given with the constructor asynchronously. 
			 * After it has finished, the callback function is executed in the original node thread.
			 */
			void run();

			/**
			 * Allows the asynchronous function to set its result.
			 *
			 * @param result The result of the asynchronous function.
			 */
			void setResult(std::vector<ObjectProxy*> result) {
				this->result = result;
			};

			/**
			 * This needs to be implemented for libuv.
			 *
			 * @param *req A libuv request.
			 */
			static void uvRunner(uv_work_t *req);

			/**
			 * Executes the callback function.
			 * 
			 * @param *req The libuv request, used to get context.	
			 * @param status If the callback is canceled with uv_cancel() this is UV_ECANCELED.
			 */
			static void uvCallback(uv_work_t *req, int status);
	};
}

#endif /* ASYNC_RUNNER_H */
