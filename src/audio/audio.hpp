#include <portaudio.h>
#include "FFTProcessor.hpp"
#include <glog/logging.h>


namespace Audio {

typedef struct {
    FFTProcessor *fft_processor;
    int channel_count;
} StreamData;


int pa_init();
void get_pa_device_info();

int create_pa_stream(
                    FFTProcessor *fft_processor,
                    const int device_id,
                    const int channel_array_size
);

}