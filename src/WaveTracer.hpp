#pragma once

#include "src/fft/FFTProcessor.hpp"

class WaveTracer {
public:
    WaveTracer();

    void set_fft_processors(FFT::Processor **fft_processor);

    int mic_order[3] = {1,2,3};

    void process_data();

    void set_data_size(int data_size);
    int get_data_size() {return data_size; }

    void get_avg_data(int f, double *arg_out, double *mag_out);

    // Each data point should have radians for x and intensity for y. There will be one data point for each frequency.
    // The below data will soon be a histogram of data that we can take the average of.
    
    double ***data = nullptr;
    int data_size = 0;
    const int histogram_size = 10;

private:

    int index = 0;

    FFT::Processor **fft_processor = nullptr;
    void write_data(double arg, double mag, int f);

};