#pragma once
#include "ftd2xx.h"
#include <memory>
#include <vector>

namespace serial
{

//
// Public type used by others when writing data to the board
//
using ByteVector_t = std::vector<BYTE>;

//
// Connects to an FTDI serial connection and has some nice wrappers C++11 around the
// gross C
//
class SerialConnection
{
public: // constructor ////////////////////////////////////////////////////////
    SerialConnection(const size_t baudrate, const size_t device_number = 0);

    SerialConnection(const SerialConnection &s);

    ~SerialConnection();

public: // public methods /////////////////////////////////////////////////////
    //
    // Simple c++11 wrapper to write some data and check that it went through.
    // Give it a byte vector that you don't care about since it'll eat it
    //
    bool write_data(ByteVector_t data) const;

private: // methods ///////////////////////////////////////////////////////////
    //
    // Makes sure the status return FT_OK
    //
    inline bool status_okay(const FT_STATUS ft_status) const;

private: // members ///////////////////////////////////////////////////////////

    //
    // Handle to the device
    //
    FT_HANDLE ft_handle;

};

} // namespace serial
