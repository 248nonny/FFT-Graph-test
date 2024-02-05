
#include <glog/logging.h>

#include "src/audio/AudioBuffer.hpp"

// this main loop is used for testing the AudioBuffer object. Change 

int main(int argc, char *argv[]) {

    // init logging.
    google::InitGoogleLogging(argv[0]);
    gflags::ParseCommandLineFlags(&argc,&argv,true);
    fLB::FLAGS_logtostderr = 1;

    AudioBuffer buffer;

    buffer.set_buffer_period(2);
    buffer.set_sample_rate(300);

    printf("buf size: %d\n", buffer.get_buffer_size());

    for (int i = 0; i < buffer.get_buffer_size() * 2; i++) {
        buffer.write_value(i);
    }

    double output[500];

    buffer.pop_array(output, 500);

    for (int i = 0; i < 500; i++) {
        printf("%lf\n", output[i]);
    }

}