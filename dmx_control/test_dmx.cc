#include "lights/litake_basic_light.hh"
#include "light_universe_controller.hh"
#include "dmx.hh"
#include "serial.hh"

#include <iostream>
#include <thread>

int main()
{
    lights::litake_basic_light::ptr light = std::make_shared<lights::litake_basic_light>(1);
    light->set_color(0, 255, 0);

    serial::serial_connection connection(dmx::BAUDRATE);
    lights::light_universe_controller universe(connection, 0.02);
    universe.add_light_to_universe(light);

    while (true)
    {
        light->set_color(255, 0, 0);
        std::this_thread::sleep_for(std::chrono::duration<double>(1));
        light->set_color(0, 255, 0);
        std::this_thread::sleep_for(std::chrono::duration<double>(1));
        light->set_color(0, 0, 255);
        std::this_thread::sleep_for(std::chrono::duration<double>(1));
    }

}
