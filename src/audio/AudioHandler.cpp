#include "AudioHandler.hpp"


// Note that the structures of the audio_callback() and create_pa_stream()
// functions were mostly copied from the port audio website as well as from the
// YouTube tutorial: https://www.youtube.com/watch?v=jpsJCji71Ec

namespace Audio {


// a function that gets and prints Port Audio error text from an error code.
static void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio Error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

// the function that will be run everytime Port Audio has new data.
static int audio_callback(
        const void *input,
        void *output,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData
) {

    // DLOG(INFO) << "audio callback called.";

    float* in = (float*) input;
    (void)output;

    StreamData *data = (StreamData*) userData;

    int channel_count = data->channel_count;

    // Store incoming audio data in the audio buffers.
    // Data is ordered like this: {c1:0, c2:0, c3:0,..., c1:1, c2:1, c3:1,..., c1:2 ...}
    // so the first value of the first channel has index 0, the second value of the first channel has index 0 + the number of channels, etc.
    // Similarly, the second channel's first value's index is 1, index for the second value is 1 + num. channels, etc.
    for (int frame = 0; frame < framesPerBuffer; frame++) {
        for (int channel = 0; channel < channel_count; channel++) {
            data->audio_buffer[channel]->write_value(in[frame * channel_count + channel]);
        }
    }

    // DLOG(INFO) << "sending data to FFTCommander.";

    // The fft processors already have pointers to their respective buffers (see below "create_pa_stream" function)
    // so all we have to do is tell them that there is new data.
    for (int i = 0; i < channel_count; i++) {
        data->fft_processor[i]->new_data();
    }

    // DLOG(INFO) << "Audio Callback done running.";

    return 0;
}

int Handler::create_pa_stream() {

    // allocate and initialize fft_processors and audio buffers:

    // initialize pointers to pointers:
    fft_processor = new FFT::Processor*[channel_count];
    audio_buffer = new AudioBuffer*[channel_count];
    
    // allocate and initialize fft processors and audio buffers at respective pointers:
    for (int i = 0; i < channel_count; i++) {
        audio_buffer[i] = new AudioBuffer;
        fft_processor[i] = new FFT::Processor;

        // assign each fft processor an audio buffer.
        fft_processor[i]->assign_audio_buffer(audio_buffer[i]);
    }


    PaError err;

    int channel_count = Pa_GetDeviceInfo(device_id)->maxInputChannels;

    // store pointers in stream data for access from the callback function.
    stream_data.fft_processor = fft_processor;
    stream_data.audio_buffer = audio_buffer;
    stream_data.channel_count = channel_count;
    stream_data.channel_array_size = channel_array_size;

    // set audio buffer parameters based on audio device properties.
    for (int i = 0; i < channel_count; i++) {
        // a period of two seconds was chosen arbitrarily (long enough is long enough; we won't need to compute FFTs with longer periods).
        audio_buffer[i]->set_buffer_period(2);
        audio_buffer[i]->set_sample_rate(Pa_GetDeviceInfo(device_id)->defaultSampleRate);
    }

    DLOG(INFO) << "Setting Audio Stream Parameters...";

    PaStreamParameters stream_input_parameters;
    PaStreamParameters stream_output_parameters;

    // we set the stream parameters.
    memset(&stream_output_parameters, 0, sizeof(stream_output_parameters));
    stream_output_parameters.channelCount = 1;
    stream_output_parameters.device = device_id;
    stream_output_parameters.hostApiSpecificStreamInfo = NULL;
    stream_output_parameters.sampleFormat = paFloat32;
    stream_output_parameters.suggestedLatency = Pa_GetDeviceInfo(device_id)->defaultLowOutputLatency;

    memset(&stream_input_parameters, 0, sizeof(stream_input_parameters));
    stream_input_parameters.channelCount = Pa_GetDeviceInfo(device_id)->maxInputChannels;
    stream_input_parameters.device = device_id;
    stream_input_parameters.hostApiSpecificStreamInfo = NULL;
    stream_input_parameters.sampleFormat = paFloat32;
    stream_input_parameters.suggestedLatency = Pa_GetDeviceInfo(device_id)->defaultLowInputLatency;

    DLOG(INFO) << "Opening Stream...";
    // open the PA stream, feeding it the stream data, the callback function, and the stream parameters.
    err = Pa_OpenStream(
    &stream,
    &stream_input_parameters,
    &stream_output_parameters,
    Pa_GetDeviceInfo(device_id)->defaultSampleRate,
    channel_array_size,
    paNoFlag,
    audio_callback,
    &stream_data
    );

    checkErr(err);

    DLOG(INFO) << "Success. Starting Stream...";

    err = Pa_StartStream(stream);
    checkErr(err);
    DLOG(INFO) << "Success! Audio Stream is up and running.";

    return (err >= 0);

}

// A function that takes care of closing the stream.
int Handler::close_stream() {
    
    PaError err;

    err = Pa_CloseStream(stream);
    checkErr(err);

    return (err >= 0);
}

}