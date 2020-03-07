#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
   #include <GL/freeglut.h>
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include <OpenGL/glew.h>
#elif __linux__
    #include <GL/glew.h>
#elif __unix__
    #include <GL/glew.h>
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#include <render/render.h>
#include <render/asteroids_font.h>
#include <string>
#include <imgui/imgui.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl2.h>
#include <gui/gui.h>
#include <geometry/geometry.h>
#include <chrono>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    this->loop_delay = 0;
    glewInit();
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

    glTranslatef(this->pan.x, this->pan.y, this->pan.z);
    glScalef(this->zoom, this->zoom, this->zoom);
    
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
            if (entity_stack[x].deleted == false)
            {
                glColor3f(entity_stack[x].color.x, entity_stack[x].color.y, entity_stack[x].color.z);
                glLineWidth(entity_stack[x].width);
                if (entity_stack[x].style == entity_styles::entity_dashed)
                {
                    glPushAttrib(GL_ENABLE_BIT);
                    glLineStipple(10, 0xAAAA);
                    glEnable(GL_LINE_STIPPLE);
                    //printf("Rendering dashed line %d\n", x);
                }
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
                else if (entity_stack[x].type == entity_types::entity_circle)
                {
                    if (entity_stack[x].visable == true)
                    {
                        glBegin(GL_LINE_LOOP);
                        for(int i = 0; i < 360; i++)
                        {
                            double theta = 2.0f * 3.1415926f * double(i) / double(360);//get the current angle
                            double tx = entity_stack[x].circle.radius * (double)cosf(theta);//calculate the x component
                            double ty = entity_stack[x].circle.radius * (double)sinf(theta);//calculate the y component
                            glVertex2f(tx + entity_stack[x].circle.center.x, ty + entity_stack[x].circle.center.y);
                        }
                        glEnd();
                    }
                }
                else if (entity_stack[x].type == entity_types::entity_arc)
                {
                    if (entity_stack[x].visable == true)
                    {
                        DrawArc(entity_stack[x].arc.center.x, entity_stack[x].arc.center.y, entity_stack[x].arc.radius, entity_stack[x].arc.start_angle, entity_stack[x].arc.end_angle, 30); 
                    }
                }
                else if (entity_stack[x].type == entity_types::entity_text)
                {
                    entity_t e;
                    line_t l;

                    std::vector<asteroid_line_t> lines = string_to_lines(entity_stack[x].text.text, entity_stack[x].text.position.x * 12000, entity_stack[x].text.position.y * 12000, entity_stack[x].text.height * 1000);
                    //printf("Number of lines: %d\n", lines.size());
                    for (int x = 0; x < lines.size(); x++)
                    {
                        e.type = entity_types::entity_line;
                        l.start.x = (double)lines[x].start.x;
                        l.start.y = (double)lines[x].start.y;
                        l.end.x = (double)lines[x].end.x;
                        l.end.y = (double)lines[x].end.y;
                        e.color.x = 1.0f;
                        e.line = l;
                        glBegin(GL_LINES);
                            glVertex3f(e.line.start.x / 12000.0f, e.line.start.y / 12000.0f, e.line.start.z / 12000.0f);
                            glVertex3f(e.line.end.x / 12000.0f, e.line.end.y / 12000.0f, e.line.end.z / 12000.0f);
                        glEnd();
                    }
                }
                else if (entity_stack[x].type == entity_types::entity_filled_rectangle)
                {
                    //Left Triangle
                    glBegin(GL_TRIANGLES);
                    // Lower left vertex
                    glVertex2f(entity_stack[x].rectangle.bottom_left.x, entity_stack[x].rectangle.bottom_left.y);
                    // Lower right vertex
                    glVertex2f(entity_stack[x].rectangle.bottom_left.x + entity_stack[x].rectangle.size.x, entity_stack[x].rectangle.bottom_left.y);
                    // Upper vertex
                    glVertex2f(entity_stack[x].rectangle.bottom_left.x, entity_stack[x].rectangle.bottom_left.y + entity_stack[x].rectangle.size.y);
                    glEnd();
                    //Right Triangle
                    glBegin(GL_TRIANGLES);
                    // Lower Right vertex
                    glVertex2f(entity_stack[x].rectangle.bottom_left.x + entity_stack[x].rectangle.size.x, entity_stack[x].rectangle.bottom_left.y);
                    // Upper left vertex
                    glVertex2f(entity_stack[x].rectangle.bottom_left.x, entity_stack[x].rectangle.bottom_left.y + entity_stack[x].rectangle.size.y);
                    // Upper right vertex
                    glVertex2f(entity_stack[x].rectangle.bottom_left.x + entity_stack[x].rectangle.size.x, entity_stack[x].rectangle.bottom_left.y + entity_stack[x].rectangle.size.y);
                    glEnd();
                }
                else if (entity_stack[x].type == entity_types::entity_part)
                {
                    Geometry g;
                    glColor3f(entity_stack[x].color.x, entity_stack[x].color.y, entity_stack[x].color.z);
                    for (int i = 0; i < entity_stack[x].part.contours.size(); i++)
                    {
                        glBegin(GL_LINE_STRIP);
                        for (int ii = 0; ii < entity_stack[x].part.contours[i].points.size(); ii++)
                        {
                            glm::vec2 p = g.rotate_point(entity_stack[x].part.center, entity_stack[x].part.contours[i].points[ii], entity_stack[x].part.angle);
                            glVertex2f((p.x + entity_stack[x].part.offset.x) * entity_stack[x].part.scale, (p.y + entity_stack[x].part.offset.y) * entity_stack[x].part.scale);
                        }
                        glEnd();
                    }
                    glColor3f(0.0f, 1.0f, 0.0f); //Toolpaths are green
                    for (int i = 0; i < entity_stack[x].part.toolpaths.size(); i++)
                    {
                        glBegin(GL_LINE_STRIP);
                        for (int ii = 0; ii < entity_stack[x].part.toolpaths[i].points.size(); ii++)
                        {
                            glm::vec2 p = g.rotate_point(entity_stack[x].part.center, entity_stack[x].part.toolpaths[i].points[ii], entity_stack[x].part.angle);
                            glVertex2f((p.x + entity_stack[x].part.offset.x) * entity_stack[x].part.scale, (p.y + entity_stack[x].part.offset.y) * entity_stack[x].part.scale);
                        }
                        glEnd();
                    }
                    glColor3f(entity_stack[x].color.x, entity_stack[x].color.y, entity_stack[x].color.z);
                }
                if (entity_stack[x].style == entity_styles::entity_dashed)
                {
                    glPopAttrib();
                }
            }
        }
    }
    

    //Draw Origin
    if (show_crosshair == true)
    {
        glLineWidth(1);
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y), (this->crosshair_pos.z));
            glVertex3f((this->crosshair_pos.x) + (10 / this->zoom), (this->crosshair_pos.y), (this->crosshair_pos.z));
        glEnd();
        glBegin(GL_LINES);
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y), (this->crosshair_pos.z));
            glVertex3f((this->crosshair_pos.x) - (10 / this->zoom), (this->crosshair_pos.y), (this->crosshair_pos.z));
        glEnd();
        glBegin(GL_LINES);
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y), (this->crosshair_pos.z));
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y)  + (10 / this->zoom), (this->crosshair_pos.z));
        glEnd();
        glBegin(GL_LINES);
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y), (this->crosshair_pos.z));
            glVertex3f((this->crosshair_pos.x), (this->crosshair_pos.y)  - (10 / this->zoom), (this->crosshair_pos.z));
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
void Render::DrawArc(double cx, double cy, double r, double start_angle, double end_angle, int num_segments) 
{
    glm::vec3 start;
    glm::vec3 sweeper;
    glm::vec3 end;
    start.x = cx + (r * cosf((start_angle) * 3.1415926f / 180.0f));
    start.y = cy + (r * sinf((start_angle) * 3.1415926f / 180.0f));
    end.x = cx + (r * cosf((end_angle) * 3.1415926f / 180.0f));
    end.y = cy + (r * sinf((end_angle) * 3.1415926f / 180.0f));

    glBegin(GL_LINE_STRIP);
     glVertex2f(start.x, start.y);
                    
    //draw start line
    /*glBegin(GL_LINES);
        glVertex2f(cx, cy);
        glVertex2f(start.x, start.y);
    glEnd();*/
    double diff = MAX(start_angle, end_angle) - MIN(start_angle, end_angle);
    if (diff > 180) diff = 360 - diff;
    double angle_increment = diff / num_segments;
    double angle_pointer = start_angle + angle_increment;
    for (int i = 0; i < num_segments; i++)
    {
        sweeper.x = cx + (r * cosf((angle_pointer) * 3.1415926f / 180.0f));
        sweeper.y = cy + (r * sinf((angle_pointer) * 3.1415926f / 180.0f));
        angle_pointer += angle_increment;

        /*glBegin(GL_LINES);
            glVertex2f(cx, cy);
            glVertex2f(sweeper.x, sweeper.y);
        glEnd();*/
        glVertex2f(sweeper.x, sweeper.y);
    }
    //draw end line
    /*glBegin(GL_LINES);
        glVertex2f(cx, cy);
        glVertex2f(end.x, end.y);
    glEnd();*/
    glVertex2f(end.x, end.y);
    glEnd();
}