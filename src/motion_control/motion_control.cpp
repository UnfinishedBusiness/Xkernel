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

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78
uint32_t MotionControl::crc32c(uint32_t crc, const char *buf, size_t len)
{
    int k;

    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
    }
    return ~crc;
}

void MotionControl::soft_reset()
{
    this->send_byte(0x18);
}
void MotionControl::cycle_start()
{
    this->send_byte('~');
}
void MotionControl::feed_hold()
{
    this->send_byte('!');
}
void MotionControl::feedrate_overide_set_100()
{
    this->send_byte(0x90);
}
void MotionControl::feedrate_overide_plus_10()
{
    this->send_byte(0x91);
}
void MotionControl::feedrate_overide_minus_10()
{
    this->send_byte(0x92);
}
void MotionControl::spindle_stop()
{
    this->send_byte(0x9E);
}
void MotionControl::comp_torch_plus() //Special for AVTHC
{
    this->send_byte('>');
}
void MotionControl::comp_torch_minus() //Special for AVTHC
{
    this->send_byte('<');
}
void MotionControl::comp_torch_cancel() //Special for AVTHC
{
    this->send_byte('^');
}
std::string MotionControl::GetErrorMeaning(int error)
{
    std::string ret;
    switch(error)
    {
        case 1: ret = "G-code words consist of a letter and a value. Letter was not found"; break;
        case 2: ret = "Numeric value format is not valid or missing an expected value."; break;
        case 3: ret = "System command was not recognized or supported."; break;
        case 4: ret = "Negative value received for an expected positive value."; break;
        case 5: ret = "Homing cycle is not enabled via settings."; break;
        case 6: ret = "Minimum step pulse time must be greater than 3usec"; break;
        case 7: ret = "EEPROM read failed. Reset and restored to default values."; break;
        case 8: ret = "Real-Time command cannot be used unless machine is IDLE. Ensures smooth operation during a job."; break;
        case 9: ret = "G-code locked out during alarm or jog state"; break;
        case 10: ret = "Soft limits cannot be enabled without homing also enabled."; break;
        case 11: ret = "Max characters per line exceeded. Line was not processed and executed."; break;
        case 12: ret = "Setting value exceeds the maximum step rate supported."; break;
        case 13: ret = "Safety door detected as opened and door state initiated."; break;
        case 14: ret = "Build info or startup line exceeded EEPROM line length limit."; break;
        case 15: ret = "Jog target exceeds machine travel. Command ignored."; break;
        case 16: ret = "Jog command with no '=' or contains prohibited g-code."; break;
        case 17: ret = "Laser mode requires PWM output."; break;
        case 20: ret = "Unsupported or invalid g-code command found in block."; break;
        case 21: ret = "More than one g-code command from same modal group found in block."; break;
        case 22: ret = "Feed rate has not yet been set or is undefined."; break;
        case 23: ret = "G-code command in block requires an integer value."; break;
        case 24: ret = "Two G-code commands that both require the use of the XYZ axis words were detected in the block."; break;
        case 25: ret = "A G-code word was repeated in the block."; break;
        case 26: ret = "A G-code command implicitly or explicitly requires XYZ axis words in the block, but none were detected."; break;
        case 27: ret = "N line number value is not within the valid range of 1 - 9,999,999."; break;
        case 28: ret = "A G-code command was sent, but is missing some required P or L value words in the line."; break;
        case 29: ret = "System only supports six work coordinate systems G54-G59. G59.1, G59.2, and G59.3 are not supported."; break;
        case 30: ret = "The G53 G-code command requires either a G0 seek or G1 feed motion mode to be active. A different motion was active."; break;
        case 31: ret = "There are unused axis words in the block and G80 motion mode cancel is active."; break;
        case 32: ret = "A G2 or G3 arc was commanded but there are no XYZ axis words in the selected plane to trace the arc."; break;
        case 33: ret = "The motion command has an invalid target. G2, G3, and G38.2 generates this error, if the arc is impossible to generate or if the probe target is the current position."; break;
        case 34: ret = "A G2 or G3 arc, traced with the radius definition, had a mathematical error when computing the arc geometry. Try either breaking up the arc into semi-circles or quadrants, or redefine them with the arc offset definition."; break;
        case 35: ret = "A G2 or G3 arc, traced with the offset definition, is missing the IJK offset word in the selected plane to trace the arc."; break;
        case 36: ret = "There are unused, leftover G-code words that aren't used by any command in the block."; break;
        case 37: ret = "The G43.1 dynamic tool length offset command cannot apply an offset to an axis other than its configured axis. The Grbl default axis is the Z-axis."; break;
        case 38: ret = "Tool number greater than max supported value."; break;
        case 100: ret = "Communication Redundancy Check Failed. Program Aborted to ensure unintended behavior does not occur!"; break;
        default: ret = "unknown"; break;
    }
    return ret;
}
void MotionControl::removeSubstrs(std::string& s, std::string p) { 
  std::string::size_type n = p.length();
  for (std::string::size_type i = s.find(p);
      i != std::string::npos;
      i = s.find(p))
      s.erase(i, n);
}
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
void MotionControl::send_byte(uint8_t b)
{
    if (this->waiting_for_connect == false)
    {
        if (this->serial.isOpen())
        {
            try{
                std::vector<uint8_t> bytes;
                bytes.push_back(b);
                this->serial.write(bytes);
            }
            catch(...){
                //std::cout << "write exception!\n";
            }
        }
    }
}
void MotionControl::send_rt(std::string s)
{
    if (this->waiting_for_connect == false)
    {
        if (this->serial.isOpen())
        {
            try{
                if (s == "?")
                {
                    this->serial.write(s + "\n"); //Status report should not override last_sent because it will break echo redundancy check!
                }
                else
                {
                    s.erase(remove(s.begin(), s.end(), ' '), s.end());
                    s.erase(remove(s.begin(), s.end(), '\n'), s.end());
                    s.erase(remove(s.begin(), s.end(), '\r'), s.end());
                    uint32_t checksum = this->crc32c(0, s.c_str(), s.size());
                    //std::cout << s + "*" + std::to_string(checksum) + "\n";
                    last_sent = s + "*" + std::to_string(checksum) + "\n";
                    this->serial.write(s + "*" + std::to_string(checksum) + "\n");
                }
            }
            catch(...){
                //std::cout << "write exception!\n";
            }
        }
    }
}
void MotionControl::send(std::string s)
{
    if (this->waiting_for_connect == false)
    {
        if (this->serial.isOpen())
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
    }
}
void MotionControl::send_parameters()
{
    using namespace std;
    uint8_t step_invert_mask = 0b00000000;
    uint8_t dir_invert_mask = 0b00000000;
    if (this->parameters.x_axis_step_invert) step_invert_mask |= (1 << 0);
    if (this->parameters.y1_axis_step_invert) step_invert_mask |= (1 << 1);
    if (this->parameters.z_axis_step_invert) step_invert_mask |= (1 << 2);
    if (this->parameters.y2_axis_step_invert) step_invert_mask |= (1 << 3);

    if (this->parameters.x_axis_dir_invert) dir_invert_mask |= (1 << 0);
    if (this->parameters.y1_axis_dir_invert) dir_invert_mask |= (1 << 1);
    if (this->parameters.z_axis_dir_invert) dir_invert_mask |= (1 << 2);
    if (this->parameters.y2_axis_dir_invert) dir_invert_mask |= (1 << 3);

    this->send("$0=" + to_string(this->parameters.step_pulse_time));
    this->send("$2=" + to_string(step_invert_mask));
    this->send("$3=" + to_string(dir_invert_mask));
    this->send("$11=" + to_string(this->parameters.junction_deviation));
    this->send("$100=" + to_string(this->parameters.x_step_scale));
    this->send("$101=" + to_string(this->parameters.y_step_scale));
    this->send("$102=" + to_string(this->parameters.z_step_scale));
    this->send("$110=" + to_string(this->parameters.x_max_vel));
    this->send("$111=" + to_string(this->parameters.y_max_vel));
    this->send("$112=" + to_string(this->parameters.z_max_vel));
    this->send("$120=" + to_string(this->parameters.x_max_accel));
    this->send("$121=" + to_string(this->parameters.y_max_accel));
    this->send("$122=" + to_string(this->parameters.z_max_accel));
}
json MotionControl::get_dro()
{
   return this->current_dro;
}
json MotionControl::get_errors()
{
    json j;
    for (size_t x = 0; x < this->error_stack.size(); x++)
    {
        this->error_stack[x].meaning = this->GetErrorMeaning(this->error_stack[x].number);
        json o;
        o["line"] = this->error_stack[x].line;
        o["number"] = this->error_stack[x].number;
        o["meaning"] = this->error_stack[x].meaning;
        j.push_back(o);
    }
    return j;
}
void MotionControl::clear_errors()
{
    this->error_stack.clear();
}
bool MotionControl::is_connected()
{
    if (this->is_connected_flag == true && this->waiting_for_connect == false) 
    {
        return true;
    }
    else
    {
        return false;
    }
}
void MotionControl::recieved_ok()
{
    this->checksum_retry_count = 0;
    this->waiting_for_okay = false;
    if (this->motion_stack.size() > 0)
    {
        this->send_rt(this->motion_stack.at(0));
        this->motion_stack.pop_front();
    }
}
void MotionControl::process_line(std::string line)
{
    if (line.find("error") != std::string::npos)
    {
        motion_error_t error;
        this->removeSubstrs(line, "error:");
        error.number= atoi(line.c_str());
        error.line = this->last_sent;
        if (error.number != 8) //Error number 8 is dumb, don't log it!
        {
            this->error_stack.push_back(error);
        }
        //printf("(MotionControl::process_line::error) Error Line: \"%s\" #%d\n", error.line.c_str(), error.number);
        this->recieved_ok();
    }
    /*else if (line.find("ok_to_send") != std::string::npos)
    {
        this->recieved_ok();
    }*/
    else if (line.find("Grbl") != std::string::npos)
    {
        this->waiting_for_connect = false;
        //printf("Connected to control and updating work offset!\n");
        this->send_parameters(); //Always sync control parameters
        this->send("G10 L2 P0 X" + std::to_string(this->work_offset.x) + " Y" + std::to_string(this->work_offset.y) + " Z" + std::to_string(this->work_offset.z));
    }
    else if (line.find("{") != std::string::npos)
    {
        try{
            this->current_dro = json::parse(line);
            if (this->current_dro["IN_MOTION"] == false)
            {
                if (this->soft_reset_upon_idle == true)
                {
                    //printf("(MotionControl::process_line) Resetting!\n");
                    this->soft_reset_upon_idle = false;
                    this->delay(150);
                    this->soft_reset();
                }
            }
        }
        catch(...){
            //Do nothing
        }
    }
    /*else if (line.find("[echo: ") != std::string::npos)
    {
        line.erase(0, 7);
        line.erase(line.size() - 1);
        if (line.size() > 0)
        {
            this->last_sent.erase(remove(this->last_sent.begin(), this->last_sent.end(), ' '), this->last_sent.end());
            this->last_sent.erase(remove(this->last_sent.begin(), this->last_sent.end(), '\n'), this->last_sent.end());
            this->last_sent.erase(remove(this->last_sent.begin(), this->last_sent.end(), '\r'), this->last_sent.end());
            if (line != this->last_sent)
            {
                //printf("(MotionControl::echo_redundancy) Critical Communication error! Last Send: %s, Echo Check: \"%s\"\n", this->last_sent.c_str(), line.c_str());
                this->motion_stack.clear();
                this->abort();
                motion_error_t error;
                error.number = 100;
                error.line = this->last_sent;
                this->error_stack.push_back(error);
            }
        }
    }*/ //Depricated. Rare occasions would cause planner to run out of buffer and fail to send be actual line...
    else if (line == "[CHECKSUM_FAILURE]")
    {
        printf("(MotionControl::checksum_failure) on sent line: %s, retry count: %d\n", line.c_str(), this->checksum_retry_count);
        if (this->checksum_retry_count > 5)
        {
            printf("\t\tRetry count exceeded and program aborted!\n");
            this->motion_stack.clear();
            this->abort();
            motion_error_t error;
            error.number = 100;
            error.line = this->last_sent;
            this->error_stack.push_back(error);
        }
        else
        {
            this->send_rt(this->last_sent);
            this->checksum_retry_count++; 
        }
    }
    else
    {
        printf("(MotionControl::process_line) \"%s\"\n", line.c_str());
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
                    if (read.at(x) == '>')
                    {
                        this->recieved_ok();
                    }
                    else if (read.at(x) == '\n' || read.at(x) == '\r')
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
    if ((this->millis() - this->reconnect_timer) > 1000)
    {
        if (this->is_connected_flag == false && this->port_description != "")
        { 
            //printf("Looking for port with %s in description\n\r", this->port_description.c_str());
            std::vector<serial::PortInfo> devices_found = serial::list_ports();
            std::vector<serial::PortInfo>::iterator iter = devices_found.begin();
            while( iter != devices_found.end() )
            {
                serial::PortInfo device = *iter++;
                if (device.description.find(this->port_description) != std::string::npos)
                {
                    //printf("\tFound at port - %s\n\r", device.port.c_str());
                    try{
                        serial.setPort(device.port.c_str());
                        serial.setBaudrate(this->baudrate);
                        auto timeout = serial::Timeout::simpleTimeout(250);
                        serial.setTimeout(timeout);
                        serial.open();
                        if (serial.isOpen())
                        {
                            this->is_connected_flag = true;
                            this->waiting_for_connect = true;
                            serial.setDTR(true);
                            this->delay(100);
                            serial.setDTR(false);
                            printf("\topened port! %s\n\r", device.port.c_str());
                            break;
                        }
                        else
                        {
                            printf("\tcould not open port!\n\r");
                        }
                    } catch (...) {
                        // ...
                    }
                }
                else if (device.description.find("USB") != std::string::npos) //Also check if we are trying to connect to firmware with old 16U2 firmware on windows
                {
                    if (this->port_description == "Arduino")
                    {
                        printf("(motion_control) *Windows* proper description not reported! Falling back to \"USB\"\n\r");
                        try{
                            serial.setPort(device.port.c_str());
                            serial.setBaudrate(this->baudrate);
                            auto timeout = serial::Timeout::simpleTimeout(250);
                            serial.setTimeout(timeout);
                            serial.open();
                            if (serial.isOpen())
                            {
                                this->is_connected_flag = true;
                                this->waiting_for_connect = true;
                                serial.setDTR(true);
                                this->delay(100);
                                serial.setDTR(false);
                                printf("\topened port! %s\n\r", device.port.c_str());
                                break;
                            }
                            else
                            {
                                printf("\tcould not open port!\n\r");
                            }
                        } catch (...) {
                            // ...
                        }
                    }
                }
            }
        }
        else
        {
            if (!serial.isOpen())
            {
                this->is_connected_flag = false;
                this->waiting_for_connect = true;
            }
        }
        this->reconnect_timer = this->millis();
    }
    if ((this->millis() - this->dro_interval_timer) > this->dro_interval)
    {
        if (this->is_connected_flag == true)
        {
            //Check to see if the condition exists that there are items in send stack and we are idle, this indicates that a "ok" signal has been missed and has broke the
            //send stream
            if (this->current_dro["STATUS"] == "Idle" && this->idle_fire_once == false)
            {
                this->idle_fire_once = true;
                //printf("(MotionControl) Idle Fire once!\n");
                if (this->motion_stack.size() > 0)
                {
                    //printf("(MotionControl) Motion stopped and should not have!\n");
                    //this->recieved_ok(); //Pretend we recieved an OK Signal to "un-break" streaming cycle
                }
            }
            if (this->current_dro["STATUS"] != "Idle")
            {
                this->idle_fire_once = false;
            }
            this->send_rt("?");
            this->dro_interval_timer = this->millis();
        }
    }
}
void MotionControl::clear_stack()
{
    this->motion_stack.clear();
    this->waiting_for_okay = false;
}
void MotionControl::abort()
{
    this->soft_reset_upon_idle = true;
    this->feed_hold();
}
void MotionControl::init()
{
    this->checksum_retry_count = 0;
    this->soft_reset_upon_idle = false;
    this->delay_timer = 0;
    this->reconnect_timer = 0;
    this->waiting_for_okay = false;
    this->is_connected_flag = false;
    this->waiting_for_connect = true;
    this->port_description = "";
    this->read_line = "";
    this->current_dro = json::parse("{\"ADC\":0,\"FEED\":0,\"MCS\":{\"x\":0,\"y\":0,\"z\":0},\"WCS\":{\"x\":0,\"y\":0,\"z\":0},\"STATUS\":\"Idle\", \"IN_MOTION\": false, \"ARC_OK\": false}");
    this->baudrate = 115200;
    this->dro_interval_timer = 0;
    this->dro_interval = 200;
    this->work_offset.x = 0;
    this->work_offset.y = 0;
    this->work_offset.z = 0;
}
std::string upper_string(const std::string& str)
{
    std::string upper;
    std::transform(str.begin(), str.end(), std::back_inserter(upper), toupper);
    return upper;
}
