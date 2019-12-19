#include <motion_control/motion_control.h>

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
void MotionControl::set_dro_interval(int ms)
{
    this->dro_interval = ms;
}
void MotionControl::send_rt(std::string s)
{
    if (this->serial.isOpen())
    {
        try{
            this->serial.write(s + "\n");
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
    return json::parse(this->current_dro);
}
bool MotionControl::is_connected()
{
    return this->is_connected_flag;
}
void MotionControl::process_line(std::string line)
{
    //printf("(MotionControl::process_line) \"%s\"\n", line.c_str());
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
    else if (line.find("{") != std::string::npos)
    {
        this->current_dro = line;
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
                        this->read_line.erase(std::remove(this->read_line.begin(), this->read_line.end(), '\n'),this->read_line.end());
                        this->read_line.erase(std::remove(this->read_line.begin(), this->read_line.end(), '\r'),this->read_line.end());
                        if (this->read_line != "") this->process_line(this->read_line);
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
    if ((this->millis() - this->reconnect_timer) > 1500)
    {
        if (this->is_connected_flag == false && this->port_description != "")
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
                            this->is_connected_flag = true;
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
                this->is_connected_flag = false;
            }
        }
        this->reconnect_timer = this->millis();
    }
    if ((this->millis() - this->dro_interval_timer) > this->dro_interval)
    {
        if (this->is_connected_flag == true)
        {
            this->send_rt("?");
            this->dro_interval_timer = this->millis();
        }
    }
}
void MotionControl::init()
{
    this->delay_timer = 0;
    this->reconnect_timer = 0;
    this->waiting_for_okay = false;
    this->is_connected_flag = false;
    this->port_description = "";
    this->read_line = "";
    this->current_dro = "{}";
    this->baudrate = 115200;
    this->dro_interval_timer = 0;
    this->dro_interval = 60;
}