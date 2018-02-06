#pragma once
#include "serial.hh"

#include <bitset>
#include <vector>

namespace dmx
{
static constexpr size_t BAUDRATE = 250000;

class dmx_helper
{
public: ///////////////////////////////////////////////////////////////////////
    // Vector of up to 512 channels
    struct channel_t
    {
        uint16_t address; // up to 512
        uint8_t level;
    };
    typedef std::vector<channel_t> channels_t;

    // Serialize the channels into a byte stream to send over the wire, this will include the header
    static serial::ByteVector_t generate_message_from_channel(const channels_t& channels);

private: //////////////////////////////////////////////////////////////////////
    // Header is 28 bits, with a break followed by the mark after break (MAB)
    static constexpr double BREAK_SEQ_LEN_SECONDS = 100E-6;
    static constexpr size_t BREAK_SEQ_LEN_BITS = BAUDRATE * BREAK_SEQ_LEN_SECONDS;
    static constexpr double MAB_SEQ_LEN_SECONDS = 12E-6;
    static constexpr size_t MAB_SEQ_LEN_BITS = BAUDRATE * MAB_SEQ_LEN_SECONDS;
    static constexpr size_t HEADER_LENGTH = BREAK_SEQ_LEN_BITS + MAB_SEQ_LEN_BITS;

    // Each channel is 11 bits (1 low start bit, 8 data bits, 2 high stop bits)
    static constexpr double CHANNEL_SECONDS = 44E-6;
    static constexpr size_t CHANNEL_BITS = BAUDRATE * CHANNEL_SECONDS;

    // Generate the DMX header that goes at the start of each packet. This only needs to be generated once and
    // is all done at compile time at >= -O1
    static std::bitset<HEADER_LENGTH> generate_header();

    // Convert a single value from between 0 and 255 to a bitset properly sized for the BAUDRATE
    static std::bitset<CHANNEL_BITS> generate_channel(const uint8_t level);
};
}
