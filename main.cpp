#include <cstdlib>
#include <glog/logging.h>
#include <portaudio.h>
#include <string.h>


#include "gtkmm/application.h"
#include "src/audio/audio.hpp"
#include "src/audio/AudioHandler.hpp"
#include "src/graph/Graph.hpp"\


#ifdef USE_GTK
    #include "src/gtk/MainWindow.hpp"
#endif



int main (int argc, char *argv[]) {

    // init logging.
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc,&argv,true);
    fLB::FLAGS_logtostderr = 1;


    printf("%d\n",Audio::pa_init());
    Audio::get_pa_device_info();


    int dev_id = 14;
    int num_ch = 3;

    // if there were command line parameters, the first should be the device name:
    if (argc > 1) {
        for (int i = 0; i < Pa_GetDeviceCount(); i++) {
            if (strcmp(argv[1], Pa_GetDeviceInfo(i)->name) == 0) {
                dev_id = i;
            }
        }
    }

    if (argc > 2) {
        num_ch = atoi(argv[2]);
    }

    DLOG(INFO) << "selected device name: " << Pa_GetDeviceInfo(dev_id)->name;

    Audio::Handler audio_handler;

    audio_handler.set_device_id(dev_id);
    audio_handler.set_channel_count(3);
    audio_handler.set_channel_array_size(512);

    audio_handler.create_pa_stream();

    #ifdef USE_GTK

    auto app = Gtk::Application::create("org.gtkmm.example");



    app->make_window_and_run<MainWindow>(argc, argv,audio_handler.audio_buffer, audio_handler.fft_processor, 1, AxisType::LOG);

    #else
        Pa_Sleep(1000 * 1000);
    #endif

    audio_handler.close_stream();

    return 0;
}