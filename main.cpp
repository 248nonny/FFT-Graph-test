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

    Audio::AudioHandler audio_handler;

    audio_handler.set_device_id(5);
    audio_handler.set_channel_count(2);
    audio_handler.set_channel_array_size(512);

    audio_handler.create_pa_stream();

    Pa_Sleep(1000 * 4);

    audio_handler.close_stream();

    return 0;
}