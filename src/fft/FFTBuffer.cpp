#include "FFTBuffer.hpp"



FFTBuffer::FFTBuffer() {
}


void FFTBuffer::print_contents() {
    for (int i = 0; i < frame_count; i++) {
        for (int j = 0; j < frame_size; j++) {
            printf("%d:%d -> %lf\n", i,j,frames[i][j]);
        }
    }
}

void FFTBuffer::write_frame(double *frame) {
    for (int i = 0; i < frame_size; i++) {
        frames[frame_index][i] = frame[i];

        printf("wrote %lf at %d:%d\n",frames[frame_index][i], frame_index, i);
    }

    printf("frame written at index: %d\n", frame_index);
    increase_index();
};

void FFTBuffer::write_freqs(double *new_freqs) {
    for (int i = 0; i < frame_size; i++) {
        freqs[i] = new_freqs[i];
    }
};

void FFTBuffer::set_size(int new_frame_count, int new_frame_size) {


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
