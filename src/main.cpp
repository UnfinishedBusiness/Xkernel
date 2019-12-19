// dear imgui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl2.h>
#include <javascript/javascript.h>
#include <render/render.h>
#include <gui/gui.h>
#include <serial/serial.h>
#include <motion_control/motion_control.h>
#include <network/httplib.h>
#include <inih/inih.h>
#include <math/fmath.h>
#include <text/rang.h>
#include <iostream>
#include <chrono>
#include <stdio.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

uint64_t loop_timer;
uint64_t millis()
{
    using namespace std::literals;
    using Clock = std::chrono::system_clock;
    auto point1 = Clock::now();
    return point1.time_since_epoch() / 1ms;
}

Render renderer;
GUI gui;
Javascript js;
MotionControl motion_control;

int main(int argv, char** args)
{
    loop_timer = 0;
    int ret = -1;
    renderer = Render();
    gui = GUI();
    js = Javascript(); 
    js.init();
    motion_control.init();
    // Setup window
    if (argv > 0)
    {
        //printf("Script: %s\n", args[1]);
        js.eval_file(args[1]);
        js.eval("setup();");
        if (js.window_open == true)
        {
            glfwSetErrorCallback(glfw_error_callback);
        }
        while(js.loop == true)
        {
            motion_control.tick();
            if ((millis() - loop_timer) > renderer.loop_delay) //This timer should be settable from JS to give higher loop priority to classes like MotionControl
            {  
                if (glfwWindowShouldClose(renderer.window)) break;
                if (js.window_open == true)
                {
                    glfwPollEvents();
                    gui.tick();
                    renderer.render();
                }
                js.eval("loop();");
                loop_timer = millis();
            }
        }
        if (js.window_open == true)
        {
            gui.destroy();
            renderer.destroy();
            glfwDestroyWindow(renderer.window);
            glfwTerminate();
        }
        ret = js.ret;
        js.destroy();
    }
    return ret;
}
