#include <render/render.h>
#include <string>
#include <imgui/imgui.h>
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl2.h>
#include <gui/gui.h>
#include <render/Arcball.h>
#include <chrono>

static Arcball arcball(1.5f, true, true );

void Render::init(GLFWwindow* w)
{
    this->window = w;
    this->clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
    this->rotY = 0;
    this->rotX = 0;
    this->origin.x = 10;
    this->origin.y = 10;
    this->origin.z = 10;
    this->lookat.x = 0;
    this->lookat.y = 0;
    this->lookat.z = 0;
    this->distance = 1;
}
display_size_t Render::getSize()
{
    display_size_t display_size;
    glfwGetFramebufferSize(this->window, &display_size.width, &display_size.height);
}
void Render::destroy()
{
    
}
void Render::render()
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glLoadIdentity();
    gluOrtho2D(-(float)w/h, (float)w/h, -1.0, 1.0);
    glRotatef(-this->rotY, 0.0f, 1.0, 0.0f);
    glRotatef(-this->rotX, 1.0f, 0.0f, 0.0f);
    glTranslatef(-origin.x, -origin.y, -origin.z);

    glClearColor(this->clear_color.x, this->clear_color.y, this->clear_color.z, this->clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINES);
        glColor3f(0, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(10, 10, 10);
    glEnd();

    gui.render();

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}