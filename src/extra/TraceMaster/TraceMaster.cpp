#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
   //#include <GL/freeglut.h>
   #include <GL/glew.h>
   #include <opencv2/opencv.hpp>
   #include <opencv2/imgproc/imgproc_c.h>
   #include <opencv2/imgproc/types_c.h>
   #include <opencv2/videoio/legacy/constants_c.h>
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
    #include <OpenGL/glew.h>
#elif __linux__
    #include <GL/glew.h>
    #include <opencv2/opencv.hpp>
#elif __unix__
    #include <GL/glew.h>
#elif defined(_POSIX_VERSION)
    // POSIX
#else
#   error "Unknown compiler"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <chrono>
#include <cmath>
#include <string>
#include <algorithm>
#include <imgui/imgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <extra/TraceMaster/TraceMaster.h>

GLuint TraceMaster::matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter) {
    // Generate a number for our textureID's unique handle
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind to our texture handle
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Catch silly-mistake texture interpolation method for magnification
    if (magFilter == GL_LINEAR_MIPMAP_LINEAR  ||
            magFilter == GL_LINEAR_MIPMAP_NEAREST ||
            magFilter == GL_NEAREST_MIPMAP_LINEAR ||
            magFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        std::cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << std::endl;
        magFilter = GL_LINEAR;
    }

    // Set texture interpolation methods for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

    // Set incoming texture format to:
    // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
    // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
    // Work out other mappings as required ( there's a list in comments in main() )
    GLenum inputColourFormat = GL_BGR;
    if (mat.channels() == 1)
    {
        inputColourFormat = GL_LUMINANCE;
    }
    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,            // Internal colour format to convert to
                 mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                 mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                 0,                 // Border width in pixels (can either be 1 or 0)
                 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE,  // Image data type
                 mat.ptr());        // The actual image data itself

    // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR  ||
            minFilter == GL_LINEAR_MIPMAP_NEAREST ||
            minFilter == GL_NEAREST_MIPMAP_LINEAR ||
            minFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return textureID;
}
void TraceMaster::open()
{
    this->capture = cv::VideoCapture("/dev/video0");
    if (!capture.isOpened()) {
        std::cout << "Cannot open video: /dev/video0" << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        this->show = true;
    }
    double fps = 0.0;
    fps = capture.get(CV_CAP_PROP_FPS);
    if (fps != fps) { // NaN
        fps = 25.0;
    }
    int window_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int window_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    std::cout << "Video width: " << window_width << std::endl;
    std::cout << "Video height: " << window_height << std::endl;
}
void TraceMaster::close()
{
    this->init();
}
void TraceMaster::init()
{
    this->show = false;
}
void TraceMaster::render()
{
    if (this->show) //Entry point for TraceMaster
    {
        if (!capture.read(this->frame)) 
        {
            std::cout << "Cannot grab a frame." << std::endl;
        }
        else
        {
            cv::blur( this->frame, this->blurred, cv::Size(3,3) );
            /*cv::Canny(this->blurred, this->canny, 190, 200, 3);
            std::vector<cv::Vec4i> lines;
            HoughLinesP(this->canny, lines, 1, CV_PI/180, 50, 50, 10 );
            for( size_t i = 0; i < lines.size(); i++ )
            {
                cv::Vec4i l = lines[i];
                line(this->frame, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0,0,255), 3, CV_AA);
            }

            if (lines.size() == 0)
            {
                line(this->frame, cv::Point(640/2, 100), cv::Point(640/2, 480-100), cv::Scalar(0,0,0), 1, CV_AA);
                line(this->frame, cv::Point(100, 480/2), cv::Point(640-100, 480/2), cv::Scalar(0,0,0), 1, CV_AA);
                circle(this->frame, cv::Point(640/2, 480/2), 100, cv::Scalar(0,0,0));
            }
            else if (lines.size() > 1)
            {
                line(this->frame, cv::Point(640/2, 100), cv::Point(640/2, 480-100), cv::Scalar(0,255,0), 1, CV_AA);
                line(this->frame, cv::Point(100, 480/2), cv::Point(640-100, 480/2), cv::Scalar(0,255,0), 1, CV_AA);
                circle(this->frame, cv::Point(640/2, 480/2), 50, cv::Scalar(0,255,0));
            }*/
            cv::Mat hsv;
            cvtColor(this->blurred, hsv, cv::COLOR_BGR2HSV);
            //morphological opening (removes small objects from the foreground)
            cv::erode(this->blurred, this->blurred, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
            cv::dilate(this->blurred, this->blurred, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5))); 

            //morphological closing (removes small holes from the foreground)
            cv::dilate(this->blurred, this->blurred, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5))); 
            cv::erode(this->blurred, this->blurred, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
            cv::Mat mask1,mask2;
            cv::inRange(this->blurred, cv::Scalar(0, 0, 0), cv::Scalar(100, 100, 100), mask1);
            std::vector<std::vector<cv::Point> > contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours( mask1, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
            /// Get the moments
            std::vector<cv::Moments> mu(contours.size() );
            for( int i = 0; i < contours.size(); i++ )
                { mu[i] = moments( contours[i], false ); }

            ///  Get the mass centers:
            std::vector<cv::Point2f> mc( contours.size() );
            for( int i = 0; i < contours.size(); i++ )
            { 
                mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
            }
            /// Draw contours
            cv::RNG rng(12345);
            cv::Mat drawing = cv::Mat::zeros( mask1.size(), CV_8UC3 );
            int closest_contour_distance = 1000;
            cv::Point closest_contour_point;
            cv::Point next_next_contour_point;
            for( int i = 0; i< contours.size(); i++ )
            {
                cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
                drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point());
                int large_distance = 10000;
                cv::Point closest;
                cv::Point next;
                for (int ii = 0; ii < contours[i].size(); ii++)
                {
                    int x = (640/2) - contours[i][ii].x;
                    int y = (480/2) - contours[i][ii].y;
                    int distance = sqrt(x*x + y*y);
                    if (distance < large_distance)
                    {
                        large_distance = distance;
                        closest = contours[i][ii];
                        if (ii+10 < contours[i].size())
                        {
                            next = contours[i][ii+10];
                        }
                    }
                }
                int x = (640/2) - closest.x;
                int y = (480/2) - closest.y;
                int distance = sqrt(x*x + y*y);
                if (distance < closest_contour_distance)
                {
                    closest_contour_distance = distance;
                    closest_contour_point = closest;
                    next_next_contour_point = next;
                }
            }
            //circle( drawing, mc[i], 4, color, -1, 8, 0 );
            circle(drawing, closest_contour_point, 4, cv::Scalar(0,255,0), -1, 8, 0 );
            circle(this->frame, closest_contour_point, 10, cv::Scalar(0,255,0), 5);
            circle(this->frame, next_next_contour_point, 3, cv::Scalar(255,0,0), 3);
            cv::Moments oMoments = cv::moments(mask1);
            //if (oMoments.m00 < 14587530.0000 && oMoments.m00 > 1000.0f)
            if (closest_contour_distance < 15)
            {
                //printf("m01: %.4f, m10: %.4f, m00: %.4f\n", oMoments.m01, oMoments.m10, oMoments.m00);
                //int posX = oMoments.m10 / oMoments.m00;
                //int posY = oMoments.m01 / oMoments.m00;
                //circle(this->frame, cv::Point(posX, posY), 10, cv::Scalar(0,255,0), 5);

                line(this->frame, cv::Point(640/2, 100), cv::Point(640/2, 480-100), cv::Scalar(0,255,0), 1, CV_AA);
                line(this->frame, cv::Point(100, 480/2), cv::Point(640-100, 480/2), cv::Scalar(0,255,0), 1, CV_AA);
                circle(this->frame, cv::Point(640/2, 480/2), 50, cv::Scalar(0,255,0));

                //circle(mask1, cv::Point(posX, posY), 10, cv::Scalar(0,255,0), 5);

                line(mask1, cv::Point(640/2, 100), cv::Point(640/2, 480-100), cv::Scalar(0,255,0), 1, CV_AA);
                line(mask1, cv::Point(100, 480/2), cv::Point(640-100, 480/2), cv::Scalar(0,255,0), 1, CV_AA);
                circle(mask1, cv::Point(640/2, 480/2), 50, cv::Scalar(0,255,0));
            }
            else
            {
                line(this->frame, cv::Point(640/2, 100), cv::Point(640/2, 480-100), cv::Scalar(0,0,0), 1, CV_AA);
                line(this->frame, cv::Point(100, 480/2), cv::Point(640-100, 480/2), cv::Scalar(0,0,0), 1, CV_AA);
                circle(this->frame, cv::Point(640/2, 480/2), 100, cv::Scalar(0,0,0));
            }
            this->render_texture(this->frame, drawing);
        }
        this->render_imgui();
    }
}
void TraceMaster::render_texture(const cv::Mat& frame, const cv::Mat& frame2)
{
    glDeleteTextures(1, &this->texture); //Delete the last allocated texture before creating a new one
    glDeleteTextures(1, &this->texture2); //Delete the last allocated texture before creating a new one
    //Creates a textures and renders primitives to it
    /*int xSize = 640;
    int ySize = 480; //size of texture
    //new array
    char* colorBits = new char[ xSize * ySize * 3 ];
    //texture creation..
    glGenTextures(1,&this->texture);
    glBindTexture(GL_TEXTURE_2D,this->texture);
    glTexImage2D(GL_TEXTURE_2D, 0 ,3 ,xSize, ySize, 0 , GL_RGB, GL_UNSIGNED_BYTE, colorBits);
    //you can set other texture parameters if you want
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //clean up
    delete[] colorBits;
    glEnable(GL_TEXTURE_2D);                    // Enable 2D Texture Mapping
    glBindTexture(GL_TEXTURE_2D,this->texture);
    glMatrixMode(GL_PROJECTION);
    //glOrtho( -512/2, 512/2, -512/2, 512/2, -1,1 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, 640, 480);
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    //matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
    glBegin(GL_LINES);
        glColor3f(0.5, 0.5, 0.5);
        glVertex2f(0, 0);
        glVertex2f(512, 512);
    glEnd();

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, xSize, ySize, 0);
    glDisable(GL_TEXTURE_2D);*/
    this->texture = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
    this->texture2 = matToTexture(frame2, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);
}
void TraceMaster::render_imgui()
{
    ImGui::Begin("TraceMaster");
    ImGui::Image((void*)(intptr_t)this->texture, ImVec2(640,480));
    ImGui::SameLine();
    ImGui::Image((void*)(intptr_t)this->texture2, ImVec2(640,480));
    ImGui::End();
}
