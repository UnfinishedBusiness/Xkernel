//-----------------------------------------------------------------------------
// Methods for creating primatives and boiling it down to OpenGL calls
//-----------------------------------------------------------------------------
#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

enum entity_types {
    entity_line,
    entity_arc,
    entity_circle,
    entity_text,
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
struct text_t{
    std::string text;
    glm::vec3 position;
    double height;
};
struct entity_t{
    bool visable;
    int type;
    std::string layer;
    ImVec4 color;
    line_t line;
    arc_t arc;
    circle_t circle;
    text_t text;
};

class Render{

    public:
        std::vector<entity_t> entity_stack;
        std::vector<entity_t> live_entity_stack;
        glm::vec3 pan;
        glm::vec3 orbit_point;
        glm::vec3 rot;
        glm::vec2 scroll;
        float zoom;
        bool show_crosshair;
        glm::vec3 crosshair_pos;
        ImVec4 clear_color;
        GLFWwindow* window;
        void init();
        void destroy();
        void render();
        glm::vec2 get_mouse_in_world_coordinates();

        display_size_t getSize();
    private:
        
    
};

extern Render renderer;
#endif
