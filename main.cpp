#include "src/CPO.hpp"
#include <glog/logging.h>

/*
FOR TESTING ONLY
DELETE FOR PRODUCTION OR WHATEVA
(also try to find the right devices the "smart" way).
(i.e. not hardcoded IDs).
*/
#include <portaudio.h>


static void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio Error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
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


int main (int argc, char *argv[]) {

    PaError err;
    err = Pa_Initialize();
    checkErr(err);

    get_pa_device_info();

    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc,&argv,true);
    fLB::FLAGS_logtostderr = 1;

    CPO cpo;

    // while(1);

    return 0;
}