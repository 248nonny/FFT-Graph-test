#pragma once

#include "src/audio/AudioBuffer.hpp"
#include <fftw3.h>



class FFTProcessor {
public:
    // max period is measured in data points; true max period will be the closest (just under) power of two.
    // frame size is the number of new elements that there are every time the ffts are executed;
    // this should be the same as framerPerBuffer for the corresponding audio stream.
    FFTProcessor();

    void new_data();

    void assign_audio_buffer(Audio::AudioBuffer *new_a_buffer) {audio_buffer = new_a_buffer; }
    int get_arr_size() {return arr_size;}

    fftw_complex *out;

private:

    Audio::AudioBuffer *audio_buffer;

    const int arr_size = 8192;

    fftw_plan fft_plan;
    double *in;
    double *freqs;
};