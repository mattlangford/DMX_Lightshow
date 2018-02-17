#include "dmx.hh"
#include "bit_convert.hh"

namespace dmx
{
serial::ByteVector_t dmx_helper::generate_message_from_channel(const channels_t& channels)
{
    // we will keep an ordered buffer of the channel data in this vector, which will be transformed into
    // bits, headers added, and then serialized out
    // TODO: Might need to do some stuff with the zero channel
    std::vector<uint8_t> ordered_channels;
    for (const channel_t& channel : channels)
    {
        // only resize if we need to
        if (channel.address > ordered_channels.size())
            ordered_channels.resize(channel.address + 1, 0);

        ordered_channels[channel.address] = channel.level;
    }

    std::vector<bool> full_bit_vector;

    // copy header, easy since it's at the start
    static const std::vector<bool> header = generate_header();
    std::copy(header.begin(), header.end(), std::back_inserter(full_bit_vector));

    // copy each channel in, a little more difficult
    for (const uint8_t& channel : ordered_channels)
    {
        const std::vector<bool> channel_bits = generate_channel(channel);
        std::copy(channel_bits.begin(), channel_bits.end(), std::back_inserter(full_bit_vector));
    }

    auto message = packet_bits(full_bit_vector);
    return {message.begin(), message.end()};
}

//
// ############################################################################
//

std::vector<bool> dmx_helper::generate_header()
{
    std::vector<bool> header(HEADER_LENGTH);

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

std::vector<bool> dmx_helper::generate_channel(const uint8_t level)
{
    // make sure we're at 250kb/s
    constexpr size_t CHANNEL_BITS_250KBS = 11;
    static_assert(CHANNEL_BITS == CHANNEL_BITS_250KBS, "Not supporting non-250kbs baud rate for the time being");

    std::vector<bool> channel(CHANNEL_BITS_250KBS);

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

//
// ############################################################################
//

std::vector<uint8_t> dmx_helper::packet_bits(std::vector<bool> bits)
{
    const size_t num_bytes = bits.size() / BYTE_SIZE;

    std::vector<uint8_t> bytes(num_bytes, 0);
    for (size_t byte = 0; byte < num_bytes; ++byte)
    {
        for (size_t bit = 0; bit < BYTE_SIZE; --bit)
        {
            bytes[byte] += bits[byte * BYTE_SIZE + bit] << bit;
        }
    }

    return bytes;
}
}
