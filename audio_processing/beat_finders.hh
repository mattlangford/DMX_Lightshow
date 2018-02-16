#pragma once

#include "audio.hh"
#include "fft_helpers.hh"

namespace Beats
{

template <typename T, size_t NUM_SAMPLES>
class Averager
{
public:
inline operator T() const { return avg;}
void operator+=(const T& val)
{
    if (num_samples < NUM_SAMPLES)
    {
        num_samples++;
    }

    avg -= (double) avg / num_samples;
    avg += (double) val / num_samples;
}
private:
    size_t num_samples = 0;
    T avg = 0;
};

typedef fft::fft_helpers<float, audio::SAMPLE_RATE> fft;

class DumbBeatFinder
{
public: // constructor ///////////////////////////////////////////////////////
    DumbBeatFinder() = default;

public: // methods ////////////////////////////////////////////////////////////
    //
    // Add a new sample to the detector
    //
    void add_sample(const std::vector<fft::FrequencyBin>& bins)
    {
        double power = 0;
        for (const fft::FrequencyBin& bin : bins)
        {
            power += bin.amplitude;
        }
        long_average += power;
        short_average += power;

        std::cout << "power: " << power << "\n";
        std::cout << "long_average: " << long_average << "\n";
        std::cout << "short_average: " << short_average << "\n";
        std::cout << "in_beat: " << is_in_beat() << "\n\n";
    }

    //
    // Check if we're inside a beat
    //
    bool is_in_beat() const
    {
        constexpr double BEAT_FACTOR = 2.0;
        return short_average > BEAT_FACTOR * long_average;
    }

private: // members ///////////////////////////////////////////////////////////
    static constexpr size_t NUM_SAMPLES_LONG = 50;
    Averager<double, NUM_SAMPLES_LONG> long_average;

    static constexpr size_t NUM_SAMPLES_SHORT = 1;
    Averager<double, NUM_SAMPLES_SHORT> short_average;
};

} // namespace Beats
