#include "WaveTracer.hpp"
#include <glog/logging.h>
#include <math.h>


int sgn(double a) {
    return (0 < a) - (a < 0);
}


WaveTracer::WaveTracer() {
    set_data_size(8192);
}



void WaveTracer::set_fft_processors(FFT::Processor **fft_processor) {
    this->fft_processor = fft_processor;
}

void WaveTracer::set_data_size(int data_size) {
    this->data_size = data_size;

    data = new double*[data_size];

    for (int i = 0; i < data_size; i++) {
        data[i] = new double[2];
    }
}

void WaveTracer::process_data() {

    double x;
    double y;

    double arg;
    double mag;

    double m[3];  // magnitudes of frequencies will be stored here for mics 1 thru 3.

    for (int i = 0; i < data_size - 7000; i++) {
        // we assume mic 1 points forwards, and mic 2 and 3 are located counterclockwise at 120 degree intervals.
        // we add the fft data from each microphone as vectors pointing in the direction of each microphone,
        // and then express the vectors in polar form and save them in the data array:

        for (int j = 0; j < 3; j++) {
            // printf("j: %d\n", j);
            m[j] = sqrt(pow(fft_processor[mic_order[j] - 1]->out[i][0],2) + pow(fft_processor[mic_order[j] - 1]->out[i][1],2));
        }

        // printf("asdasda\n");
        // calculate resultant vector x value;
        // this should have the cos component of both mics 2 and 3.
        // use ref. angle π/6.
        x = m[1] * cos(M_PI_2 / 3.0) - m[2] * cos(M_PI_2 / 3.0);
        y = m[0] - m[1] * sin(M_PI_2 / 3.0) - m[2] * sin(M_PI_2 / 3.0);


        mag = sqrt(pow(x,2) + pow(y,2));

        // if (mag > 0.001 && mag < 30) {

            arg = atan(y / x);

            // expand arctan to the interval [-π/2, 3π/2].
            arg = (sgn(x) >= 0) ? arg : arg + M_PI;

            // restrict arg to interval [0, 2π].
            arg = (arg > 0) ? arg : arg + 2 * M_PI;

            // if (mag > 10)
            // printf("arg: %9.5lf, mag: %9.5lf\n", arg, mag);

        // } else {
        //     arg = 0;
        // }
        // store data.
        data[i][0] = arg;
        data[i][1] = mag;

        // printf("data: %9.5lf, mag: %9.5lf\n", data[i][1], mag);

    }
}

