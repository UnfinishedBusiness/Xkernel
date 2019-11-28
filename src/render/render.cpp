#include <render/render.h>
#include <string>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl2.h>
#include <gui/gui.h>
#include <chrono>

void Render::init(GLFWwindow* w)
{
    this->window = w;
    this->clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
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
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(this->clear_color.x, this->clear_color.y, this->clear_color.z, this->clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    gui.render();

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}