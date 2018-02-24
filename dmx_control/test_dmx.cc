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

    lights::light_universe_controller::controller_params params;
    params.control = lights::light_universe_controller::control_type::MANUAL;
    params.enforce_44hz = true;
    lights::light_universe_controller universe(connection, params);

    universe.add_light_to_universe(light);

    while (true)
    {
        universe.do_update();
    }

}
