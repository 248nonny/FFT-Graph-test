#include "AudioBuffer.hpp"

namespace Audio {

AudioBuffer::AudioBuffer() {   
}


void AudioBuffer::set_sample_rate(int rate) {
    sample_rate = rate;
    resize_buffer();
}

void AudioBuffer::set_buffer_period(double seconds) {
    buffer_period = seconds;
    resize_buffer();
}

void AudioBuffer::print_contents() {
    for (int i = 0; i < buffer_size; i++) {
        printf("i: %d, data: %f\n", i, data[i]);
    }
}

void AudioBuffer::pop_array(float *output, int array_size) {
    int ref_index = index;

    for (int i = 0; i < array_size; i++) {
        output[i] = data[((ref_index - array_size + i) % buffer_size + buffer_size) % buffer_size];
    }
}


void AudioBuffer::resize_buffer(int size) {
    buffer_size = (size < 0) * (buffer_period * sample_rate) + (size >= 0) * size;
    
    delete data;
    data = new float[buffer_size];
}

}