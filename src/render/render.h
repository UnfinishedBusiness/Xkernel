//-----------------------------------------------------------------------------
// Methods for creating primatives and boiling it down to OpenGL calls
//-----------------------------------------------------------------------------
#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <render/Arcball.h>

enum entity_types {
    line,
    arc,
    circle,
};

struct display_size_t{
    int width;
    int height;
};

struct line_t{
    glm::vec3 start;
    glm::vec3 end;

    glm::vec3 (*start_callback)();
    glm::vec3 (*end_callback)();
};
struct arc_t{
    glm::vec3 center;
    glm::vec3 start_angle;
    glm::vec3 end_angle;
};
struct circle_t{
    glm::vec3 center;
    glm::vec3 radius;
};
struct entity_t{
    int type;
    line_t line;
    arc_t arc;
    circle_t circle;
};

class Render{

    public:
        glm::vec3 pan;
        glm::vec3 orbit_point;
        glm::vec3 rot;
        float zoom;
        bool show_crosshair;

        ImVec4 clear_color;
        GLFWwindow* window;
        void init(GLFWwindow* w);
        void destroy();
        void render();
        glm::vec2 get_mouse_in_world_coordinates();

        display_size_t getSize();
    private:
        
    
};

extern Render renderer;
extern std::vector<entity_t> entity_stack;
extern std::vector<entity_t> live_entity_stack;
#endif
