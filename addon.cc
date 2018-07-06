#include <node_api.h>
#include <assert.h>
#include<stdio.h>
#if defined _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

typedef struct wait_async{
  napi_deferred deferred;
  napi_async_work async_work;
}wait_async;

napi_value addSync(napi_env env, napi_callback_info info)
{
  size_t argc = 2;
  napi_value args[2];
  napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

  if (argc < 2)
  {
    napi_throw_type_error(env, nullptr, "Wrong number of arguments");
  }

  napi_valuetype args0type;
  napi_typeof(env, args[0], &args0type);

  napi_valuetype args1type;
  napi_typeof(env, args[1], &args1type);

  if (args0type != napi_number || args1type != napi_number)
  {
    napi_throw_type_error(env, nullptr, "Wrong arguments type");
  }

  double value0, value1;
  napi_get_value_double(env, args[0], &value0);
  napi_get_value_double(env, args[1], &value1);

  napi_value sum;
  napi_create_double(env, value0 + value1, &sum);
  return sum;
}

void executeWait(napi_env env, void *data)
{
  #if defined _WIN32
    Sleep(1000);
  #else
    sleep(1);
  #endif
}

void completeWait(napi_env env, napi_status status, void *data)
{
  napi_value undefined;
  napi_get_undefined(env, &undefined);
  wait_async* c = (wait_async*)data;
  if(!c){
    printf("Null pointer");
  }
  napi_resolve_deferred(env, c->deferred, undefined);
  napi_delete_async_work(env, c->async_work);
  free(c);
}

napi_value wait(napi_env env, napi_callback_info)
{
  napi_value promise;
  napi_value resource_name;

  wait_async* data = (wait_async*)malloc(sizeof(wait_async));

  napi_create_promise(env, &data->deferred, &promise);

  napi_create_string_utf8(env, "waitAsync", 9,  &resource_name);
  napi_create_async_work(env, nullptr, resource_name, executeWait, completeWait, data, &data->async_work);
  napi_queue_async_work(env, data->async_work);

  return promise;
}

napi_value Init(napi_env env, napi_value exports)
{
  napi_status status;
  napi_property_descriptor descs[] = {
      {"addSync", 0, addSync, 0, 0, 0, napi_default, 0},
      {"wait", 0, wait, 0, 0, 0, napi_default, 0},
  };
  status = napi_define_properties(env, exports, sizeof(descs) / sizeof(descs[0]), descs);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)