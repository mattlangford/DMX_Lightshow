#include <boost/lockfree/spsc_queue.hpp>
#include <boost/circular_buffer.hpp>

#include <atomic>
#include <chrono>
#include <iostream>
#include <valarray>
#include <complex>

#include "audio.hh"
#include "fft_helpers.hh"
#include "beat_finders.hh"

#include "../dmx_control/light_universe_controller.hh"
#include "../dmx_control/lights/litake_basic_light.hh"

class Timer {
public:
    Timer() :
            m_beg(clock_::now()) {
    }
    void reset() {
        m_beg = clock_::now();
    }

    double elapsed() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                clock_::now() - m_beg).count();
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1>> second_;
    std::chrono::time_point<clock_> m_beg;
};

int record_callback(const void *input_buffer, void *output_buffer,
                    size_t frames_in_buffer,
                    const PaStreamCallbackTimeInfo* time_info,
                    PaStreamCallbackFlags status_flag,
                    void* buffer_ptr)
{
    auto& buffer = *static_cast<boost::lockfree::spsc_queue<float>*>(buffer_ptr);
    bool res = buffer.push(static_cast<const float*>(input_buffer), frames_in_buffer);

    if (res)
    {
        return paContinue;
    }
    else
    {
        for (size_t i = 0; i < frames_in_buffer; ++i)
        {
            std::cout << static_cast<const float*>(input_buffer)[i] << ", ";
        }
        std::cout << std::endl;
        std::cout << "Unable to push back into buffer, is it full?\n";
        return paAbort;
    }
}

int main(void)
{
    constexpr size_t AUDIO_BUFFER_QUEUE_SIZE = 262144;
    boost::lockfree::spsc_queue<float> queue(AUDIO_BUFFER_QUEUE_SIZE);

    constexpr size_t FFT_WINDOW_SIZE = 8192;
    boost::circular_buffer<float> fft_data(FFT_WINDOW_SIZE);

    //
    // The audio manager deals with pulling audio from the sound card. The queue needs to
    // be passed in as a void pointer, which causes some weirdness down the line
    //
    audio::audio_manager manager;
    manager.make_good_record_callback(record_callback, static_cast<void*>(&queue));

    Beats::dumb_beat_finder beat_finder;
    bool last_beat = false;

    typedef fft::fft_helpers<float, audio::SAMPLE_RATE> fft;

    lights::litake_basic_light::ptr light = std::make_shared<lights::litake_basic_light>(1);
    light->set_color(0, 0, 255);

    serial::serial_connection connection(dmx::BAUDRATE);

    lights::light_universe_controller::controller_params params;
    params.control = lights::light_universe_controller::control_type::MANUAL;
    params.enforce_44hz = true;
    lights::light_universe_controller universe(connection, params);

    universe.add_light_to_universe(light);

    Timer t;
    constexpr size_t POP_AMOUNT = 2048;
    float result[POP_AMOUNT];
    while(Pa_IsStreamActive(manager.get_stream()) == 1)
    {
        Pa_Sleep(10);
        size_t popped = queue.pop(result, POP_AMOUNT);

        for (size_t i = 0; i < popped; ++i)
        {
            fft_data.push_back(result[i]);
        }

        if (fft_data.empty())
        {
            std::cout << "No data for FFT, waiting...\n";
            continue;
        }

        if (t.elapsed() < 100)
            continue;
        t.reset();

        std::vector<fft::frequency_bin_t> bins {fft::compute_fft(fft_data)};
        constexpr double MIN_FREQ = 25;
        constexpr double MAX_FREQ = 150;
        std::vector<fft::frequency_bin_t> bandpassed = fft::get_frequencies_in_range(MIN_FREQ, MAX_FREQ, bins);
        beat_finder.add_sample(bandpassed);
        if (beat_finder.is_in_beat())
        {
            // we weren't in a beat already
            if (last_beat == false)
            {
                size_t sum = 0;
                for (const auto b : bandpassed)
                {
                    sum += b.amplitude;
                }
                std::cout << sum << "\n";

                light->set_color(255,0,255);
                universe.do_update();

                last_beat = true;
                std::cout << "BEAT\n";
            }
        }
        else
        {
            light->set_color(0,0,255);
            universe.do_update();
            last_beat = false;
        }
    }
}

