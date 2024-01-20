#include "window-functions.hpp"
#include <cmath>
#include <glog/logging.h>


namespace FFTWindow {



    // should use the e2i function to convert enumeration to int.
    // extern const short int HANN = 0;
    // extern const short int FLAT_TOP = 1;
    // extern const short int HAMMING = 2;
    // extern const short int BLACKMAN = 3;
    // extern const short int BLACKMAN_HARRIS = 4;
    // extern const short int BARTLETT = 5;
    // extern const short int WELCH = 6;

    const char* names[window_count] = {
        "Hann",
        "Flat Top",
        "Hamming",
        "Blackman",
        "Blackman-Harris",
        "Bartlett",
        "Welch"
    };

    std::function<double (int, int)> function[window_count] = {
        ([] (int array_size, int index){return pow(sin((M_PI*index)/array_size),2);}),
        ([] (int array_size, int index){return 0.21557895 - 0.41663158 * cos((2 * M_PI * index)/array_size) + 0.277263158 * cos((4 * M_PI * index)/array_size) - 0.083578947 * cos((6 * M_PI * index)/array_size) + 0.00735 * cos((8 * M_PI * index)/array_size);}),
        ([] (int array_size, int index) {return 0.53836 - 0.46164 * cos((2*M_PI*index)/array_size);}),
        ([] (int array_size, int index) {return (1 - 0.16)/2 - 0.5 * cos((2*M_PI*index)/array_size) + (0.16/2) * cos((4*M_PI*index)/array_size);}),
        ([] (int array_size, int index) {return 0.35875 - 0.48829 * cos((2 * M_PI * index)/array_size) + 0.14128 * cos((4 * M_PI * index)/array_size) - 0.01168 * cos((6 * M_PI * index)/array_size);}),
        ([] (int array_size, int index) {return (index > array_size/2) ? (2 - (double)(2 * index)/array_size) : ((double)(2 * index)/array_size);}),
        ([] (int array_size, int index) {return 1 - pow((double)(index - (float)array_size/2)/((float)array_size/2),2);})
    };

    void make_window_array(FFTWindow::WindowType window_type, double * array, int array_size) {
        DLOG(INFO) << "making window array of type " << FFTWindow::names[e2i(window_type)] << ", and size " << array_size << ".";
        for (int i = 0; i < array_size; i++) {
            array[i] = FFTWindow::function[e2i(window_type)](array_size,i);
            DLOG(INFO) << array[i] << "\n";
        }
    }
}

