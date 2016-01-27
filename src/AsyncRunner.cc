#include "AsyncRunner.h"

namespace rootJS {
	AsyncRunner::AsyncRunner(AsyncFunction *func, void *param, v8::Local<v8::Function> callback):
		func(func), param(param), callback(callback) {

	}

	void AsyncRunner::run() {
		uv_work_t *req = new uv_work_t();
		uv_queue_work(uv_default_loop(), req, uvRunner, uvCallback);
	}

	void AsyncRunner::uvRunner(uv_work_t *req) {
		AsyncRunner *runner = static_cast<AsyncRunner*>(req->data);
		(*(runner->func))(runner, runner->param);
	}

	void AsyncRunner::uvCallback(uv_work_t *req, int status) {
		AsyncRunner *runner = static_cast<AsyncRunner*>(req->data);
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		if(runner->result.size()) {
			runner->callback->Call(isolate->GetCurrentContext()->Global(), runner->result.size(), &(runner->result[0]));
		} else {
			runner->callback->Call(isolate->GetCurrentContext()->Global(), 0, nullptr);
		}
	}
}
