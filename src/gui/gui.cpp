#include <gui/gui.h>
#include <render/render.h>
#include <string>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl2.h>

void GUI::init(GLFWwindow* w)
{
    this->window = w;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    ImGui_ImplOpenGL2_Init();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    // Our state
    show_demo_window = false;
}
void GUI::destroy()
{
    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
void GUI::tick()
{
    /// Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            ImGui::Begin("View Controls");

            
            ImGui::SliderFloat("rotX", &renderer.rotX, 0.0f, 360.0f);
            ImGui::SliderFloat("rotY", &renderer.rotY, 0.0f, 360.0f);
            ImGui::SliderFloat("rotZ", &renderer.rotZ, 0.0f, 360.0f);
            ImGui::SliderFloat("originX", &renderer.origin.x, 0.0f, 100.0f);
            ImGui::SliderFloat("originY", &renderer.origin.y, 0.0f, 100.0f);
            ImGui::SliderFloat("originZ", &renderer.origin.z, 0.0f, 100.0f);
            ImGui::SliderFloat("zoom", &renderer.zoom, 0.01f, 1.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
}
void GUI::render()
{
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}