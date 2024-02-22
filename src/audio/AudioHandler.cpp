#include "AudioHandler.hpp"

namespace Audio {


double max(double a, double b) {
    return a > b ? a : b;
}


static void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio Error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

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


    for (int frame = 0; frame < framesPerBuffer; frame++) {
        for (int channel = 0; channel < channel_count; channel++) {
            // printf("i: %d: %lf\n", frame * channel_count + channel, in[frame * channel_count + channel]);
            data->audio_buffer[channel]->write_value(in[frame * channel_count + channel]);
        }
    }



    // DLOG(INFO) << "sending data to FFTCommander.";

    for (int i = 0; i < channel_count; i++) {
        data->fft_processor[i]->new_data();
    }

    // DLOG(INFO) << "Audio Callback done running.";

    return 0;
}

int Handler::create_pa_stream() {

    // create fft_processors and audio buffers:
    fft_processor = new FFT::Processor*[channel_count];
    audio_buffer = new AudioBuffer*[channel_count];
    
    for (int i = 0; i < channel_count; i++) {
        audio_buffer[i] = new AudioBuffer;
        fft_processor[i] = new FFT::Processor;
        fft_processor[i]->assign_audio_buffer(audio_buffer[i]);
    }


    PaError err;

    int channel_count = Pa_GetDeviceInfo(device_id)->maxInputChannels;

    stream_data.fft_processor = fft_processor;
    stream_data.audio_buffer = audio_buffer;
    stream_data.channel_count = channel_count;
    stream_data.channel_array_size = channel_array_size;

    for (int i = 0; i < channel_count; i++) {
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

int Handler::close_stream() {
    
    PaError err;

    err = Pa_CloseStream(stream);
    checkErr(err);

    return (err >= 0);
}

Handler::Handler() {
}

}