#include "dmx.hh"

namespace dmx
{
serial::ByteVector_t dmx_helper::generate_message_from_channel(const channels_t& channels)
{
    // we will keep an ordered buffer of the channel data in this array, then using max address we will know
    // what to serialize
    // TODO: If lights aren't zero indexed, this will need to be 513
    std::array<uint8_t, 512> ordered_channels;
    size_t max_address = 0;
    for (const channel_t& channel : channels)
    {
        if (channel.address > max_address)
        {
            max_address = channel.address;
        }

        ordered_channels[channel.address] = channel.level;
    }


}

//
// ############################################################################
//

std::bitset<dmx_helper::HEADER_LENGTH> dmx_helper::generate_header()
{
    std::bitset<HEADER_LENGTH> header;
    for (size_t i = 0; i < BREAK_SEQ_LEN_BITS; ++i)
    {
        constexpr int BREAK_LEVEL = 0;
        header[i] = BREAK_LEVEL;
    }
    for (size_t i = 0; i < MAB_SEQ_LEN_BITS; ++i)
    {
        constexpr int MAB_LEVEL = 1;
        header[i + BREAK_SEQ_LEN_BITS] = MAB_LEVEL;
    }

    return header;
}

//
// ############################################################################
//

std::bitset<dmx_helper::CHANNEL_BITS> dmx_helper::generate_channel(const uint8_t level)
{
    // first we will make a bitset where each bit corresponds to a single actual bit, the we scale up to the proper baudrate
    constexpr size_t CHANNEL_BITS_250KBS = 11;
    std::bitset<CHANNEL_BITS_250KBS> channel;
    channel[0] = 0; // start bit is low
    channel[CHANNEL_BITS_250KBS - 2] = 1; // last two bits are high
    channel[CHANNEL_BITS_250KBS - 1] = 1;

    // now fill in the level
    std::bitset<8> level_bits {level};
    for (size_t i = 0; i < 8; ++i)
    {
        channel[i + 1] = level_bits[i];
    }

    static_assert(CHANNEL_BITS == CHANNEL_BITS_250KBS, "Not supporting non-250kbs baud rate for the time being");
    return channel;
}
}
