#include "AsyncRunner.h"

namespace rootJS
{
	AsyncRunner::AsyncRunner(AsyncFunction func, void *param, v8::Local<v8::Function> callback):
		func(func), param(param), callback(callback)
	{

	}

	void AsyncRunner::run()
	{
		uv_work_t *req = new uv_work_t();
		req->data = this;
		uv_queue_work(uv_default_loop(), req, uvRunner, uvCallback);
	}

	void AsyncRunner::uvRunner(uv_work_t *req)
	{
		AsyncRunner *runner = static_cast<AsyncRunner*>(req->data);
		(*(runner->func))(runner, runner->param);
	}

	void AsyncRunner::uvCallback(uv_work_t *req, int status)
	{
		v8::HandleScope scope(v8::Isolate::GetCurrent());
		AsyncRunner *runner = static_cast<AsyncRunner*>(req->data);
		v8::Isolate *isolate = v8::Isolate::GetCurrent();
		std::vector<v8::Local<v8::Value>> passParams;

		for(int i = 0; i < (int)runner->result.size(); i++) {
			if(!runner->result[i].isValid()) {
				continue;
			}
			ObjectProxy* proxy = runner->result[i].createObjectProxy();
			passParams.push_back(proxy->get());
		}

		v8::Local<v8::Function> cb = v8::Local<v8::Function>::New(isolate, runner->callback);
		if(runner->result.size()) {
			cb->Call(isolate->GetCurrentContext()->Global(), passParams.size(), &(passParams[0]));
		} else {
			cb->Call(isolate->GetCurrentContext()->Global(), 0, nullptr);
		}
	}
}
