#ifndef TRACE_MASTER_H
#define TRACE_MASTER_H

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class TraceMaster{
    public:
        void init();
        void render();
        void render_cleanup();
    private:
        cv::Mat frame;
        cv::Mat blurred;
        cv::Mat canny;
        cv::Mat final_frame;
        cv::VideoCapture capture;
        bool show;
        unsigned int texture;
        void render_texture(const cv::Mat& frame);
        void render_imgui();
        GLuint matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter);
};

#endif