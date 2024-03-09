#include "WaveTracer.hpp"
#include <glog/logging.h>
#include <math.h>


int sgn(double a) {
    return (0 < a) - (a < 0);
}


WaveTracer::WaveTracer() {
    set_data_size(8192);
}

// a function to find the argument of a vector given its components
inline double arg(double x, double y) {
    double out = atan(y / x);

    // expand arctan to the interval [-π/2, 3π/2].
    out = (sgn(x) >= 0) ? out : out + M_PI;

    // restrict out to interval [0, 2π].
    // out = (out > 0) ? out : out + 2 * M_PI;
    return (out > 0) ? out : out + 2 * M_PI;
}

// a function to find the magnitude of a vector given its components.
inline double mag(double x, double y) {
    return sqrt(pow(x,2) + pow(y,2));
}

void WaveTracer::set_fft_processors(FFT::Processor **fft_processor) {
    this->fft_processor = fft_processor;
}

void WaveTracer::set_data_size(int data_size) {

    if (data != nullptr) {
        for (int i = 0; i < histogram_size; i++) {
            if (data[i] != nullptr) {
                for (int j = 0; j < this->data_size; j++) {
                    if (data[i][j] != nullptr)
                        delete data[i][j];
                }
            }

            delete data[i];
        }
    }

    delete data;


    this->data_size = data_size;

    data = new double**[histogram_size];

    for (int i = 0; i < histogram_size; i ++) {

        data[i] = new double*[data_size];

        for (int j = 0; j < data_size; j++) {
            data[i][j] = new double[2];
        }
    }

}

void WaveTracer::process_data() {

    double x;
    double y;

    double data_arg;
    double data_mag;

    double m[3];  // magnitudes of frequencies will be stored here for mics 1 thru 3.

    for (int i = 0; i < data_size - 6000; i++) {

        // ignore the last 6000 data points (out of 8192) since the high frequencies are full of noise,
        // making their direction estimate more error prone;
        // this makes the output much more reliable.

        // we assume mic 1 points forwards, and mic 2 and 3 are located counterclockwise at 120 degree intervals.
        // we add the fft data from each microphone as vectors pointing in the direction of each microphone,
        // and then express the vectors in polar form and save them in the data array:

        for (int j = 0; j < 3; j++) {
            // m[j] = mag(fft_processor[mic_order[j] - 1]->out[i][0], fft_processor[mic_order[j] - 1]->out[i][1])
            m[j] = sqrt(pow(fft_processor[mic_order[j] - 1]->out[i][0],2) + pow(fft_processor[mic_order[j] - 1]->out[i][1],2));
        }

        // we assume the microphones are 120 degrees (2/3 π radians) apart, with the first mic along the y axis.

        // calculate resultant vector x and y values;
        // x has the cos component of both mics 2 and 3.
        // y has all of mic 1 and the sin component of mics 2 and 3.
        // use ref. angle π/6 = (π/2)/3.
        x = m[1] * cos(M_PI_2 / 3.0) - m[2] * cos(M_PI_2 / 3.0);
        y = m[0] - m[1] * sin(M_PI_2 / 3.0) - m[2] * sin(M_PI_2 / 3.0);


        data_mag = mag(x,y);

        if (data_mag > 0.001 && data_mag < 30) {

            data_arg = arg(x,y);

        } else {
            data_arg = 0;
        }

        write_data(data_arg, data_mag, i);

    }
}

// write new data to the histogram.
void WaveTracer::write_data(double arg, double mag, int f) {
    data[index][f][0] = arg;
    data[index][f][1] = mag;

    index = (index + 1) % histogram_size;
}

void WaveTracer::get_avg_data(int f, double *arg_out, double *mag_out) {

    double x = 0;
    double y = 0;

    // add up all the x and y components of all the data.
    for (int i = 0; i < histogram_size; i++) {
        x += data[i][f][1] * cos(data[i][f][0]) * (((histogram_size - index + i) % histogram_size) * 2.0 / histogram_size);
        y += data[i][f][1] * sin(data[i][f][0]) * (((histogram_size - index + i) % histogram_size) * 2.0 / histogram_size);
    }

    // write output to pointers.
    *arg_out = arg(x, y);
    *mag_out = *arg_out == 0 ? 0 : mag(x, y) / histogram_size;
}