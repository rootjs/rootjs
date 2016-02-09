#ifndef ASYNC_RUNNER_H
#define ASYNC_RUNNER_H

#include "ObjectProxy.h"

#include <vector>
#include <v8.h>
#include <uv.h>

namespace rootJS {

	class AsyncRunner {
			typedef void (*AsyncFunction)(AsyncRunner* runner, void* param);
		private:
			AsyncFunction func;
			void *param;
			v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> callback;
			std::vector<ObjectProxy*> result;

		public:
			AsyncRunner(AsyncFunction func, void *param, v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> callback);
			void run();
			void setResult(std::vector<ObjectProxy*> result) {
				this->result = result;
			};
			static void uvRunner(uv_work_t *req);
			static void uvCallback(uv_work_t *req, int status);
	};
}

#endif /* ASYNC_RUNNER_H */
