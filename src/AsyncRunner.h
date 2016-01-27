#ifndef ASYNC_RUNNER_H
#define ASYNC_RUNNER_H

#include <vector>
#include <v8.h>
#include <uv.h>

namespace rootJS {

	class AsyncRunner {
	typedef void (*AsyncFunction)(AsyncRunner* runner, void* param);
	private:
		AsyncFunction *func;
		void *param;
		v8::Local<v8::Function> callback;
		std::vector<v8::Local<v8::Value>> result;

	public:
		AsyncRunner(AsyncFunction *func, void *param, v8::Local<v8::Function> callback);
		void run();
		static void uvRunner(uv_work_t *req);
		static void uvCallback(uv_work_t *req, int status);
	};
}

#endif /* ASYNC_RUNNER_H */
