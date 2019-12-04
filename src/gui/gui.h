//-----------------------------------------------------------------------------
// Methods for creating primatives and boiling it down to OpenGL calls
//-----------------------------------------------------------------------------
#ifndef GUI_H
#define GUI_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>

enum element_types {
    text,
    checkbox,
};
struct window_checkbox_t{
    std::string text;
    bool value;
};
struct window_text_t{
    std::string text;
};
struct window_element_t{
    int type;

    window_text_t text;
    window_checkbox_t checkbox;
};
struct window_t{
    std::string title;
    bool visable;
    std::vector<window_element_t> elements;
};

class GUI{

    public:
        std::vector<window_t> windows;
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
