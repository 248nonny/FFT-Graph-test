#pragma once

#include "src/audio/AudioBuffer.hpp"
#include <fftw3.h>


namespace FFT {

class Processor {
public:

    Processor();

    void new_data();

    // setter function for audio buffer pointer.
    void assign_audio_buffer(Audio::AudioBuffer *new_a_buffer) {audio_buffer = new_a_buffer; }

    // getter function for protected array size variable.
    int get_arr_size() {return arr_size;}

    // FFT output is read directly from this array.
    fftw_complex *out;

private:

    Audio::AudioBuffer *audio_buffer;

    // For the WaveTrace POC, a fixed value of 8192 was chosen.
    // This array size provides reasonable frequency resolution,
    // while still computing very quickly.
    // Future versions will have the option to change this value.
    const int arr_size = 8192;

    fftw_plan fft_plan;
    double *in;
};

}