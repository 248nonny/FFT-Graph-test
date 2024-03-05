
#pragma once
#include <functional>
#include <map>



namespace FFT {

    enum class WindowType {
        HANN,
        FLAT_TOP,
        HAMMING,
        BLACKMAN,
        BLACKMAN_HARRIS,
        BARTLETT,
        WELCH,
        NONE
    };

    const short window_count = 8;

    extern const char* names[window_count];

    // fills an array of specific length with a discreet window function; this array can then be multiplied by
    // an input array element wise to apply the window. This window array can be resused as long as its values are preserved.
    void make_window_array(FFT::WindowType window_type, double * array, int array_size);
}

// turn a WindowType into an index that can be used with "names" and the "function" lambda variable (in the cpp file of the same name as this one).
inline int e2i (FFT::WindowType window_type) {
    return static_cast<int>(window_type);
}
