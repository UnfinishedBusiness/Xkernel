//-----------------------------------------------------------------------------
// Methods for creating primatives and boiling it down to OpenGL calls
//-----------------------------------------------------------------------------
#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct display_size_t{
    int width;
    int height;
};

class Render{

    public:
        ImVec4 clear_color;
        GLFWwindow* window;
        void init(GLFWwindow* w);
        void destroy();
        void render();

        display_size_t getSize();
    private:
        float rotY;
        float rotX;
        glm::vec3 origin;
        glm::vec3 lookat;
        float distance;
    
};

extern Render render;

#endif
