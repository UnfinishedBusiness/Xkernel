//-----------------------------------------------------------------------------
// Methods for creating primatives and boiling it down to OpenGL calls
//-----------------------------------------------------------------------------
#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>

class Render{

    public:
        ImVec4 clear_color;
        GLFWwindow* window;
        void init(GLFWwindow* w);
        void destroy();
        void render();
    private:
    
};

extern Render render;

#endif
