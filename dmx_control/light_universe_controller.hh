#pragma once
#include "lights/light_base.hh"
#include "serial.hh"

#include <memory>
#include <thread>

namespace lights
{

class light_universe_controller
{
public:
    light_universe_controller(serial::serial_connection& connection, double update_period);

public:
    // add a new light, return if the add was successful
    void add_light_to_universe(light_base::ptr light);

    // preform an update
    void do_update();

private:
    // runs do_update every update_period seconds
    void executive_thread();

private:
    // connection to the board that interfaces with the lights
    serial::serial_connection& connection_;

    // how long to sleep after each update
    std::chrono::duration<double> update_period_;

    // all of the lights registered, each timestep will get
    std::vector<light_base::ptr> lights_;
    std::array<bool, dmx::MAX_NUM_CHANNELS> valid_addresses_;

    // handle to the main executive runner thread
    std::thread executive_handle_;
};

}
