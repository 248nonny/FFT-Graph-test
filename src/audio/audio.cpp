#include "audio.hpp"
#include "src/audio/AudioBuffer.hpp"

#include <math.h>
#include <portaudio.h>

// Note: Much of this code was copied from the YouTube tutorial
// https://www.youtube.com/watch?v=jpsJCji71Ec

// Code for the tutorial is found at:
// https://github.com/chrisrouck/tutorial-cpp-audio-capture/releases/tag/v1.0.0


namespace Audio {

// a function that prints port audio error text if fed an error code.
// does nothing if error code is "paNoError."
// partially copied from PortAudio examples.
static void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio Error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

// A function that initializes the PortAudio library.
int pa_init() {
    PaError err;
    err = Pa_Initialize();
    checkErr(err);
    return (err >= 0);
}

// a function to terminate PortAudio.
int pa_term() {
    PaError err;
    err = Pa_Terminate();
    checkErr(err);
    return err >= 0;
}

// A function to get information from available devices and print the info for the user.

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

}