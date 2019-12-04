#include <duktape/duktape.h>
#include <duktape/duk_module_duktape.h>
#include <javascript/javascript.h>
#include <render/render.h>
#include <serial/serial.h>
#include <gui/gui.h>
#include <string>


static void push_file_as_string(duk_context *ctx, const char *filename) {
    FILE *f;
    size_t len;
    char buf[16384];

    f = fopen(filename, "rb");
    if (f) {
        len = fread((void *) buf, 1, sizeof(buf), f);
        fclose(f);
        duk_push_lstring(ctx, (const char *) buf, (duk_size_t) len);
    } else {
        duk_push_undefined(ctx);
    }
}
/* Javascript binding functions */
static duk_ret_t print(duk_context *ctx) {
  printf("%s", duk_to_string(ctx, 0));
  return 0;  /* no return value (= undefined) */
}
static duk_ret_t include(duk_context *ctx) {
  push_file_as_string(ctx, duk_to_string(ctx, 0));
  if (duk_peval(ctx) != 0) {
    printf("[Javascript::include] Error: %s\n", duk_safe_to_string(ctx, -1));
  }
  duk_pop(ctx);  /* ignore result */
  return 0;  /* no return value (= undefined) */
}
static duk_ret_t create_window(duk_context *ctx) {
    //printf("%s", duk_to_string(ctx, 0));
    if (!glfwInit()) return 1;
    renderer.window = glfwCreateWindow(duk_to_int(ctx, 0), duk_to_int(ctx, 1), duk_to_string(ctx, 2), NULL, NULL);
    if (renderer.window == NULL) return 1;
    glfwMakeContextCurrent(renderer.window);
    glfwSwapInterval(1); // Enable vsync
    renderer.init();
    gui.init(renderer.window);   
    return 0;  /* no return value (= undefined) */
}
static duk_ret_t close_window(duk_context *ctx) {
    js.loop = false;  
    return 0;  /* no return value (= undefined) */
}
static duk_ret_t serial_list_ports(duk_context *ctx) {
    duk_idx_t arr_idx = duk_push_array(ctx);
    duk_idx_t obj_idx = NULL;
    std::vector<serial::PortInfo> devices_found = serial::list_ports();
	std::vector<serial::PortInfo>::iterator iter = devices_found.begin();
    int count = 0;
	while( iter != devices_found.end() )
	{
		serial::PortInfo device = *iter++;
		//printf( "(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(), device.hardware_id.c_str() );
        obj_idx = duk_push_object(ctx);
        duk_push_string(ctx, device.port.c_str());
        duk_put_prop_string(ctx, obj_idx, "port");
        duk_push_string(ctx, device.description.c_str());
        duk_put_prop_string(ctx, obj_idx, "description");
        duk_push_string(ctx, device.hardware_id.c_str());
        duk_put_prop_string(ctx, obj_idx, "hardware_id");
        duk_put_prop_index(ctx, arr_idx, count);
        count++;
	}
    return 1;  /* no return value (= undefined) */
}
/* End Javascript binding functions */
std::string Javascript::eval(std::string exp)
{
    duk_push_string(ctx, exp.c_str());
    if (duk_peval(ctx) != 0)
    {
        printf("[Javascript::eval] (%s) eval failed: %s\n",exp.c_str(), duk_safe_to_string(ctx, -1));
    }
    else
    {
        if (!strcmp(duk_safe_to_string(ctx, -1), "undefined") == 0)
        {
        //makV printf("[scriptEval] result is: %s\n", duk_safe_to_string(ctx, -1));
        }
    }
	std::string ret = std::string(duk_get_string(ctx, -1));
    duk_pop(ctx);  /* pop result */
	return ret;
}
void Javascript::eval_file(std::string file)
{
    push_file_as_string(ctx, file.c_str());
    if (duk_peval(ctx) != 0) {
        printf("[Javascript::eval_file] Error: %s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);  /* ignore result */
}
void Javascript::init()
{
    //printf("*INIT* Javascript Engine!\n");
    ctx = duk_create_heap_default();
    
    duk_push_c_function(ctx, print, 1 /*nargs*/);
    duk_put_global_string(ctx, "print");

    duk_push_c_function(ctx, include, 1 /*nargs*/);
    duk_put_global_string(ctx, "include");

    duk_push_c_function(ctx, create_window, 3 /*nargs*/);
    duk_put_global_string(ctx, "create_window");

    duk_push_c_function(ctx, close_window, 3 /*nargs*/);
    duk_put_global_string(ctx, "close_window");

    duk_push_c_function(ctx, serial_list_ports, 0 /*nargs*/);
    duk_put_global_string(ctx, "serial_list_ports");

    duk_module_duktape_init(ctx);
}
void Javascript::destroy()
{
    duk_destroy_heap(ctx);
}
void Javascript::refresh()
{
    if (ctx != NULL)
    {
        destroy();
    }
    init();
}