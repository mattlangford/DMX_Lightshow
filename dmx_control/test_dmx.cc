#include "serial.hh"
#include "dmx.hh"


int main()
{
    serial::serial_connection connection(dmx::BAUDRATE);
    connection.write_data({0x00, 0x10, 0x15, 0x23});
}
