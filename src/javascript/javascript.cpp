#include <duktape/duktape.h>
#include <duktape/duk_module_duktape.h>
#include <javascript/javascript.h>
#include <tinyfiledialog/tinyfiledialogs.h>
#include <json/json.h>
#include <network/httplib.h>
#include <render/render.h>
#include <serial/serial.h>
#include <gcode/gcode_parser.h>
#include <motion_control/motion_control.h>
#include <dxf/dxf.h>
#include <inih/inih.h>
#include <gui/gui.h>
#include <dirent.h>
#include <string>
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

using json = nlohmann::json;
serial::Serial my_serial;

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
/* Begin bindings */
#include <javascript/bindings/serial.bind>
#include <javascript/bindings/render.bind>
#include <javascript/bindings/live_render.bind>
#include <javascript/bindings/http.bind>
#include <javascript/bindings/gui.bind>
#include <javascript/bindings/file.bind>
#include <javascript/bindings/ini.bind>
#include <javascript/bindings/console.bind>
#include <javascript/bindings/classless.bind>
#include <javascript/bindings/window_menu.bind>
#include <javascript/bindings/file_dialog.bind>
#include <javascript/bindings/system.bind>
#include <javascript/bindings/time.bind>
#include <javascript/bindings/gcode.bind>
#include <javascript/bindings/dxf.bind>
#include <javascript/bindings/FastMath.bind>
#include <javascript/bindings/motion_control.bind>
/* End Bindings */ 
std::string Javascript::eval(std::string exp)
{
    duk_push_string(ctx, exp.c_str());
    if (duk_peval(ctx) != 0)
    {
        printf("[Javascript::eval] (%s) eval failed: %s\n",exp.c_str(), duk_safe_to_string(ctx, -1));
    }
    else
    {
        if (std::string(duk_safe_to_string(ctx, -1)) != "undefined")
        {
            printf("[scriptEval] result is: %s\n", duk_safe_to_string(ctx, -1));
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
    using namespace std::literals;
    using Clock = std::chrono::system_clock;
    auto t = Clock::now();
    this->init_timestamp_micros = t.time_since_epoch() / 1us;
    this->init_timestamp_millis = t.time_since_epoch() / 1ms;

    this->lines_available = true;
    this->window_open = false;
    this->loop = true;
    ctx = duk_create_heap_default();

    /* Register binidngs */
    classless_register_bindings();
    render_register_bindings();
    live_render_register_bindings();
    serial_register_bindings();
    http_register_bindings();
    ini_register_bindings();
    gui_register_bindings();
    window_menu_register_bindings();
    console_register_bindings();
    file_register_bindings();
    file_dialog_register_bindings();
    system_register_bindings();
    time_register_bindings();
    gcode_register_bindings();
    dxf_register_bindings();
    FastMath_register_bindings();
    motion_control_register_bindings();
}
void Javascript::bind(std::string name, duk_ret_t (*callback)(duk_context *ctx), int number_of_arguments)
{
    duk_push_c_function(ctx, callback, number_of_arguments /*nargs*/);
    duk_put_global_string(ctx, name.c_str());
}
void Javascript::bind_module(std::string name, const duk_function_list_entry function_list[])
{
    duk_push_global_object(ctx);
    duk_push_object(ctx);  /* -> [ ... global obj ] */
    duk_put_function_list(ctx, -1, function_list);
    duk_put_prop_string(ctx, -2, name.c_str());  /* -> [ ... global ] */
    duk_pop(ctx);
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