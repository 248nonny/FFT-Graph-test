#include <glog/logging.h>
#include <portaudio.h>

#include "src/audio/audio.hpp"



int main (int argc, char *argv[]) {

    // init logging.
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc,&argv,true);
    fLB::FLAGS_logtostderr = 1;


    printf("%d\n",Audio::pa_init());
    Audio::get_pa_device_info();

    FFTProcessor **fft_processor;

    Audio::create_pa_stream(fft_processor, 5, 512);


    return 0;
}