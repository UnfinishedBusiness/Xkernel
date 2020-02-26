#ifndef MOTION_CONTROL_H
#define MOTION_CONTROL_H

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include <string>
#include <deque>
#include <serial/serial.h>
#include <json/json.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
using json = nlohmann::json;

struct motion_error_t{
    int number;
    std::string meaning;
    std::string line; 
};
struct offset_t{
    double x;
    double y;
    double z;
};

class motion_parameters_t{
    public:
        float step_pulse_time = 50;

        bool x_axis_step_invert = false;
        bool y1_axis_step_invert = false;
        bool y2_axis_step_invert = false;
        bool z_axis_step_invert = false;

        bool x_axis_dir_invert = false;
        bool y1_axis_dir_invert = false;
        bool y2_axis_dir_invert = false;
        bool z_axis_dir_invert = false;

        float junction_deviation = 0.010;

        float x_step_scale = 518.0;
        float y_step_scale = 518.0;
        float z_step_scale = 2540.0;

        float x_max_vel = 800.0;
        float y_max_vel = 800.0;
        float z_max_vel = 70.0;

        float x_max_accel = 8.0;
        float y_max_accel = 8.0;
        float z_max_accel = 12.0;
};

class MotionControl{
    public:
        offset_t work_offset;
        motion_parameters_t parameters;
        void set_port(std::string p);
        void set_baudrate(int b);
        void set_dro_interval(int ms);
        
        /* Control & Command */
        void send_byte(uint8_t b);
        void send_rt(std::string s);
        void send(std::string s);
        void soft_reset();
        void cycle_start();
        void feed_hold();
        void feedrate_overide_set_100();
        void feedrate_overide_plus_10();
        void feedrate_overide_minus_10();
        void spindle_stop();
        void comp_torch_plus(); //Special for AVTHC
        void comp_torch_minus(); //Special for AVTHC
        void comp_torch_cancel(); //Special for AVTHC
        void send_parameters();
        void abort();
        /*********************/
        json get_dro();
        json get_errors();
        void clear_errors();
        bool is_connected();
        void clear_stack();

        void process_line(std::string line);
        void tick();
        void init();
    private:
        bool soft_reset_upon_idle;
        bool idle_fire_once;
        std::string upper_string(const std::string& str);
        std::string GetErrorMeaning(int error);
        void removeSubstrs(std::string& s, std::string p);
        std::vector<motion_error_t> error_stack;
        std::deque<std::string> motion_stack;
        std::string last_sent;
        std::string read_line;
        json current_dro;
        serial::Serial serial;
        uint64_t delay_timer;
        uint64_t reconnect_timer;
        bool waiting_for_okay;
        bool waiting_for_connect;
        bool is_connected_flag;
        std::string port_description;
        int baudrate;
        uint64_t millis();
        uint64_t dro_interval_timer;
        uint64_t dro_interval;
        void delay(int ms);
        void recieved_ok();
        uint32_t crc32c(uint32_t crc, const char *buf, size_t len);
};

extern MotionControl motion_control;

/**********************
 * VIEWER PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif