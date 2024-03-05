#include "window-functions.hpp"
#include <cmath>
#include <glog/logging.h>


namespace FFT {

    // Another note: the math for the functions was mainly taken from the respective wikipedia articles.
    // Also, I wrote this code for earlier tests with the FFTW3 library,
    // but it turns out that a windowless FFT works great for the purposes of
    // WaveTrace, so this code is unused in the proof of concept for the time being; it remains as an option
    // in the future.


    // for the below, use the e2i function described in the header file to
    // get the index for each function. This index can be fed into the below "names" to
    // get the name of a function as a string, or to "function" to get a lambda function
    // variable that contains the window function.

    const char* names[window_count] = {
        "Hann",
        "Flat Top",
        "Hamming",
        "Blackman",
        "Blackman-Harris",
        "Bartlett",
        "Welch",
        "None"
    };

    std::function<double (int, int)> function[window_count] = {
        ([] (int array_size, int index){return pow(sin((M_PI*index)/array_size),2);}),
        ([] (int array_size, int index){return 0.21557895 - 0.41663158 * cos((2 * M_PI * index)/array_size) + 0.277263158 * cos((4 * M_PI * index)/array_size) - 0.083578947 * cos((6 * M_PI * index)/array_size) + 0.00735 * cos((8 * M_PI * index)/array_size);}),
        ([] (int array_size, int index) {return 0.53836 - 0.46164 * cos((2*M_PI*index)/array_size);}),
        ([] (int array_size, int index) {return (1 - 0.16)/2 - 0.5 * cos((2*M_PI*index)/array_size) + (0.16/2) * cos((4*M_PI*index)/array_size);}),
        ([] (int array_size, int index) {return 0.35875 - 0.48829 * cos((2 * M_PI * index)/array_size) + 0.14128 * cos((4 * M_PI * index)/array_size) - 0.01168 * cos((6 * M_PI * index)/array_size);}),
        ([] (int array_size, int index) {return (index > array_size/2) ? (2 - (double)(2 * index)/array_size) : ((double)(2 * index)/array_size);}),
        ([] (int array_size, int index) {return 1 - pow((double)(index - (float)array_size/2)/((float)array_size/2),2);}),
        ([] (int array_size, int index) {return 1; })
    };

    void make_window_array(FFT::WindowType window_type, double * array, int array_size) {
        DLOG(INFO) << "making window array of type " << FFT::names[e2i(window_type)] << ", and size " << array_size << ".";
        for (int i = 0; i < array_size; i++) {
            array[i] = FFT::function[e2i(window_type)](array_size,i);
            // DLOG(INFO) << array[i] << "\n";
        }
    }
}

