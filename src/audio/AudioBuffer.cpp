#include "AudioBuffer.hpp"

namespace Audio {

AudioBuffer::AudioBuffer() {   
}

// setter functions for protected variables;
// the buffer is also resized to match the new values.
void AudioBuffer::set_sample_rate(int rate) {
    sample_rate = rate;
    resize_buffer();
}

void AudioBuffer::set_buffer_period(double seconds) {
    buffer_period = seconds;
    resize_buffer();
}

// will print all data stored in the buffer; useful for testing.
void AudioBuffer::print_contents() {
    for (int i = 0; i < buffer_size; i++) {
        printf("i: %d, data: %f\n", i, data[i]);
    }
}

// writes the most recent values in an array.
void AudioBuffer::pop_array(float *output, int array_size) {
    int ref_index = index;

    for (int i = 0; i < array_size; i++) {
        output[i] = data[((ref_index - array_size + i) % buffer_size + buffer_size) % buffer_size];
    }
}

// reallocates data array to new size according to sample rate and period (in seconds).
void AudioBuffer::resize_buffer(int size) {
    buffer_size = (size < 0) * (buffer_period * sample_rate) + (size >= 0) * size;
    
    delete data;
    data = new float[buffer_size];
}

}