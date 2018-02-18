#include "lights/litake_basic_light.hh"
#include "dmx.hh"
#include "serial.hh"

#include <iostream>

int main()
{
    lights::litake_basic_light light(1);
    light.set_color(255, 0, 255);

    auto data = dmx::dmx_helper::generate_message_from_channels(light.get_channels());

    serial::serial_connection connection(dmx::BAUDRATE);
    connection.write_data(data);
}
