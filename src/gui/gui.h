//-----------------------------------------------------------------------------
// Methods for creating primatives and boiling it down to OpenGL calls
//-----------------------------------------------------------------------------
#ifndef GUI_H
#define GUI_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <extra/TextEditor/TextEditorHandler.h>

/* Begin window structures */
enum element_types {
    element_text,
    element_checkbox,
    element_radio_group,
    element_slider,
    element_button,
    element_sameline,
    element_separator,
    element_input_text,
    element_input_double,
};
struct window_slider_t{
    std::string text;
    float value;
    float min;
    float max;
};
struct window_checkbox_t{
    std::string text;
    bool value;
};
struct window_radio_group_t{
    std::vector<std::string> items;
    int value;
};
struct window_text_t{
    std::string text;
    ImVec4 color;
    float size;
};
struct window_button_t{
    std::string text;
    bool value;
};
struct window_input_text_t{
    std::string text;
    char value[1024];
};
struct window_input_double_t{
    std::string text;
    double value;
};
struct window_element_t{
    int type;
    window_text_t text;
    window_checkbox_t checkbox;
    window_radio_group_t radio_group;
    window_slider_t slider;
    window_button_t button;
    window_input_text_t input_text;
    window_input_double_t input_double;
};
struct window_t{
    std::string title;
    bool visable;
    std::vector<window_element_t> elements;
};
/* End window structures */

/* Begin menu stuctures */
enum menu_types {
    menu_button,
    menu_checkbox,
};
struct menu_button_t{
    std::string text;
    bool value;
};
struct menu_checkbox_t{
    std::string text;
    bool value;
};
struct menu_item_t{
    std::string title;
    int type;
    menu_button_t button;
    menu_checkbox_t checkbox;
};
struct menu_t{
    std::string title;
    std::vector<menu_item_t> items;
};
/* End menu structures */

class GUI{

    public:
        TextEditorHandler text_editor_handler;
        std::vector<window_t> windows;
        std::vector<menu_t> menu;
        bool show_demo_window;
        bool show_another_window;
        bool show_view_controls;
        ImVec4 default_text_color;
        float default_text_size;
        GLFWwindow* window;
        void init(GLFWwindow* w);
        void destroy();
        void tick();
        void render();
    private:
        

};

extern GUI gui;

#endif
