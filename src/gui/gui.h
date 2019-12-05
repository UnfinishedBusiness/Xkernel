//-----------------------------------------------------------------------------
// Methods for creating primatives and boiling it down to OpenGL calls
//-----------------------------------------------------------------------------
#ifndef GUI_H
#define GUI_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>

/* Begin window structures */
enum element_types {
    element_text,
    element_checkbox,
    element_slider,
    element_button,
    element_sameline,
    element_separator,
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
struct window_text_t{
    std::string text;
};
struct window_button_t{
    std::string text;
    bool value;
};
struct window_element_t{
    int type;
    window_text_t text;
    window_checkbox_t checkbox;
    window_slider_t slider;
    window_button_t button;
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
        std::vector<window_t> windows;
        std::vector<menu_t> menu;
        bool show_demo_window;
        bool show_another_window;
        bool show_view_controls;
        GLFWwindow* window;
        void init(GLFWwindow* w);
        void destroy();
        void tick();
        void render();
    private:

};

extern GUI gui;

#endif
