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
#include <network/httplib.h>
#include <inih/inih.h>
#include <math/fmath.h>
#include <text/rang.h>
#include <iostream>
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

Render render;
GUI gui;
Javascript js;

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Xkernel", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    render = Render();
    
    gui = GUI();
    js = Javascript();

    render.init(window);
    gui.init(window);    
    js.init();

    //List serial ports
    std::vector<serial::PortInfo> devices_found = serial::list_ports();
	std::vector<serial::PortInfo>::iterator iter = devices_found.begin();
	while( iter != devices_found.end() )
	{
		serial::PortInfo device = *iter++;

		printf( "(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(), device.hardware_id.c_str() );
	}
    //Get jetcad.io/robots.txt
    httplib::Client cli("jetcad.io", 80);
    auto res = cli.Get("/");
    if (res != NULL)
    {
        std::cout << "Request status = " << res->status << std::endl;
        if (res && res->status == 200) {
            std::cout << res->body << std::endl;
        }
    }
    else
    {
        std::cout << "Network Error!" << std::endl;
    }
    //Parse test.ini
    INIReader reader("test.ini");

    //Rang test
    std::cout << "Rang Test: " << rang::style::bold << rang::fg::blue << "This is a test" << rang::style::reset << std::endl;

    if (reader.ParseError() != 0) {
        std::cout << "Can't load 'test.ini'\n";
        return 1;
    }
    std::cout << "Config loaded from 'test.ini': version="
              << reader.GetInteger("protocol", "version", -1) << ", name="
              << reader.Get("user", "name", "UNKNOWN") << ", email="
              << reader.Get("user", "email", "UNKNOWN") << ", pi="
              << reader.GetReal("user", "pi", -1) << ", active="
              << reader.GetBoolean("user", "active", true) << "\n";

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        gui.tick();
        render.render();
    }

    gui.destroy();
    render.destroy();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
