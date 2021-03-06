#pragma once

#include <complex>
#include <valarray>

#include <boost/circular_buffer.hpp>

namespace fft
{

//
// I'm asserting that the SAMPLE_RATE is a whole non-float number
//
template <typename T, size_t SAMPLE_RATE>
struct fft_helpers
{

public: // types //////////////////////////////////////////////////////////////
typedef std::complex<T> ComplexT;
typedef std::valarray<ComplexT> CArray;

//
// A single frequency bin
//
struct frequency_bin_t
{
    double frequency; // hz
    double amplitude; // dB? Not sure
};

public: // methods ////////////////////////////////////////////////////////////

//
// Get the index in the bins array that corresponds to a particular frequency
// This assumes the frequency bins are spaced equally!
//
static inline size_t index_from_frequency(const double frequency, const std::vector<frequency_bin_t>& bins)
{
    if (bins.size() == 0)
    {
        throw "can't find index_from_frequency with zero bins";
    }

    const double frequency_factor = double(bins.size()) / SAMPLE_RATE;
    return static_cast<size_t>((frequency - bins[0].frequency) * frequency_factor);
}

//
// Get the frequency that corresponds to a particular index
// This assumes the frequency bins are spaced equally!
//
static inline double frequency_from_index(const size_t index, const std::vector<frequency_bin_t>& bins)
{
    if (bins.size() == 0)
    {
        throw "can't find frequency_from_index with zero bins";
    }

    const double frequency_factor = double(SAMPLE_RATE) / (2 * bins.size());
    return index * frequency_factor - bins[0].frequency;
}

//
// Compute FFT on time domain sample non-complex number. I'm using a circular buffer since that's
// how the data will be formatted
//
static std::vector<frequency_bin_t> compute_fft(const boost::circular_buffer<T>& time_domain_samples)
{
    //
    // Take the raw float data and create a complex array with it
    //
    std::vector<std::complex<T>> complex_time_domain_samples;
    complex_time_domain_samples.reserve(time_domain_samples.size());
    for (const T sample : time_domain_samples)
    {
        complex_time_domain_samples.emplace_back(sample);
    }

    return compute_fft(complex_time_domain_samples);
}

//
// Save some compute, if you've already got a complex vector
//
static std::vector<frequency_bin_t> compute_fft(const std::vector<ComplexT>& time_domain_samples)
{
    //
    // Compute the FFT over the complex data
    //
    CArray transformed_complex_data{time_domain_samples.data(), time_domain_samples.size()};
    fft(transformed_complex_data);

    //
    // Convert back from complex to just the magnitudes of the complex numbers
    //
    const size_t frequency_bins = transformed_complex_data.size() / 2;

    std::vector<frequency_bin_t> bins;
    bins.resize(frequency_bins);
    for (size_t i = 0; i < frequency_bins; ++i)
    {
        bins[i].frequency = frequency_from_index(i, bins);
        bins[i].amplitude = std::norm(transformed_complex_data[i]) / frequency_bins;
    }

    return bins;
}

//
// Generate a map: frequency->magnitude in some frequency range
//
static std::vector<frequency_bin_t> get_frequencies_in_range(const double frequency_min,
                                                          const double frequency_max,
                                                          const std::vector<frequency_bin_t>& fft_data)
{
    const size_t index_min = index_from_frequency(frequency_min, fft_data);
    const size_t index_max = index_from_frequency(frequency_max, fft_data);

    if (index_min >= fft_data.size() || index_max >= fft_data.size() || index_min > index_max)
    {
        throw "can't get frequencies in range!";
    }

    std::vector<frequency_bin_t> in_range;
    for(size_t i = index_min; i <= index_max; ++i)
    {
        in_range.push_back(fft_data[i]);
    }

    return in_range;
}

private: // methods ///////////////////////////////////////////////////////////
//
// Stolen from:
// https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
//
static void fft(CArray& x)
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
        std::complex<T> t = std::polar<T>(1.0, -2 * M_PI * k / N) * odd[k];
        x[k    ] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}

};
} // namespace fft
