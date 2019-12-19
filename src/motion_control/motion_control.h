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

        void send_rt(std::string s);
        void send(std::string s);

        json get_dro();

        void process_line(std::string line);
        void tick();
        void init();
    private:
        std::deque<std::string> motion_stack;
        std::string read_line;
        serial::Serial serial;
        uint64_t delay_timer;
        uint64_t reconnect_timer;
        bool waiting_for_okay;
        bool is_connected;
        std::string port_description;
        int baudrate;
        uint64_t millis();
        void delay(int ms);
};

/**********************
 * VIEWER PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif