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

public: // methods ////////////////////////////////////////////////////////////

//
// Compute FFT on time domain sample non-complex numbers
//
static std::vector<T> compute_fft(const boost::circular_buffer<T>& time_domain_samples)
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
static std::vector<T> compute_fft(const std::vector<ComplexT>& time_domain_samples)
{
    //
    // Compute the FFT over that data
    //
    CArray transformed_complex_data{time_domain_samples.data(), time_domain_samples.size()};
    fft(transformed_complex_data);

    //
    // Convert back from complex to just the magnitudes of the complex numbers
    //
    const size_t frequency_bins = transformed_complex_data.size() / 2;
    std::vector<T> magnitudes;
    magnitudes.reserve(frequency_bins);
    for (size_t i = 0; i < frequency_bins; ++i)
    {
        magnitudes.push_back(std::norm(transformed_complex_data[i]));
    }
}

//
// Given an index from a vector that was compute with compute_fft, generate
//
static inline double get_frequency_from_index(const std::vector<T>& fft_data, const size_t i)
{
    const double frequency_factor = double(SAMPLE_RATE) / fft_data.size();
    return i * frequency_factor;
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
