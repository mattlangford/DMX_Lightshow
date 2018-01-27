#include <boost/lockfree/spsc_queue.hpp>
#include <boost/circular_buffer.hpp>

#include <atomic>
#include <chrono>
#include <iostream>
#include <valarray>
#include <complex>

#include "audio.hh"

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



//
// Stolen from rosettacode:
// https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
//
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

void fft(CArray& x)
{
    const size_t N = x.size();
    if (N <= 1) return;

    // divide
    CArray even = x[std::slice(0, N/2, 2)];
    CArray  odd = x[std::slice(1, N/2, 2)];

    // conquer
    fft(even);
    fft(odd);

    // combine
    for (size_t k = 0; k < N/2; ++k)
    {
        Complex t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
        x[k    ] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}

int main(void)
{
    boost::lockfree::spsc_queue<float> queue(std::pow(2, 16));
    boost::circular_buffer<float> fft_data(std::pow(2, 13));

    AudioManager manager;
    manager.make_good_record_callback(record_callback, static_cast<void*>(&queue));

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

        //
        // Take the raw float data and create a complex array with it
        //
        std::vector<Complex> complex_data;
        complex_data.reserve(fft_data.size());
        for (float sample : fft_data)
        {
            complex_data.emplace_back(sample);
        }

        //
        // Compute the FFT over that data
        //
        CArray transformed_complex_data{complex_data.data(), complex_data.size()};
        fft(transformed_complex_data);

        //
        // Convert back from complex to just the magnitudes of the complex numbers
        //
        const size_t frequency_bins = transformed_complex_data.size() / 2;
        const double frequency_factor = SAMPLE_RATE / frequency_bins;
        std::vector<double> magnitudes;
        std::vector<double> frequencies;
        magnitudes.reserve(frequency_bins);
        frequencies.reserve(frequency_bins);
        double max_frequency = 0;
        double max_magnitude = 0;
        for (size_t i = 50; i < frequency_bins; ++i)
        {
            double norm = std::norm(transformed_complex_data[i]);
            if (norm > max_magnitude)
            {
                max_frequency = i * frequency_factor;
                max_magnitude = norm;
            }

            frequencies.push_back(i * frequency_factor);
            magnitudes.push_back(norm);
        }
    }
}

