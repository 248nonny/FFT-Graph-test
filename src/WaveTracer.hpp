#pragma once

#include "src/fft/FFTProcessor.hpp"

class WaveTracer {
public:
    WaveTracer();

    void set_fft_processors(FFT::Processor **fft_processor);

    int mic_order[3] = {1,2,3};

    void process_data();

    void set_data_size(int data_size);

    double **data; // each data point should have radians for x and intensity for y. There will be one data point for each frequency.
    int data_size = 0;

private:

    FFT::Processor **fft_processor = nullptr;


};