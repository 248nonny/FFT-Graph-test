#include "FFTProcessor.hpp"
#include <fftw3.h>

namespace FFT {

Processor::Processor() {

    // Allocate and initialize in and out arrays during construction.
    in = new double[arr_size];
    out = new double[arr_size/2 + 1][2];

    // setup fftw plan.
    fft_plan = fftw_plan_dft_r2c_1d(arr_size, in, out, FFTW_ESTIMATE);
}

// this function is run whenever there's new audio data to process.
void Processor::new_data() {

    // read data from audio buffer and write in the "in" array.
    int aud_index = audio_buffer->get_current_index();
    for (int i = 0; i < arr_size; i++) {
        in[i] = audio_buffer->read_value(i - arr_size, aud_index);
    }

    // compute FFT; output is automatically written in "out."
    fftw_execute(fft_plan);
}

}