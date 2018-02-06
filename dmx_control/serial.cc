#include "serial.hh"
#include <assert.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace serial
{

//
// ### constructor ############################################################
//

SerialConnection::SerialConnection(const size_t baudrate, const size_t device_number)
{
    DWORD number_of_devices;
    assert(FT_CreateDeviceInfoList(&number_of_devices) == FT_OK);
    std::cout << number_of_devices << " devices found!" << std::endl;
    if (number_of_devices < 1)
    {
        std::cout << "Need at least one device\n";
        return;
    }

    //
    // Try to open it
    //
    assert(FT_Open(device_number, &ft_handle) == FT_OK);
    std::cout << "Device opened successfully!" << std::endl;


    //
    // Now let's configure our device, to do that first we need to reset the receive buffer
    //
    // Set BAUD rate
    constexpr size_t DMX_BAUDRATE = 250000;
    assert(FT_SetBaudRate(ft_handle, DMX_BAUDRATE) == FT_OK);
    std::cout << "Baudrate set to " << DMX_BAUDRATE << "\n";
    // Flow control set to none
    assert(FT_SetFlowControl(ft_handle, FT_FLOW_NONE, 0, 0) == FT_OK);
    std::cout << "Flow control configured\n";
    // Enable RS485 mode
    constexpr uint8_t RS485_MODE = 0x20;
    constexpr uint8_t PIN_MASK = 0x66;
    assert(FT_SetBitMode(ft_handle, PIN_MASK, RS485_MODE) == FT_OK);
    std::cout << "Bit bang mode configured\n";

    std::cout << "Ready to transmit data!\n";
}

//
// ############################################################################
//

SerialConnection::SerialConnection(const SerialConnection &s)
{
    ft_handle = s.ft_handle;
}

//
// ############################################################################
//

SerialConnection::~SerialConnection()
{
    FT_SetBitMode(ft_handle, 0x0, 0x00);
    FT_Close(ft_handle);
}

//
// ### public methods #########################################################
//

bool SerialConnection::write_data(ByteVector_t data) const
{
    const unsigned int bytes_to_send = data.size();
    unsigned int bytes_sent = 0;
    FT_STATUS ft_status = FT_Write(ft_handle, data.data(), bytes_to_send, &bytes_sent);

    if (status_okay(ft_status) == false)
    {
        return false;
    };

    return bytes_sent == bytes_to_send;
}

//
// ### private methods ########################################################
//

inline bool SerialConnection::status_okay(const FT_STATUS ft_status) const
{
    if (ft_status != FT_OK)
    {
        std::cout << "ERROR: ft_status = " << ft_status << std::endl;
        return false;
    }
    return true;
}

} // namespace serial
