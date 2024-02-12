#include <portaudio.h>
#include "src/fft/FFTProcessor.hpp"
#include "src/audio/AudioBuffer.hpp"
#include <glog/logging.h>


namespace Audio {

typedef struct {
    FFTProcessor **fft_processor;
    AudioBuffer **audio_buffer;
    int channel_count;
    int channel_array_size;
} StreamData;


int pa_init();
void get_pa_device_info();


class AudioHandler {
public:
    AudioHandler();

    void set_device_id(int device_id) {
        this->device_id = device_id;
    }

    void set_channel_array_size(int channel_array_size) {
        this->channel_array_size = channel_array_size;
    }

    void set_channel_count(int channel_count) {
        this->channel_count = channel_count;
    }

    // this is useful since "**fft_processor" must have this length.
    int get_channel_count() { return Pa_GetDeviceInfo(device_id)->maxInputChannels; }
    StreamData* get_stream_data() {return &(this->stream_data); }

    int create_pa_stream(); // use device_id, channel_array_size, and fft_processor for this.
    int close_stream();


    FFTProcessor **fft_processor;
    AudioBuffer **audio_buffer;

private:

    int device_id = 5;
    int channel_array_size = 512;
    int channel_count = 2;


    StreamData stream_data;
    PaStream *stream;
};

}