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

glm::vec3 start_test()
{
    glm::vec3 point;
    point.x = 0;
    point.y = 0;
    point.z = 0;
}
glm::vec3 end_test()
{
    glm::vec2 mouse_pos = renderer.get_mouse_in_world_coordinates();
    glm::vec3 point;
    point.x = mouse_pos.x;
    point.y = mouse_pos.y;
    point.z = 0;
    //printf("End is: %.4f ,%.4f, %.4f\n", point.x, point.y, point.z);
    return point;
}

void Render::init()
{
    this->clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    this->rot.x = 0;
    this->rot.y = 0;
    this->rot.z = 0;
    this->orbit_point.x = 0;
    this->orbit_point.y = 0;
    this->orbit_point.z = 0;
    this->zoom = 0.001;
    this->show_crosshair = false;
    this->crosshair_pos.x = 0;
    this->crosshair_pos.y = 0;
    this->crosshair_pos.z = 0;
    entity_t e;
    e.type = entity_types::entity_line;
    line_t l;
    l.start = {0, 0, 0};
    l.end = {10, 10, 0};
    e.line = l;
    entity_stack.push_back(e);

    /*
    e.type = entity_types::entity_line;
    //line_t l;
    l.start_callback = &start_test;
    l.end_callback = &end_test;
    e.line = l;
    live_entity_stack.push_back(e);*/
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
    //Render live ensities in stack and get points from user settable callbacks
    if (live_entity_stack.size() > 0)
    {
        for (long x = 0; x < live_entity_stack.size(); x++)
        {
            glColor3f(live_entity_stack[x].color.x, live_entity_stack[x].color.y, live_entity_stack[x].color.z); //This needs to be stored int entity_t
            if (live_entity_stack[x].type == entity_types::entity_line)
            {
                if (live_entity_stack[x].visable == true)
                {
                    if (live_entity_stack[x].line.start_callback != NULL && live_entity_stack[x].line.end_callback != NULL)
                    {
                        glm::vec3 start = live_entity_stack[x].line.start_callback();
                        glm::vec3 end = live_entity_stack[x].line.end_callback();
                        glBegin(GL_LINES);
                            glVertex3f(start.x, start.y, start.z);
                            glVertex3f(end.x, end.y, end.z);
                        glEnd();
                    }
                }
            }
        }
    }
    //Render all entities on stack
    if (entity_stack.size() > 0)
    {
        for (long x = 0; x < entity_stack.size(); x++)
        {
            glColor3f(entity_stack[x].color.x, entity_stack[x].color.y, entity_stack[x].color.z);
            if (entity_stack[x].type == entity_types::entity_line)
            {
                if (entity_stack[x].visable == true)
                {
                    glBegin(GL_LINES);
                        glVertex3f(entity_stack[x].line.start.x, entity_stack[x].line.start.y, entity_stack[x].line.start.z);
                        glVertex3f(entity_stack[x].line.end.x, entity_stack[x].line.end.y, entity_stack[x].line.end.z);
                    glEnd();
                }
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
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y), (this->crosshair_pos.z));
            glVertex3f((this->crosshair_pos.x) + (0.010 / this->zoom), (this->crosshair_pos.y), (this->crosshair_pos.z));
        glEnd();
        glBegin(GL_LINES);
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y), (this->crosshair_pos.z));
            glVertex3f((this->crosshair_pos.x) - (0.010 / this->zoom), (this->crosshair_pos.y), (this->crosshair_pos.z));
        glEnd();
        glBegin(GL_LINES);
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y), (this->crosshair_pos.z));
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y)  + (0.010 / this->zoom), (this->crosshair_pos.z));
        glEnd();
        glBegin(GL_LINES);
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y), (this->crosshair_pos.z));
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y)  - (0.010 / this->zoom), (this->crosshair_pos.z));
        glEnd();
    }
    gui.render();

    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}
glm::vec2 Render::get_mouse_in_world_coordinates()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImVec2 mouse_pos = io.MousePos;
    //printf("Mouse-> X: %.4f, Y: %.4f\n", mouse_pos.x, mouse_pos.y);
    if (mouse_pos.x <= -300000000000000 || mouse_pos.y <= -300000000000000)
    {
        //printf("Mouse context lost!\n");
        mouse_pos.x = 0.0f;
        mouse_pos.y = 0.0f;
    }
    glm::vec2 ret;
    GLint viewport[4]; //var to hold the viewport info
    GLdouble modelview[16]; //var to hold the modelview info
    GLdouble projection[16]; //var to hold the projection matrix info
    GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
    GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
    glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
    glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info
	winX = (float)mouse_pos.x;
    winY = (float)viewport[3] - (float)mouse_pos.y;
	winZ = 0;
	//get the world coordinates from the screen coordinates
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
    ret.x = worldX;
    ret.y = worldY;
    return ret;
}