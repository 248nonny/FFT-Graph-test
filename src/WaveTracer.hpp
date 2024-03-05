#pragma once

#include "src/fft/FFTProcessor.hpp"


// This object is in charge of taking FFT data from the mics and computing the directional data associated with each
// frequency component.

class WaveTracer {
public:
    WaveTracer();

    // setter and getter functions for private vars
    void set_fft_processors(FFT::Processor **fft_processor);
    void set_data_size(int data_size);
    int get_data_size() {return data_size; }

    // the default mic order
    int mic_order[3] = {1,2,3};

    // a function that processes new data.
    void process_data();

    // a function that finds the average of the data for each frequency throughout the whole histogram.
    void get_avg_data(int f, double *arg_out, double *mag_out);

    // Each data point should have radians for x and intensity for y. There will be one data point for each frequency.
    // The below "data" is a histogram of data that we can take the average of with "get_avg_data."
    double ***data = nullptr;
    int data_size = 0;
    const int histogram_size = 5;

private:

    int index = 0;

    FFT::Processor **fft_processor = nullptr;
    void write_data(double arg, double mag, int f);

};