#include <duktape/duktape.h>
#include <duktape/duk_module_duktape.h>
#include <javascript/javascript.h>
#include <network/httplib.h>
#include <render/render.h>
#include <serial/serial.h>
#include <inih/inih.h>
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
    js.window_open = true;   
    return 0;  /* no return value (= undefined) */
}
static duk_ret_t exit(duk_context *ctx) {
    js.loop = false;
    js.ret = duk_to_int(ctx, 0);
    return 0;  /* no return value (= undefined) */
}
static duk_ret_t serial_list_ports(duk_context *ctx) {
    duk_idx_t arr_idx = duk_push_array(ctx);
    duk_idx_t obj_idx;
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
static duk_ret_t http_get(duk_context *ctx) {
    duk_idx_t obj_idx;
    httplib::Client cli(duk_to_string(ctx, 0), duk_to_int(ctx, 1));
    auto res = cli.Get(duk_to_string(ctx, 2));
    if (res != NULL)
    {
        if (res && res->status == 200)
        {
            obj_idx = duk_push_object(ctx);
            duk_push_int(ctx, res->status);
            duk_put_prop_string(ctx, obj_idx, "status");
            duk_push_string(ctx, res->body.c_str());
            duk_put_prop_string(ctx, obj_idx, "body");
        }
        else
        {
            obj_idx = duk_push_object(ctx);
            duk_push_int(ctx, res->status);
            duk_put_prop_string(ctx, obj_idx, "status");
        }
    }
    else
    {
        obj_idx = duk_push_object(ctx);
        duk_push_string(ctx, "network error");
        duk_put_prop_string(ctx, obj_idx, "status");
    }
    return 1;  /* no return value (= undefined) */
}
static duk_ret_t ini_get(duk_context *ctx) {
    duk_idx_t obj_idx;
    INIReader reader(duk_to_string(ctx, 0));
    if (reader.ParseError() != 0) {
        obj_idx = duk_push_object(ctx);
        duk_push_string(ctx, "failed to read file");
        duk_put_prop_string(ctx, obj_idx, "status");
        duk_push_string(ctx, duk_to_string(ctx, 0));
        duk_put_prop_string(ctx, obj_idx, "file");
        return 1;
    }
    obj_idx = duk_push_object(ctx);
    duk_push_string(ctx, "ok");
    duk_put_prop_string(ctx, obj_idx, "status");
    duk_push_string(ctx, duk_to_string(ctx, 0));
    duk_put_prop_string(ctx, obj_idx, "file");
    duk_push_string(ctx, reader.Get(duk_to_string(ctx, 1), duk_to_string(ctx, 2), duk_to_string(ctx, 3)).c_str());
    duk_put_prop_string(ctx, obj_idx, "value");
    return 1;  /* no return value (= undefined) */
}
static duk_ret_t show_view_controls(duk_context *ctx) {
    gui.show_view_controls = duk_to_boolean(ctx, 0);
    return 0;  /* no return value (= undefined) */
}
static duk_ret_t render_show_crosshair(duk_context *ctx) {
    renderer.show_crosshair = duk_to_boolean(ctx, 0);
    return 0;  /* no return value (= undefined) */
}
static duk_ret_t gui_new_window(duk_context *ctx) {
    window_t w;
    w.title = duk_to_string(ctx, 0);
    w.visable = true;
    int i = gui.windows.size();
    gui.windows.push_back(w);
    duk_push_int(ctx, i);
    return 1;  /* 1 return value (= undefined) */
}
static duk_ret_t gui_window_add_text(duk_context *ctx) {
    int ret = -1;
    int window_id = duk_to_int(ctx, 0);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        window_text_t t;
        t.text = duk_to_string(ctx, 1);
        window_element_t e;
        e.type = element_types::element_text;
        e.text = t;
        ret = gui.windows[window_id].elements.size();
        gui.windows[window_id].elements.push_back(e);
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_set_text(duk_context *ctx) {
    int ret = -1;
    int window_id = duk_to_int(ctx, 0);
    int text_id = duk_to_int(ctx, 1);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        if (text_id < gui.windows[window_id].elements.size())
        {
            gui.windows[window_id].elements[text_id].text.text = std::string(duk_to_string(ctx, 2));
        }
    }
    duk_push_int(ctx, ret);
    return 0;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_add_checkbox(duk_context *ctx) {
    int ret = -1;
    int window_id = duk_to_int(ctx, 0);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        window_checkbox_t w;
        w.text = duk_to_string(ctx, 1);
        w.value = duk_to_boolean(ctx, 2);
        window_element_t e;
        e.type = element_types::element_checkbox;
        e.checkbox = w;
        ret = gui.windows[window_id].elements.size();
        gui.windows[window_id].elements.push_back(e);
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_get_checkbox(duk_context *ctx) {
    int ret = -1;
    int window_id = duk_to_int(ctx, 0);
    int widget_id = duk_to_int(ctx, 1);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        if (widget_id < gui.windows[window_id].elements.size())
        {
            ret = gui.windows[window_id].elements[widget_id].checkbox.value;
        }
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_add_slider(duk_context *ctx) {
    int ret = -1;
    int window_id = duk_to_int(ctx, 0);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        window_slider_t w;
        w.text = duk_to_string(ctx, 1);
        w.value = duk_to_number(ctx, 2);
        w.min = (float)duk_to_number(ctx, 3);
        w.max = (float)duk_to_number(ctx, 4);
        window_element_t e;
        e.type = element_types::element_slider;
        e.slider = w;
        ret = gui.windows[window_id].elements.size();
        gui.windows[window_id].elements.push_back(e);
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_get_slider(duk_context *ctx) {
    float ret = -1;
    int window_id = duk_to_int(ctx, 0);
    int widget_id = duk_to_int(ctx, 1);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        if (widget_id < gui.windows[window_id].elements.size())
        {
            ret = gui.windows[window_id].elements[widget_id].slider.value;
        }
    }
    duk_push_number(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_add_button(duk_context *ctx) {
    int ret = -1;
    int window_id = duk_to_int(ctx, 0);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        window_button_t w;
        w.text = std::string(duk_to_string(ctx, 1));
        w.value = false;
        window_element_t e;
        e.type = element_types::element_button;
        e.button = w;
        ret = gui.windows[window_id].elements.size();
        gui.windows[window_id].elements.push_back(e);
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_get_button(duk_context *ctx) {
    int ret = -1;
    int window_id = duk_to_int(ctx, 0);
    int widget_id = duk_to_int(ctx, 1);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        if (widget_id < gui.windows[window_id].elements.size())
        {
            ret = gui.windows[window_id].elements[widget_id].button.value;
        }
    }
    duk_push_number(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t window_create_menu(duk_context *ctx) {
    int ret = -1;
    menu_t m;
    m.title = std::string(duk_to_string(ctx, 0));
    ret = gui.menu.size();
    gui.menu.push_back(m);
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t window_add_menu_button(duk_context *ctx) {
    int ret = -1;
    int menu_id = duk_to_int(ctx, 0);
    if (menu_id < gui.menu.size())
    {
        menu_item_t item;
        item.type = menu_types::menu_button;
        item.button.text = std::string(duk_to_string(ctx, 1));
        item.button.value = false;
        ret = gui.menu[menu_id].items.size();
        gui.menu[menu_id].items.push_back(item);
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t window_get_menu_button(duk_context *ctx) {
    int ret = -1;
    int menu_id = duk_to_int(ctx, 0);
    int item_id = duk_to_int(ctx, 1);
    if (menu_id < gui.menu.size()) //Make sure this window exists!
    {
        if (item_id < gui.menu[menu_id].items.size())
        {
            ret = gui.menu[menu_id].items[item_id].button.value;
            if (ret == true)
            {
                gui.menu[menu_id].items[item_id].button.value = false;
            }
        }
    }
    duk_push_number(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t window_add_menu_checkbox(duk_context *ctx) {
    int ret = -1;
    int menu_id = duk_to_int(ctx, 0);
    if (menu_id < gui.menu.size())
    {
        menu_item_t item;
        item.type = menu_types::menu_checkbox;
        item.checkbox.text = std::string(duk_to_string(ctx, 1));
        item.checkbox.value = duk_to_boolean(ctx, 2);
        ret = gui.menu[menu_id].items.size();
        gui.menu[menu_id].items.push_back(item);
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t window_get_menu_checkbox(duk_context *ctx) {
    int ret = -1;
    int menu_id = duk_to_int(ctx, 0);
    int item_id = duk_to_int(ctx, 1);
    if (menu_id < gui.menu.size()) //Make sure this window exists!
    {
        if (item_id < gui.menu[menu_id].items.size())
        {
            ret = gui.menu[menu_id].items[item_id].checkbox.value;
        }
    }
    duk_push_number(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
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
    this->window_open = false;
    this->loop = true;
    
    ctx = duk_create_heap_default();
    
    bind("print", print, 1);

    bind("include", include, 1);

    bind("create_window", create_window, 3);

    bind("exit", exit, 1);

    bind("serial_list_ports", serial_list_ports, 0);

    bind("http_get", http_get, 3);

    bind("ini_get", ini_get, 4);

    bind("show_view_controls", show_view_controls, 1);

    bind("render_show_crosshair", render_show_crosshair, 1);

    bind("gui_new_window", gui_new_window, 1);
    bind("gui_window_add_text", gui_window_add_text, 2);
    bind("gui_window_set_text", gui_window_set_text, 3);
    bind("gui_window_add_checkbox", gui_window_add_checkbox, 3);
    bind("gui_window_get_checkbox", gui_window_get_checkbox, 2);
    bind("gui_window_add_slider", gui_window_add_slider, 5);
    bind("gui_window_get_slider", gui_window_get_slider, 2);
    bind("gui_window_add_button", gui_window_add_button, 2);
    bind("gui_window_get_button", gui_window_get_button, 2);

    bind("window_create_menu", window_create_menu, 1);
    bind("window_add_menu_button", window_add_menu_button, 2);
    bind("window_get_menu_button", window_get_menu_button, 2);
    bind("window_add_menu_checkbox", window_add_menu_checkbox, 3);
    bind("window_get_menu_checkbox", window_get_menu_checkbox, 2);
    duk_module_duktape_init(ctx);
}
void Javascript::bind(std::string name, duk_ret_t (*callback)(duk_context *ctx), int number_of_arguments)
{
    duk_push_c_function(ctx, callback, number_of_arguments /*nargs*/);
    duk_put_global_string(ctx, name.c_str());
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