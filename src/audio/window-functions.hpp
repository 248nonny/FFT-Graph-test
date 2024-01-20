
#pragma once
#include <functional>
#include <map>

// below the names and indexes of various windows are defined.
#pragma region


namespace FFTWindow {

    enum class WindowType {
        HANN,
        FLAT_TOP,
        HAMMING,
        BLACKMAN,
        BLACKMAN_HARRIS,
        BARTLETT,
        WELCH
    };

    const short window_count = 7;

    extern const char* names[window_count];

    void make_window_array(FFTWindow::WindowType window_type, double * array, int array_size);
}


inline int e2i (FFTWindow::WindowType window_type) {
    return static_cast<int>(window_type);
}

#pragma endregion
