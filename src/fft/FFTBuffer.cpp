#include "FFTBuffer.hpp"


namespace FFT {

Buffer::Buffer() {
}

// prints all stored data; useful for testing purposes.
void Buffer::print_contents() {
    for (int i = 0; i < frame_count; i++) {
        for (int j = 0; j < frame_size; j++) {
            printf("%d:%d -> %lf\n", i,j,frames[i][j]);
        }
    }
}

// this stores an array of data, which I called a frame;
// each frame is a spectrum which is the result of an FFT computation.
void Buffer::write_frame(double *frame) {
    for (int i = 0; i < frame_size; i++) {
        frames[frame_index][i] = frame[i];

        printf("wrote %lf at %d:%d\n",frames[frame_index][i], frame_index, i);
    }

    printf("frame written at index: %d\n", frame_index);
    increase_index();
};

void Buffer::write_freqs(double *new_freqs) {
    for (int i = 0; i < frame_size; i++) {
        freqs[i] = new_freqs[i];
    }
};

void Buffer::set_size(int new_frame_count, int new_frame_size) {


    if (new_frame_size > 0) {
        frame_size = new_frame_size;

        delete freqs;
        freqs = new double[frame_size];
    }

    if (new_frame_count > 0) {
        frame_count = new_frame_count;
        
        delete frames;
        frames = new double*[frame_count];

        for (int i = 0; i < frame_count; i++) {
            frames[i] = new double[frame_size];
        }
    }



}

}