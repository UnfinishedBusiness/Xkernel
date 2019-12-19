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

class MotionControl{
    public:
        void set_port(std::string p);
        void set_baudrate(int b);
        void set_dro_interval(int ms);
        
        void send_rt(std::string s);
        void send(std::string s);

        json get_dro();
        bool is_connected();

        void process_line(std::string line);
        void tick();
        void init();
    private:
        std::deque<std::string> motion_stack;
        std::string read_line;
        std::string current_dro;
        serial::Serial serial;
        uint64_t delay_timer;
        uint64_t reconnect_timer;
        bool waiting_for_okay;
        bool is_connected_flag;
        std::string port_description;
        int baudrate;
        uint64_t millis();
        uint64_t dro_interval_timer;
        uint64_t dro_interval;
        void delay(int ms);
};

extern MotionControl motion_control;

/**********************
 * VIEWER PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif