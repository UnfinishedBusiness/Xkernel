//-----------------------------------------------------------------------------
// Methods for creating primatives and boiling it down to OpenGL calls
//-----------------------------------------------------------------------------
#ifndef GUI_H
#define GUI_H

#include <string>
#include <GLFW/glfw3.h>

class GUI{

    public:
        bool show_demo_window;
        bool show_another_window;
        GLFWwindow* window;
        void init(GLFWwindow* w);
        void destroy();
        void tick();
        void render();
    private:

};

extern GUI gui;

#endif
