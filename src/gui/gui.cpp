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
            
            ImGui::SliderFloat("pan.x", &renderer.pan.x, -360.0f, 360.0f);
            ImGui::SliderFloat("pan.y", &renderer.pan.y, -360.0f, 360.0f);
            ImGui::SliderFloat("pan.z", &renderer.pan.z, -360.0f, 360.0f);

            ImGui::SliderFloat("orbit_center.x", &renderer.orbit_point.x, -360.0f, 360.0f);
            ImGui::SliderFloat("orbit_center.y", &renderer.orbit_point.y, -360.0f, 360.0f);
            ImGui::SliderFloat("orbit_center.z", &renderer.orbit_point.z, -360.0f, 360.0f);

            ImGui::SliderFloat("rot.x", &renderer.rot.x, -360.0f, 360.0f);
            ImGui::SliderFloat("rot.y", &renderer.rot.y, -360.0f, 360.0f);
            ImGui::SliderFloat("rot.z", &renderer.rot.z, -360.0f, 360.0f);

            ImGui::SliderFloat("zoom", &renderer.zoom, 0.000001f, 1.0f);
            ImGui::Checkbox("show crosshair", &renderer.show_crosshair);


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