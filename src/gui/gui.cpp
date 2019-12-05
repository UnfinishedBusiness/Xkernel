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
    this->show_view_controls = false;
    this->show_demo_window = false;
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
    
}
void GUI::destroy()
{
    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
bool show_app_main_menu_bar;
bool p_open = true;
void GUI::tick()
{
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoBackground;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        /// Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        display_size_t window_size = renderer.getSize();
        ImGui::SetNextWindowSize(ImVec2(window_size.width,450), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::Begin("Menu Bar Window", &p_open, window_flags);
        if (ImGui::BeginMenuBar())
        {
            for (int x = 0; x < menu.size(); x++)
            {
                if (ImGui::BeginMenu(menu[x].title.c_str()))
                {
                    for (int y = 0; y < menu[x].items.size(); y++)
                    {
                        if (menu[x].items[y].type == menu_types::menu_button)
                        {
                            if (ImGui::MenuItem(menu[x].items[y].button.text.c_str()))
                            { 
                                menu[x].items[y].button.value = true;
                            }
                            else
                            {
                                menu[x].items[y].button.value = false;
                            }
                        }
                        else if (menu[x].items[y].type == menu_types::menu_checkbox)
                        {
                            ImGui::MenuItem(menu[x].items[y].checkbox.text.c_str(), NULL, &menu[x].items[y].checkbox.value);
                        }
                    }
                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
        window_flags = 0;
        
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

    
        if (show_view_controls)
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
        for (int x = 0; x < windows.size(); x ++)
        {
            if (windows[x].visable == true)
            {
                ImGui::Begin(windows[x].title.c_str());
                for (int y = 0; y < windows[x].elements.size(); y++)
                {
                    if (windows[x].elements[y].type == element_types::element_text)
                    {
                        ImGui::Text("%s", windows[x].elements[y].text.text.c_str());
                    }
                    else if (windows[x].elements[y].type == element_types::element_checkbox)
                    {
                        ImGui::Checkbox(windows[x].elements[y].checkbox.text.c_str(), &windows[x].elements[y].checkbox.value);
                    }
                    else if (windows[x].elements[y].type == element_types::element_slider)
                    {
                        ImGui::SliderFloat(windows[x].elements[y].slider.text.c_str(), &windows[x].elements[y].slider.value, windows[x].elements[y].slider.min, windows[x].elements[y].slider.max);
                    }
                    else if (windows[x].elements[y].type == element_types::element_button)
                    {
                        if (ImGui::Button(windows[x].elements[y].button.text.c_str()))
                        {
                            windows[x].elements[y].button.value = true;
                        }
                        else
                        {
                            windows[x].elements[y].button.value = false;
                        }
                    }
                    else if (windows[x].elements[y].type == element_types::element_sameline)
                    {
                        ImGui::SameLine();
                    }
                    else if (windows[x].elements[y].type == element_types::element_separator)
                    {
                        ImGui::Separator();
                    }
                }
                ImGui::End();
            }
        }
        // Rendering
        ImGui::Render();
}
void GUI::render()
{
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}