#include <boost/lockfree/spsc_queue.hpp>
#include <boost/circular_buffer.hpp>

#include <atomic>
#include <chrono>
#include <iostream>
#include <valarray>
#include <complex>

#include "audio.hh"
#include "fft_helpers.hh"

int record_callback(const void *input_buffer, void *output_buffer,
                    size_t frames_in_buffer,
                    const PaStreamCallbackTimeInfo* time_info,
                    PaStreamCallbackFlags status_flag,
                    void* buffer_ptr)
{
    auto& buffer = *static_cast<boost::lockfree::spsc_queue<float>*>(buffer_ptr);
    bool res = buffer.push(static_cast<const float*>(input_buffer), frames_in_buffer);
    return res ? paContinue : paAbort;
}

int main(void)
{
    constexpr size_t AUDIO_BUFFER_QUEUE_SIZE = 65536;
    boost::lockfree::spsc_queue<float> queue(AUDIO_BUFFER_QUEUE_SIZE);

    constexpr size_t FFT_WINDOW_SIZE = 8192;
    boost::circular_buffer<float> fft_data(FFT_WINDOW_SIZE);

    //
    // The audio manager deals with pulling audio from the sound card. The queue needs to
    // be passed in as a void pointer, which causes some weirdness down the line
    //
    audio::AudioManager manager;
    manager.make_good_record_callback(record_callback, static_cast<void*>(&queue));

    typedef fft::fft_helpers<float, audio::SAMPLE_RATE> fft;

    while(Pa_IsStreamActive(manager.get_stream()) == 1)
    {
        Pa_Sleep(10);

        constexpr size_t POP_AMOUNT = 512 * 10;
        float result[POP_AMOUNT];
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

        double max_amp = 0.0;
        double max_freq = 0.0;
        for (const auto& bin : fft::compute_fft(fft_data))
        {
            if (bin.amplitude > max_amp)
            {
                max_freq = bin.frequency;
                max_amp = bin.amplitude;
            }
        }
        std::cout << "Max amplitude: " << max_amp << "\t at f=" << max_freq << "\n";
    }
}

