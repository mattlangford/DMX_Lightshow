#include "dmx.hh"
#include "bit_convert.hh"

namespace dmx
{
serial::ByteVector_t dmx_helper::generate_message_from_channel(const channels_t& channels)
{
    // we will keep an ordered buffer of the channel data in this array, then using max address we will know
    // what to serialize
    // TODO: If lights aren't zero indexed, this will need to be 513
    std::array<uint8_t, NUM_CHANNELS> ordered_channels{0};
    for (const channel_t& channel : channels)
    {
        ordered_channels[channel.address] = channel.level;
    }

    std::array<bool, HEADER_LENGTH + CHANNEL_BITS * NUM_CHANNELS> full_message;


    // copy header, easy since it's at the start
    size_t full_message_index = 0;
    static std::array<bool, HEADER_LENGTH> header = generate_header();
    for (; full_message_index < HEADER_LENGTH; ++full_message_index)
    {
        full_message[full_message_index] = header[full_message_index];
    }

    // copy each channel in, a little more difficult
    for (size_t channel_index = 0; channel_index < NUM_CHANNELS; ++channel_index)
    {
        std::array<bool, CHANNEL_BITS> channel = generate_channel(channels[channel_index].level);
        for (size_t channel_bit = 0; channel_bit < CHANNEL_BITS; ++channel_bit, ++full_message_index)
        {
            full_message[full_message_index] = channel[channel_bit];
        }
    }

    auto message = bit_convert::convert(full_message);
    return {message.begin(), message.end()};
}

//
// ############################################################################
//

std::array<bool, dmx_helper::HEADER_LENGTH> dmx_helper::generate_header()
{
    std::array<bool, HEADER_LENGTH> header;
    for (size_t i = 0; i < BREAK_SEQ_LEN_BITS; ++i)
    {
        constexpr bool BREAK_LEVEL = false;
        header[i] = BREAK_LEVEL;
    }
    for (size_t i = 0; i < MAB_SEQ_LEN_BITS; ++i)
    {
        constexpr int MAB_LEVEL = true;
        header[i + BREAK_SEQ_LEN_BITS] = MAB_LEVEL;
    }

    return header;
}

//
// ############################################################################
//

std::array<bool, dmx_helper::CHANNEL_BITS> dmx_helper::generate_channel(const uint8_t level)
{
    // make sure we're at 250kb/s
    constexpr size_t CHANNEL_BITS_250KBS = 11;
    static_assert(CHANNEL_BITS == CHANNEL_BITS_250KBS, "Not supporting non-250kbs baud rate for the time being");

    std::array<bool, CHANNEL_BITS_250KBS> channel;

    // start and stop bits
    constexpr bool START_BIT = false;
    channel[0] = START_BIT; // start bit is low
    constexpr bool STOP_BITS = true;
    channel[CHANNEL_BITS_250KBS - 2] = STOP_BITS; // last two bits are high
    channel[CHANNEL_BITS_250KBS - 1] = STOP_BITS;

    // now fill in the level
    uint8_t mask = 1;
    for (size_t i = 0; i < 8; ++i)
    {
        channel[i + 1] = mask & level;
        mask <<= 1;
    }

    return channel;
}
}
