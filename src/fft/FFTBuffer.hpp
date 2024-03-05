#pragma once


#include <cstdio>

namespace FFT {


/*
A note:
This object is currently not in use for the WaveTrace proof of concept,
since for the POC it was only necessary to consider the most recently computed
FFT for each microphone. This object was tested and does work, and it will be useful
for storing a spectogram of each mic's output for analyzing how the FFTs change with time,
which could provide more accurate direction predictions.
*/

class Buffer {
public:
    Buffer();

    void set_size(int new_frame_count = -1, int new_frame_size = -1);


    inline double get_freq(int index) {return freqs[index]; }
    inline double get_value(int frame, int index, int ref_frame = 0) {
        return frames[(frame_count + ((ref_frame + frame) % frame_count)) % frame_count][index];
    }

    int get_frame_size() {return frame_size; }
    int get_frame_count() {return frame_count; }
    int get_frame_index() {return frame_index; }

    void write_frame(double *frame);
    void write_freqs(double *new_freqs);

    void print_contents();

private:

    int frame_count = 0;
    int frame_size = 0;

    int frame_index = 0;

    double **frames = nullptr;
    double *freqs = nullptr;

    inline void increase_index() {frame_index = (frame_index + 1) % frame_count; }

};

}