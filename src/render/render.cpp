#include <render/render.h>
#include <string>
#include <imgui/imgui.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl2.h>
#include <gui/gui.h>
#include <chrono>
#include <vector>

std::vector<entity_t> entity_stack;

void Render::init(GLFWwindow* w)
{
    this->window = w;
    this->clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    this->rot.x = 0;
    this->rot.y = 0;
    this->rot.z = 0;
    this->orbit_point.x = 0;
    this->orbit_point.y = 0;
    this->orbit_point.z = 0;
    this->zoom = 0.001;
    this->show_crosshair = true;

    entity_t e;
    e.type = entity_types::line;
    line_t l;
    l.start = {0, 0, 0};
    l.end = {10, 10, 0};
    e.line = l;
    entity_stack.push_back(e);
}
display_size_t Render::getSize()
{
    display_size_t display_size;
    glfwGetFramebufferSize(this->window, &display_size.width, &display_size.height);
    return display_size;
}
void Render::destroy()
{
    
}
void Render::render()
{
    display_size_t display_size = this->getSize();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho( -display_size.width/2, display_size.width/2, -display_size.height/2, display_size.height/2, -1,1 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, display_size.width, display_size.height);

    glClearColor(this->clear_color.x, this->clear_color.y, this->clear_color.z, this->clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glDisable(GL_LIGHTING);

    glScalef(this->zoom * 600, this->zoom * 600, this->zoom * 600);
    glTranslatef(this->pan.x, this->pan.y, this->pan.z);
    
    if (entity_stack.size() > 0)
    {
        for (long x = 0; x < entity_stack.size(); x++)
        {
            glColor3f(1.0, 0.0, 0.0); //This needs to be stored int entity_t
            if (entity_stack[x].type == entity_types::line)
            {
                glBegin(GL_LINES);
                    glVertex3f(entity_stack[x].line.start.x, entity_stack[x].line.start.y, entity_stack[x].line.start.z);
                    glVertex3f(entity_stack[x].line.end.x, entity_stack[x].line.end.y, entity_stack[x].line.end.z);
                glEnd();
            }
        }
    }
    /*glLineWidth(1);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(10.0, 0.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 10.0, 0.0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 10.0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-10.0, 0.0, 0.0);
    glEnd();*/

    //Draw Origin
    if (show_crosshair == true)
    {
        glLineWidth(1);
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(-0.010 / this->zoom, 0, 0);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(+0.010 / this->zoom, 0, 0);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0, +0.010 / this->zoom, 0);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0, -0.010 / this->zoom, 0);
        glEnd();
    }
    gui.render();

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}