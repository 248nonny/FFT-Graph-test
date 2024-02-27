#include "WaveTracer.hpp"
#include <glog/logging.h>
#include <math.h>


int sgn(double a) {
    return (0 < a) - (a < 0);
}


WaveTracer::WaveTracer() {
    set_data_size(8192);
}

inline double arg(double x, double y) {
    double out = atan(y / x);

    // expand arctan to the interval [-π/2, 3π/2].
    out = (sgn(x) >= 0) ? out : out + M_PI;

    // restrict out to interval [0, 2π].
    // out = (out > 0) ? out : out + 2 * M_PI;
    return (out > 0) ? out : out + 2 * M_PI;
}

inline double mag(double x, double y) {
    return sqrt(pow(x,2) + pow(y,2));
}

void WaveTracer::set_fft_processors(FFT::Processor **fft_processor) {
    this->fft_processor = fft_processor;
}

void WaveTracer::set_data_size(int data_size) {

    printf("asdasdasd\n");
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

    printf("1123412fdmnfsd\n");

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


        data_mag = mag(x,y);

        if (data_mag > 0.001 && data_mag < 30) {

            data_arg = arg(x,y);

        } else {
            data_arg = 0;
        }

        write_data(data_arg, data_mag, i);

        // printf("data: %9.5lf, mag: %9.5lf\n", data[i][1], mag);

    }
}

void WaveTracer::write_data(double arg, double mag, int f) {
    data[index][f][0] = arg;
    data[index][f][1] = mag;

    index = (index + 1) % histogram_size;
}

void WaveTracer::get_avg_data(int f, double *arg_out, double *mag_out) {

    double x = 0;
    double y = 0;

    for (int i = 0; i < histogram_size; i++) {
        x += data[i][f][1] * cos(data[i][f][0]) * (((histogram_size - index + i) % histogram_size) * 2.0 / histogram_size);
        y += data[i][f][1] * sin(data[i][f][0]) * (((histogram_size - index + i) % histogram_size) * 2.0 / histogram_size);
    }

    *arg_out = arg(x, y);
    *mag_out = *arg_out == 0 ? 0 : mag(x, y);
}