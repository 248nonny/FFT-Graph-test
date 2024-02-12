#include <portaudio.h>
#include "src/fft/FFTProcessor.hpp"
#include "src/audio/AudioBuffer.hpp"
#include <glog/logging.h>


namespace Audio {


int pa_init();
void get_pa_device_info();


}