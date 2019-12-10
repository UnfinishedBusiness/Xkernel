#include <duktape/duktape.h>
#include <duktape/duk_module_duktape.h>
#include <javascript/javascript.h>
#include <json/json.h>
#include <network/httplib.h>
#include <render/render.h>
#include <serial/serial.h>
#include <inih/inih.h>
#include <gui/gui.h>
#include <dirent.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

using json = nlohmann::json;
serial::Serial *my_serial;

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
static duk_ret_t serial_open(duk_context *ctx) {
    bool ret;
    std::string port = duk_to_string(ctx, 0);
    int baud = duk_to_int(ctx, 1);
    serial::Serial my_s(port, baud, serial::Timeout::simpleTimeout(1000));
    my_serial = &my_s;
    if (my_serial->isOpen())
    {
        ret = true;
    }
    else
    {
        ret = false;
    }
    duk_push_boolean(ctx, ret);
    return 1;  /* no return value (= undefined) */
}
static duk_ret_t serial_is_open(duk_context *ctx) {
    bool ret;
    if (my_serial != NULL)
    {
        if (my_serial->isOpen())
        {
            ret = true;
        }
        else
        {
            ret = false;
        }
    }
    else
    {
        ret = false;
    }
    duk_push_boolean(ctx, ret);
    return 1;  /* no return value (= undefined) */
}
static duk_ret_t serial_available(duk_context *ctx) {
    bool ret;
    if (my_serial != NULL)
    {
        if (my_serial->available())
        {
            ret = true;
        }
        else
        {
            ret = false;
        }
    }
    else
    {
        ret = false;
    }
    duk_push_boolean(ctx, ret);
    return 1;  /* no return value (= undefined) */
}
static duk_ret_t serial_read(duk_context *ctx) {
    std::string ret;
    if (my_serial != NULL)
    {
        if (my_serial->isOpen())
        {
            ret = my_serial->read(1);
        }
        else
        {
            ret = "";
        }
    }
    else
    {
        ret = "";
    }
    duk_push_string(ctx, ret.c_str());
    return 1;  /* no return value (= undefined) */
}
static duk_ret_t serial_write(duk_context *ctx) {
    if (my_serial != NULL)
    {
        if (my_serial->isOpen()) my_serial->write(std::string(duk_to_string(ctx, 0)));
    }
    return 0;  /* no return value (= undefined) */
}
static duk_ret_t serial_close(duk_context *ctx) {
    if (my_serial != NULL)
    {
        my_serial->close();
    }
    my_serial = NULL;
    return 0;  /* no return value (= undefined) */
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
    duk_to_object(ctx, 0);
    nlohmann::json j = json::parse(duk_json_encode(ctx, -1));
    for (auto& [key, value] : j.items())
    {
        //std::cout << key << " : " << value << "\n";
        if (key == "visable")
        {
            renderer.show_crosshair = value;
        }
        if (key == "pos")
        {
            for (auto& [sub_key, sub_value] : value.items())
            {
                //std::cout << "\t" << sub_key << " : " << sub_value << "\n";
                if (sub_key == "x")
                {
                    if (sub_value.is_null())
                    {
                        renderer.crosshair_pos.x = 0;
                    }
                    else
                    {
                        renderer.crosshair_pos.x = sub_value;
                    }
                }
                if (sub_key == "y")
                {
                    if (sub_value.is_null())
                    {
                        renderer.crosshair_pos.y = 0;
                    }
                    else
                    {
                        renderer.crosshair_pos.y = sub_value;
                    }
                }
                if (sub_key == "z")
                {
                    if (sub_value.is_null())
                    {
                        renderer.crosshair_pos.z = 0;
                    }
                    else
                    {
                        renderer.crosshair_pos.z = sub_value;
                    }
                }
            }
        }
    }
    //printf("visable: %d, pos_x: %.4f, pos_y: %.4f, pos_z: %.4f\n", renderer.show_crosshair, renderer.crosshair_pos.x, renderer.crosshair_pos.y, renderer.crosshair_pos.z);
    return 0;  /* no return value (= undefined) */
}
static duk_ret_t render_get_mouse(duk_context *ctx) {
    nlohmann::json j;
    glm::vec2 mouse_pos = renderer.get_mouse_in_world_coordinates();
    j["x"] = mouse_pos.x;
    j["y"] = mouse_pos.y;
    duk_push_string(ctx, j.dump().c_str());
    duk_json_decode(ctx, -1);
    return 1; 
}
static duk_ret_t render_add_entity(duk_context *ctx) {
    duk_to_object(ctx, 0);
    int count = 0;
    std::string type;
    glm::vec3 start;
    glm::vec3 end;
    //printf("JSON encoded: %s\n", duk_json_encode(ctx, -1));
    std::string json = duk_json_encode(ctx, -1);
    nlohmann::json arg = json::parse(json);
    //std::string type_string = arg.value("type", "none");
    //printf("Type: %s\n", type_string.c_str());

    std::string entity_type = arg["type"];
    if (entity_type == "line")
    {
        if (arg.find("start") != arg.end())
        {
            start.x = arg.at("start").value("x", 0.0f);
            start.y = arg.at("start").value("y", 0.0f);
            start.z = arg.at("start").value("z", 0.0f);
        }
        else
        {
            start.x = 0.0f;
            start.y = 0.0f;
            start.z = 0.0f;
        }
        if (arg.find("end") != arg.end())
        {
            end.x = arg.at("end").value("x", 0.0f);
            end.y = arg.at("end").value("y", 0.0f);
            end.z = arg.at("end").value("z", 0.0f);
        }
        else
        {
            end.x = 0.0f;
            end.y = 0.0f;
            end.z = 0.0f;
        }
        entity_t e;
        e.type = entity_types::entity_line;
        if (arg.find("color") != arg.end())
        {
            e.color.x = arg.at("color").value("r", 1.0f);
            e.color.y = arg.at("color").value("g", 0.0f);
            e.color.z = arg.at("color").value("b", 0.0f);
        }
        else
        {
            e.color.x = 1.0f;
            e.color.y = 0.0f;
            e.color.z = 0.0f;
        }
        e.visable = arg.value("visable", true);
        e.layer = arg.value("layer", "default");
        line_t l;
        l.start = start;
        l.end = end;
        e.line = l;
        //printf("startX: %.4f, startY: %.4f, startZ: %.4f, endX: %.4f, endY: %.4f, endZ: %.4f, color.r: %.4f, color.g: %.4f, color.b: %.4f, visable: %d, layer: %s\n", l.start.x, l.start.y, l.start.z, l.end.x, l.end.y, l.end.z, e.color.x, e.color.y, e.color.y, e.visable, e.layer.c_str());
        renderer.entity_stack.push_back(e);
    }
    return 0; 
}
static duk_ret_t render_get_entity(duk_context *ctx) {
    int index = duk_to_int(ctx, 0);
    if (index < renderer.entity_stack.size())
    {
        nlohmann::json entity_json;
        switch (renderer.entity_stack[index].type)
        {
            case entity_types::entity_line:{
                entity_json["type"] = "line";
                entity_json["visable"] = renderer.entity_stack[index].visable;
                entity_json["layer"] = renderer.entity_stack[index].layer;
                entity_json["color"] = {{"r", renderer.entity_stack[index].color.x}, {"g", renderer.entity_stack[index].color.y}, {"b", renderer.entity_stack[index].color.z}};
                entity_json["start"] = { {"x", renderer.entity_stack[index].line.start.x}, {"y", renderer.entity_stack[index].line.start.y}, {"z", renderer.entity_stack[index].line.start.z}};
                entity_json["end"] = { {"x", renderer.entity_stack[index].line.end.x}, {"y", renderer.entity_stack[index].line.end.y}, {"z", renderer.entity_stack[index].line.end.z}};
                break;
            };
            case entity_types::entity_arc:{
                entity_json["type"] = "arc";
                break;
            };
            case entity_types::entity_circle:{
                entity_json["type"] = "circle";
                break;
            };
        }
        duk_push_string(ctx, entity_json.dump().c_str());
        duk_json_decode(ctx, -1);
        return 1;
    }
    duk_idx_t obj_idx = duk_push_object(ctx);
    duk_push_string(ctx, "no such entity");
    duk_put_prop_string(ctx, obj_idx, "status");
    return 1; 
}
static duk_ret_t render_set_entity(duk_context *ctx) {
    int index = duk_to_int(ctx, 0);
    if (index < renderer.entity_stack.size())
    {
        duk_to_object(ctx, 1);
        nlohmann::json j = json::parse(duk_json_encode(ctx, -1));
        int type = -1;
        for (auto& [key, value] : j.items())
        {
            if (key == "type")
            {
                if (value == "line")
                {
                    type = entity_types::entity_line;
                }
                else if (key == "circle")
                {
                    type = entity_types::entity_circle;
                }
                else if (key == "arc")
                {
                    type = entity_types::entity_arc;
                }
            }
        }
        for (auto& [key, value] : j.items())
        {
            //std::cout << key << " : " << value << "\n";
            if (key == "visable")
            {
                renderer.entity_stack[index].visable = value;
            }
            if (key == "layer")
            {
                renderer.entity_stack[index].layer = value;
            }
            if (key == "color")
            {
                for (auto& [sub_key, sub_value] : value.items())
                {
                    //std::cout << "\t" << sub_key << " : " << sub_value << "\n";
                    if (sub_key == "r")
                    {
                        renderer.entity_stack[index].color.x = sub_value;
                    }
                    if (sub_key == "g")
                    {
                        renderer.entity_stack[index].color.y = sub_value;
                    }
                    if (sub_key == "b")
                    {
                        renderer.entity_stack[index].color.z = sub_value;
                    }
                }
            }
            if (key == "start")
            {
                for (auto& [sub_key, sub_value] : value.items())
                {
                    //std::cout << "\t" << sub_key << " : " << sub_value << "\n";
                    if (sub_key == "x")
                    {
                        renderer.entity_stack[index].line.start.x = sub_value;
                    }
                    if (sub_key == "y")
                    {
                        renderer.entity_stack[index].line.start.y = sub_value;
                    }
                    if (sub_key == "z")
                    {
                        renderer.entity_stack[index].line.start.z = sub_value;
                    }
                }
            }
            if (key == "end")
            {
                for (auto& [sub_key, sub_value] : value.items())
                {
                    //std::cout << "\t" << sub_key << " : " << sub_value << "\n";
                    if (sub_key == "x")
                    {
                        renderer.entity_stack[index].line.end.x = sub_value;
                    }
                    if (sub_key == "y")
                    {
                        renderer.entity_stack[index].line.end.y = sub_value;
                    }
                    if (sub_key == "z")
                    {
                        renderer.entity_stack[index].line.end.z = sub_value;
                    }
                }
            }
        }
    }
    return 0;
}
static duk_ret_t render_stack_size(duk_context *ctx) {
    duk_push_int(ctx, renderer.entity_stack.size());
    return 1;
}
static duk_ret_t render_del_entity(duk_context *ctx) {
    renderer.entity_stack.erase(renderer.entity_stack.begin() + (int)duk_to_int(ctx, 0));
    return 0;
}
static duk_ret_t render_set_style(duk_context *ctx) {
    duk_to_object(ctx, 0);
    nlohmann::json j = json::parse(duk_json_encode(ctx, -1));
    for (auto& [key, value] : j.items())
    {
        if (key == "background_color")
        {
            for (auto& [sub_key, sub_value] : value.items())
            {
                if (sub_key == "r")
                {
                    renderer.clear_color.x = (float)sub_value;
                }
                if (sub_key == "g")
                {
                    renderer.clear_color.y = (float)sub_value;
                }
                if (sub_key == "b")
                {
                    renderer.clear_color.z = (float)sub_value;
                }
                if (sub_key == "a")
                {
                    renderer.clear_color.w = (float)sub_value;
                }
            }
        }
    }
    //printf("clear_color-> r: %.4f, g: %.4f, b: %.4f, a: %.4f\n", renderer.clear_color.x, renderer.clear_color.y, renderer.clear_color.z, renderer.clear_color.w);
    return 0;
}
static duk_ret_t render_clear(duk_context *ctx) {
    renderer.entity_stack.clear();
    return 0; 
}
static duk_ret_t render_get_scroll(duk_context *ctx) {
    nlohmann::json j;
    j["horizontal"] = renderer.scroll.y;
    j["vertical"] = renderer.scroll.x;
    duk_push_string(ctx, j.dump().c_str());
    duk_json_decode(ctx, -1);
    return 1; 
}
static duk_ret_t render_pan(duk_context *ctx) {
    renderer.pan.x += duk_to_number(ctx, 0);
    renderer.pan.y += duk_to_number(ctx, 1);
    return 0;  /* 1 return value (= undefined) */
}
static duk_ret_t render_zoom(duk_context *ctx) {
    renderer.zoom += duk_to_number(ctx, 0);
    return 0;  /* 1 return value (= undefined) */
}
static duk_ret_t render_get_zoom(duk_context *ctx) {
    duk_push_number(ctx, renderer.zoom);
    return 1;  /* 1 return value (= undefined) */
}
static duk_ret_t live_render_clear(duk_context *ctx) {
    renderer.live_entity_stack.clear();
    return 0;
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
        t.size = gui.default_text_size;
        t.color = gui.default_text_color;
        window_element_t e;
        e.type = element_types::element_text;
        e.text = t;
        ret = gui.windows[window_id].elements.size();
        gui.windows[window_id].elements.push_back(e);
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_set_text_style(duk_context *ctx) {
    int window_id = duk_to_int(ctx, 0);
    int text_id = duk_to_int(ctx, 1);
    duk_to_object(ctx, 2);
    nlohmann::json j = json::parse(duk_json_encode(ctx, -1));
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        if (text_id < gui.windows[window_id].elements.size())
        {
            for (auto& [key, value] : j.items())
            {
                if (key == "size")
                {
                    //printf("Setting size to: %.4f\n", (float)value);
                    gui.windows[window_id].elements[text_id].text.size = (float)value;
                }
                if (key == "color")
                {
                    for (auto& [sub_key, sub_value] : value.items())
                    {
                        if (sub_key == "r")
                        {
                            gui.windows[window_id].elements[text_id].text.color.x = (float)sub_value;
                        }
                        if (sub_key == "g")
                        {
                            gui.windows[window_id].elements[text_id].text.color.y = (float)sub_value;
                        }
                        if (sub_key == "b")
                        {
                            gui.windows[window_id].elements[text_id].text.color.z = (float)sub_value;
                        }
                        if (sub_key == "a")
                        {
                            gui.windows[window_id].elements[text_id].text.color.w = (float)sub_value;
                        }
                    }
                }
            }
        }
    }
    return 0;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_set_text(duk_context *ctx) {
    int window_id = duk_to_int(ctx, 0);
    int text_id = duk_to_int(ctx, 1);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        if (text_id < gui.windows[window_id].elements.size())
        {
            gui.windows[window_id].elements[text_id].text.text = std::string(duk_to_string(ctx, 2));
        }
    }
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
static duk_ret_t gui_window_sameline(duk_context *ctx) {
    int window_id = duk_to_int(ctx, 0);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        window_element_t e;
        e.type = element_types::element_sameline;
        gui.windows[window_id].elements.push_back(e);
    }
    return 0;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_separator(duk_context *ctx) {
    int window_id = duk_to_int(ctx, 0);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        window_element_t e;
        e.type = element_types::element_separator;
        gui.windows[window_id].elements.push_back(e);
    }
    return 0;  /* 0 return value (= undefined) */
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
static duk_ret_t gui_window_add_input_text(duk_context *ctx) {
    int ret = -1;
    int window_id = duk_to_int(ctx, 0);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        window_input_text_t w;
        w.text = std::string(duk_to_string(ctx, 1));
        sprintf(w.value, "%s", duk_to_string(ctx, 2));
        window_element_t e;
        e.type = element_types::element_input_text;
        e.input_text = w;
        ret = gui.windows[window_id].elements.size();
        gui.windows[window_id].elements.push_back(e);
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_get_input_text(duk_context *ctx) {
    std::string ret = "";
    int window_id = duk_to_int(ctx, 0);
    int widget_id = duk_to_int(ctx, 1);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        if (widget_id < gui.windows[window_id].elements.size())
        {
            ret = std::string(gui.windows[window_id].elements[widget_id].input_text.value);
        }
    }
    duk_push_string(ctx, ret.c_str());
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_add_input_double(duk_context *ctx) {
    int ret = -1;
    int window_id = duk_to_int(ctx, 0);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        window_input_double_t w;
        w.text = std::string(duk_to_string(ctx, 1));
        w.value = (double)duk_to_number(ctx, 2);
        window_element_t e;
        e.type = element_types::element_input_double;
        e.input_double = w;
        ret = gui.windows[window_id].elements.size();
        gui.windows[window_id].elements.push_back(e);
    }
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_get_input_double(duk_context *ctx) {
    double ret = 0.0f;
    int window_id = duk_to_int(ctx, 0);
    int widget_id = duk_to_int(ctx, 1);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        if (widget_id < gui.windows[window_id].elements.size())
        {
            ret = gui.windows[window_id].elements[widget_id].input_double.value;
        }
    }
    duk_push_number(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_get_mouse(duk_context *ctx) {
    duk_idx_t obj_idx = duk_push_object(ctx);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    const ImVec2 mouse_pos = io.MousePos;
    duk_push_int(ctx, mouse_pos.x);
    duk_put_prop_string(ctx, obj_idx, "x");
    duk_push_int(ctx, mouse_pos.y);
    duk_put_prop_string(ctx, obj_idx, "y");
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_get_keyboard(duk_context *ctx) {
    duk_idx_t obj_idx = duk_push_object(ctx);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    int keycode = -1;
    for (int x = 0; x < IM_ARRAYSIZE(io.KeysDown); x++)
    {
        if (io.KeysDown[x])
        {
            keycode = x;
            //printf("Keycode: %c\n", keycode);
            break;
        }
    }
    duk_push_int(ctx, keycode);
    duk_put_prop_string(ctx, obj_idx, "keycode");
    if (keycode > 0)
    {
        char charactor[5];
        sprintf(charactor, "%c", keycode);
        duk_push_string(ctx, charactor);
        duk_put_prop_string(ctx, obj_idx, "char");
    }
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_get_mouse_click(duk_context *ctx) {
    duk_idx_t obj_idx = duk_push_object(ctx);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    int keycode = -1;
    for (int x = 0; x < IM_ARRAYSIZE(io.MouseDown); x++)
    {
        if (io.MouseDown[x])
        {
            keycode = x;
            //printf("Keycode: %c\n", keycode);
            break;
        }
    }
    duk_push_int(ctx, keycode);
    duk_put_prop_string(ctx, obj_idx, "keycode");
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t gui_window_show(duk_context *ctx) {
    int window_id = duk_to_int(ctx, 0);
    bool show = duk_to_boolean(ctx, 1);
    if (window_id < gui.windows.size()) //Make sure this window exists!
    {
        gui.windows[window_id].visable = show;
    }
    return 0;  /* 0 return value (= undefined) */
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
static duk_ret_t file_open(duk_context *ctx) {
    int ret = -1;
    js.file_handle = fopen(duk_to_string(ctx, 0), duk_to_string(ctx, 1));
    if (js.file_handle != NULL)
    {
        ret = 0;
    }
    duk_push_number(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t file_write(duk_context *ctx) {
    if (js.file_handle != NULL)
    {
        fprintf(js.file_handle, "%s", duk_to_string(ctx, 0));
    }
    return 0;  /* 0 return value (= undefined) */
}
static duk_ret_t file_read(duk_context *ctx) {
    std::string read_line = "";
    if (js.file_handle != NULL)
    {
        if ((js.read = getline(&js.read_line, &js.len, js.file_handle)) != -1)
        {
            read_line = std::string(js.read_line);
            js.lines_available = true;
        }
        else
        {
            js.lines_available = false;
        }
    }
    else
    {
        read_line = "File not opened!";
    }
    duk_push_string(ctx, read_line.c_str());
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t file_lines_available(duk_context *ctx) {
    if (js.file_handle == NULL) js.lines_available = false;
    duk_push_boolean(ctx, js.lines_available);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t file_close(duk_context *ctx) {
    if (js.file_handle != NULL) fclose(js.file_handle);
    return 0;  /* 0 return value (= undefined) */
}
static duk_ret_t file_list_dir(duk_context *ctx) {
    duk_idx_t arr_idx = duk_push_array(ctx);
    duk_idx_t obj_idx;
    DIR *d;
    struct dirent *dir;
    struct stat s;
    d = opendir(duk_to_string(ctx, 0));
    int count = 0;
    if (d)
    {
        while((dir = readdir(d)) != NULL)
        {
            if (std::string(dir->d_name) != ".." && std::string(dir->d_name) != ".")
            {
                obj_idx = duk_push_object(ctx);
                duk_push_string(ctx, dir->d_name);
                duk_put_prop_string(ctx, obj_idx, "name");
                char path[2048];
                sprintf(path, "%s/%s", duk_to_string(ctx, 0), dir->d_name);
                if (stat(path, &s) == 0)
                {
                    if (s.st_mode & S_IFDIR)
                    {
                        duk_push_string(ctx, "dir");
                    }
                    else if (s.st_mode & S_IFREG)
                    {
                        duk_push_string(ctx, "file");
                    }
                }
                else
                {
                    duk_push_string(ctx, "error");
                }
                duk_put_prop_string(ctx, obj_idx, "type");
                duk_put_prop_index(ctx, arr_idx, count);
                count++;
            }
        }
        closedir(d);
    }
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t file_mkdir(duk_context *ctx) {
    int ret = mkdir(duk_to_string(ctx, 0), 0700);
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t file_remove(duk_context *ctx) {
    int ret = unlink(duk_to_string(ctx, 0));
    duk_push_int(ctx, ret);
    return 1;  /* 0 return value (= undefined) */
}
static duk_ret_t file_rmdir(duk_context *ctx) {
    int ret = rmdir(duk_to_string(ctx, 0));
    duk_push_int(ctx, ret);
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
    this->lines_available = true;
    this->window_open = false;
    this->loop = true;
    my_serial = NULL;
    ctx = duk_create_heap_default();

    bind("include", include, 1);
    bind("create_window", create_window, 3);
    bind("exit", exit, 1);
    bind("show_view_controls", show_view_controls, 1);

    const duk_function_list_entry render_class[] = {
        { "show_crosshair", render_show_crosshair, 1 /* no args */ },
        { "get_mouse", render_get_mouse, 0 /* no args */ },
        { "add_entity", render_add_entity, 1 /* no args */ },
        { "get_entity", render_get_entity, 1 /* no args */ },
        { "set_entity", render_set_entity, 2 /* no args */ },
        { "stack_size", render_stack_size, 0 /* no args */ },
        { "del_entity", render_del_entity, 1 /* no args */ },
        { "set_style", render_set_style, 1 /* no args */ },
        { "clear", render_clear, 0 /* no args */ },
        { "get_scroll", render_get_scroll, 0 /* no args */ },
        { "pan", render_pan, 2 /* no args */ },
        { "zoom", render_zoom, 1 /* no args */ },
        { "get_zoom", render_get_zoom, 0 /* no args */ },
        { NULL, NULL, 0 }
    };
    bind_module("render", render_class);

    const duk_function_list_entry live_render_class[] = {
        { "clear", live_render_clear, 0 /* no args */ },
        { NULL, NULL, 0 }
    };
    bind_module("live_render", live_render_class);

    const duk_function_list_entry serial_class[] = {
        { "list_ports", serial_list_ports, 0 /* no args */ },
        { "open", serial_open, 2 /* no args */ },
        { "is_open", serial_is_open, 0 /* no args */ },
        { "available", serial_available, 0 /* no args */ },
        { "read", serial_read, 0 /* no args */ },
        { "write", serial_write, 0 /* no args */ },
        { "close", serial_close, 0 /* no args */ },
        { NULL, NULL, 0 }
    };
    bind_module("serial", serial_class);


    const duk_function_list_entry http_class[] = {
        { "get", http_get, 4 /* no args */ },
        { NULL, NULL, 0 }
    };
    bind_module("http", http_class);
    
    const duk_function_list_entry ini_class[] = {
        { "get", ini_get, 4 /* no args */ },
        { NULL, NULL, 0 }
    };
    bind_module("ini", ini_class);

    const duk_function_list_entry gui_class[] = {
        { "new_window", gui_new_window, 1 /* no args */ },
        { "add_text", gui_window_add_text, 2 /* no args */ },
        { "set_text", gui_window_set_text, 3 /* no args */ },
        { "set_text_style", gui_window_set_text_style, 3 /* no args */ },
        { "add_checkbox", gui_window_add_checkbox, 3 /* no args */ },
        { "sameline", gui_window_sameline, 1 /* no args */ },
        { "separator", gui_window_separator, 1 /* no args */ },
        { "get_checkbox", gui_window_get_checkbox, 2 /* no args */ },
        { "add_slider", gui_window_add_slider, 5 /* no args */ },
        { "get_slider", gui_window_get_slider, 2 /* no args */ },
        { "add_button", gui_window_add_button, 2 /* no args */ },
        { "get_button", gui_window_get_button, 2 /* no args */ },
        { "add_input_text", gui_window_add_input_text, 3 /* no args */ },
        { "get_input_text", gui_window_get_input_text, 2 /* no args */ },
        { "add_input_double", gui_window_add_input_double, 3 /* no args */ },
        { "get_input_double", gui_window_get_input_double, 2 /* no args */ },
        { "get_mouse", gui_window_get_mouse, 0 /* no args */ },
        { "get_keyboard", gui_window_get_keyboard, 0 /* no args */ },
        { "get_mouse_click", gui_window_get_mouse_click, 0 /* no args */ },
        { "show", gui_window_show, 2 /* no args */ },
        { NULL, NULL, 0 }
    };
    bind_module("gui", gui_class);

    const duk_function_list_entry window_menu_class[] = {
        { "create", window_create_menu, 1 /* no args */ },
        { "add_button", window_add_menu_button, 2 /* no args */ },
        { "get_button", window_get_menu_button, 2 /* no args */ },
        { "add_checkbox", window_add_menu_checkbox, 3 /* no args */ },
        { "get_checkbox", window_get_menu_checkbox, 2 /* no args */ },
        { NULL, NULL, 0 }
    };
    bind_module("window_menu", window_menu_class);

    const duk_function_list_entry console_class[] = {
        { "log", print, 1 /* no args */ },
        { NULL, NULL, 0 }
    };
    bind_module("console", console_class);

    const duk_function_list_entry file_class[] = {
        { "open", file_open, 2 /* no args */ },
        { "write", file_write, 1 /* no args */ },
        { "read", file_read, 0 /* no args */ },
        { "lines_available", file_lines_available, 0 /* no args */ },
        { "close", file_close, 0 /* no args */ },
        { "list_dir", file_list_dir, 1 /* no args */ },
        { "mkdir", file_mkdir, 1 /* no args */ },
        { "remove", file_remove, 1 /* no args */ },
        { "rmdir", file_rmdir, 1 /* no args */ },
        { NULL, NULL, 0 }
    };
    bind_module("file", file_class);
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