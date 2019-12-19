#include <motion_control/motion_control.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <cmath>
#include <string>
#include <algorithm>
#include <json/json.h>
#include <serial/serial.h>

using json = nlohmann::json;

uint64_t MotionControl::millis()
{
    using namespace std::literals;
    using Clock = std::chrono::system_clock;
    auto point1 = Clock::now();
    return point1.time_since_epoch() / 1ms;
}
void MotionControl::delay(int ms)
{
    this->delay_timer = this->millis();
    while((this->millis() - this->delay_timer) < ms);
}

void MotionControl::set_port(std::string p)
{
    this->port_description = p;
}
void MotionControl::set_baudrate(int b)
{
    this->baudrate = b;
}
void MotionControl::send_rt(std::string s)
{
    if (this->serial.isOpen())
    {
        try{
            this->serial.write(s);
        }
        catch(...){
            //std::cout << "write exception!\n";
        }
    }
}
void MotionControl::send(std::string s)
{
    if (this->waiting_for_okay == true)
    {
        this->motion_stack.push_back(s);
    }
    else
    {
        this->send_rt(s);
        this->waiting_for_okay = true;
    }
}
json MotionControl::get_dro()
{
    json j;

    return j;
}
void MotionControl::process_line(std::string line)
{
    if (line.find("ok") != std::string::npos)
    {
        if (this->motion_stack.size() > 0)
        {
            this->send_rt(this->motion_stack.at(0));
            this->motion_stack.pop_front();
        }
        else
        {
            this->waiting_for_okay = false;
        }
    }
}
void MotionControl::tick()
{
    if (serial.isOpen())
    {
        try{
            int bytes_available = serial.available();
            if (bytes_available > 0)
            {
                std::string read = serial.read(bytes_available);
                for (int x = 0; x < read.size(); x++)
                {
                    if (read.at(x) == '\n' || read.at(x) == '\r')
                    {
                        this->process_line(this->read_line);
                        this->read_line = "";
                    }
                    else
                    {
                        this->read_line.push_back(read.at(x));
                    }
                }
            }
        }
        catch(...){
            //std::cout << "available exception!\n";
            /* if a disconnect happens we need to close the port so is_open returns false */
            serial.close();
        }
    }
    if ((this->millis() - this->reconnect_timer) > 3000)
    {
        if (this->is_connected == false && this->port_description != "")
        { 
            std::vector<serial::PortInfo> devices_found = serial::list_ports();
            std::vector<serial::PortInfo>::iterator iter = devices_found.begin();
            while( iter != devices_found.end() )
            {
                serial::PortInfo device = *iter++;
                if (device.description.find(this->port_description) != std::string::npos)
                {
                    try{
                        serial.setPort(device.port.c_str());
                        serial.setBaudrate(this->baudrate);
                        serial.open();
                        if (serial.isOpen())
                        {
                            this->is_connected = true;
                        }
                    } catch (...) {
                        // ...
                    }
                }
            }
        }
        else
        {
            if (!serial.isOpen())
            {
                this->is_connected = false;
            }
        }
        this->reconnect_timer = this->millis();
    }
}
void MotionControl::init()
{
    this->delay_timer = 0;
    this->reconnect_timer = 0;
    this->waiting_for_okay = false;
    this->is_connected = false;
    this->port_description = "";
    this->read_line = "";
    this->baudrate = 115200;
}