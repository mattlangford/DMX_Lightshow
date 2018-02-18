#include "light_universe_controller.hh"
#include "dmx.hh"

#include <iostream>

namespace lights
{

light_universe_controller::light_universe_controller(serial::serial_connection& connection,
                                                     double update_period)
    : connection_(connection),
      update_period_(std::chrono::duration<double>(update_period)),
      executive_handle_([this](){executive_thread();})
{
    valid_addresses_.fill(true);
}

//
// ############################################################################
//

void light_universe_controller::add_light_to_universe(light_base::ptr light)
{
    for (size_t i = light->get_start_address(); i <= light->get_end_address(); ++i)
    {
        if (valid_addresses_[i] == false)
        {
            throw std::runtime_error("Multiple lights are sharing the address (" + std::to_string(i) + ").");
        }

        valid_addresses_[i] = false;
    }

    lights_.emplace_back(std::move(light));
}

//
// ############################################################################
//

void light_universe_controller::do_update()
{
    dmx::dmx_helper::channels_t channels;
    channels.reserve(dmx::MAX_NUM_CHANNELS);

    for (std::shared_ptr<light_base>& light : lights_)
    {
        for (auto&& channel : light->get_channels())
        {
            channels.emplace_back(std::move(channel));
        }
    }

    serial::ByteVector_t data_to_send = dmx::dmx_helper::generate_message_from_channels(channels);
    connection_.write_data( dmx::dmx_helper::generate_message_from_channels(channels););
}

//
// ############################################################################
//

void light_universe_controller::executive_thread()
{
    while (true)
    {
        std::this_thread::sleep_for(update_period_);
        std::cout << "Update!\n";
        do_update();
    }
}

}
