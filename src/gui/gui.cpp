#include <gui/gui.h>
#include <render/render.h>
#include <string>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl2.h>

#include <extra/TextEditor/TextEditor.h>

void GUI::init(GLFWwindow* w)
{
    this->window = w;
    this->show_view_controls = false;
    this->show_demo_window = false;
    default_text_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    default_text_size = 0.350;
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
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ResizeGrip, ImVec4(0.3f, 0.3f, 0.3f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, ImVec4(0.3f, 0.3f, 0.3f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.3f, 0.3f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.8f, 0.8f, 0.8f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.6f, 0.6f, 0.6f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));

    text_editor_handler.init();
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
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
        /// Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO& io = ImGui::GetIO();
        ImFontAtlas* atlas = io.Fonts;
        ImFont* font = atlas->Fonts[0];
        font->Scale = this->default_text_size;
        ImGui::PushFont(font);
        if (menu.size() > 0)
        {
            ImFontAtlas* atlas = io.Fonts;
            ImFont* font = atlas->Fonts[0];
            font->Scale = this->default_text_size;
            ImGui::PushFont(font);
            display_size_t window_size = renderer.getSize();
            ImGui::SetNextWindowSize(ImVec2(window_size.width,10), ImGuiCond_Always);
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
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
        }
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);
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

            ImGuiIO& io = ImGui::GetIO();
            ImFontAtlas* atlas = io.Fonts;
            ImFont* font = atlas->Fonts[0];
            float original_font_scale = font->Scale;
            //font->Scale = 1;
            ImGui::PushFont(font);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            font->Scale = original_font_scale;
            ImGui::PushFont(font);
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
                        ImGuiIO& io = ImGui::GetIO();
                        ImFontAtlas* atlas = io.Fonts;
                        ImFont* font = atlas->Fonts[0];
                        font->Scale = windows[x].elements[y].text.size;
                        ImGui::PushFont(font);
                        ImGui::PushStyleColor(ImGuiCol_Text, windows[x].elements[y].text.color);
                        ImGui::Text("%s", windows[x].elements[y].text.text.c_str());
                        ImGui::PushStyleColor(ImGuiCol_Text, this->default_text_color);
                        font->Scale = this->default_text_size;
                        ImGui::PushFont(font);
                    }
                    else if (windows[x].elements[y].type == element_types::element_checkbox)
                    {
                        ImGui::Checkbox(windows[x].elements[y].checkbox.text.c_str(), &windows[x].elements[y].checkbox.value);
                    }
                    else if (windows[x].elements[y].type == element_types::element_radio_group)
                    {
                        for (int z = 0; z < windows[x].elements[y].radio_group.items.size(); z++)
                        {
                            ImGui::RadioButton(windows[x].elements[y].radio_group.items[z].c_str(), &windows[x].elements[y].radio_group.value, z);
                            if (z < windows[x].elements[y].radio_group.items.size() -1) ImGui::SameLine();
                        }
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
                    else if (windows[x].elements[y].type == element_types::element_input_text)
                    {
                        ImGui::InputText(windows[x].elements[y].input_text.text.c_str(), windows[x].elements[y].input_text.value, IM_ARRAYSIZE(windows[x].elements[y].input_text.value));
                    }
                    else if (windows[x].elements[y].type == element_types::element_input_double)
                    {
                        ImGui::InputDouble(windows[x].elements[y].input_double.text.c_str(), &windows[x].elements[y].input_double.value, 0.01f, 1.0f, "%.4f");
                    }
                }
                /*if (ImGui::IsAnyWindowHovered())
                {
                    printf("true\n");
                }
                else
                {
                    printf("False\n");
                }*/
                
                ImGui::End();
            }
        }

        text_editor_handler.render();

        if (!io.WantCaptureMouse)
        {
            renderer.scroll.x = io.MouseWheel;
            renderer.scroll.y = io.MouseWheelH;
        }
        ImGui::Render();
}
void GUI::render()
{
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}