#include "portaudio.h"

#include <iostream>
#include <exception>
#include <sstream>

#define SAMPLE_RATE 44000

static constexpr size_t FRAMES_PER_BUFFER = 512;

//
// Exception defined for when something goes wrong with the portaudio engine
//
class PortAudioException : public std::exception
{
public: // constructor ////////////////////////////////////////////////////////
    explicit PortAudioException(const PaError err)
        : err_(err)
    {
    }

public: // public methods /////////////////////////////////////////////////////
    const char* what() const throw() override
    {
        std::stringstream err;
        err << "An error occured while using the portaudio stream ( " << err_ << "): " << Pa_GetErrorText(err_) << "\n";
        return err.str().c_str();
    }

private: // private members ///////////////////////////////////////////////////
    const PaError err_;
};


//
// Crates the port audio stuff and provides an easy way to do callbacks
//
class AudioManager
{
public: // constructor ////////////////////////////////////////////////////////
    //
    //
    //
    AudioManager()
    {
        PaError err = Pa_Initialize();
        if( err != paNoError )
        {
            throw PortAudioException(err);
        }

        device_ = Pa_GetDefaultInputDevice(); /* default input device */
        if (device_ == paNoDevice) {
            std::cout << "Error: No default device could be found!\n";
            throw PortAudioException(err);
        }
    }

    //
    //
    //
    ~AudioManager()
    {
        PaError err = paNoError;

        if (stream != nullptr)
            err = Pa_CloseStream(stream);

        err = Pa_Terminate();
        if (err != paNoError)
        {
            std::cout << "Unable to shutdown audio manager!\n";
            throw PortAudioException(err);
        }
    }

public: // methods ////////////////////////////////////////////////////////////
    void make_good_record_callback(PaStreamCallback* callback, void* data)
    {
        PaStreamParameters inputParameters;
        inputParameters.device = device_;
        inputParameters.channelCount = 2;
        inputParameters.sampleFormat = paFloat32;
        inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
        inputParameters.hostApiSpecificStreamInfo = NULL;

        //
        // Let's actually open the stream now
        //
        PaError err = Pa_OpenStream(
                  &stream,
                  &inputParameters,
                  nullptr,
                  SAMPLE_RATE,
                  FRAMES_PER_BUFFER,
                  paClipOff,
                  callback,
                  data);

        if( err != paNoError )
        {
            throw PortAudioException(err);
        }

        err = Pa_StartStream(stream);

        if( err != paNoError )
        {
            throw PortAudioException(err);
        }
    }

    //
    //
    //
    PaStream* get_stream()
    {
        return stream;
    }

private: // members ///////////////////////////////////////////////////////////
    //
    // Device we use
    //
    PaDeviceIndex device_;

    //
    // We only support a single stream
    //
    PaStream* stream;
};

