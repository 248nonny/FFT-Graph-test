#include "FFTProcessor.hpp"
#include <fftw3.h>

namespace FFT {

Processor::Processor() {
    in = new double[arr_size];
    freqs = new double[arr_size/2 + 1];
    out = new double[arr_size/2 + 1][2];

    fft_plan = fftw_plan_dft_r2c_1d(arr_size, in, out, FFTW_ESTIMATE);
}

void Processor::new_data() {
    int aud_index = audio_buffer->get_current_index();

    for (int i = 0; i < arr_size; i++) {
        in[i] = audio_buffer->read_value(i - arr_size, aud_index);
    }

    fftw_execute(fft_plan);
}

}