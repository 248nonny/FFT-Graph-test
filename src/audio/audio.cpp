#include "audio.hpp"
#include <portaudio.h>


namespace Audio {

static void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio Error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

int pa_init() {
    PaError err;
    err = Pa_Initialize();
    checkErr(err);
    return (err >= 0);
}

void get_pa_device_info() {
    DLOG(INFO) << "getting PA device info.";
    int device_count = Pa_GetDeviceCount();
    DLOG(INFO) << device_count << " devices detected by portaudio.";

    if (device_count < 0) {
        printf("Error getting device count.\n");
        exit(EXIT_FAILURE);
    } else if (device_count == 0) {
        printf("There are no available audio devices on this machine.");
        Pa_Terminate();
        exit(EXIT_SUCCESS);
    }

    std::vector<const PaDeviceInfo*> device_info;
    device_info.resize(device_count);

    for (int i = 0; i < device_count; i++) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo != nullptr) {
            device_info[i] = deviceInfo;
        }
    }

    DLOG(INFO) << "printing device info.";
    for (int i = 0; i < device_info.size(); i++) {
        printf("Device: %d\n",i);
        printf("   Name: %s\n",device_info[i]->name);
        printf("   max input channels: %d\n",device_info[i]->maxInputChannels);
        printf("   max output channels: %d\n",device_info[i]->maxOutputChannels);
        printf("   default sample rate: %f\n", device_info[i]->defaultSampleRate);
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

    DLOG(INFO) << "audio callback called.";

    double* in = (double*) input;
    (void)output;

    StreamData *data = (StreamData*) userData;

    std::vector<std::vector<double>> channel_data(data->channel_count);

    DLOG(INFO) << "separating data into channels";
    for (int i = 0; i < data->channel_count; i++) {
        channel_data[i].resize(framesPerBuffer);
    }

    int channel_count = data->channel_count;

    for (int frame = 0; frame < framesPerBuffer; frame++) {
        for (int channel = 0; channel < channel_count; channel++) {
            channel_data[channel][frame] = in[frame * channel_count + channel];
        }
    }

    DLOG(INFO) << "sending data to FFTProcessor.";

    for (int i = 0; i < channel_count; i++) {
        DLOG(INFO) << "sending channel " << i << ".";
        // update to FFTProcessor.
        // (*data).fft_commander[i]->receive_audio_data(channel_data[i]);
        DLOG(INFO) << "done.";
    }

    DLOG(INFO) << "Audio Callback done running.";

    return 0;
}

int create_pa_stream(
                    FFTProcessor *fft_processor,
                    const int device_id,
                    const int channel_array_size
) {

    PaError err;
    PaStream *stream;
    StreamData stream_data;

    int channel_count = Pa_GetDeviceInfo(device_id)->maxInputChannels;

    stream_data.fft_processor = fft_processor;
    stream_data.channel_count = channel_count;

    DLOG(INFO) << "Setting Audio Stream Parameters...";

    PaStreamParameters stream_input_parameters;
    PaStreamParameters stream_output_parameters;

    // we set the stream parameters.
    memset(&stream_output_parameters, 0, sizeof(stream_output_parameters));
    stream_output_parameters.channelCount = Pa_GetDeviceInfo(device_id)->maxOutputChannels;
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




}