
#include "src/fft/FFTProcessor.hpp"
#ifdef USE_GTK // only compile if this is defined in CMakeLists.txt.

#include <gtkmm.h>
#include <vector>

#include "src/graph/Graph.hpp"
#include "src/audio/AudioBuffer.hpp"
#include "sigc++/connection.h"

using Audio::AudioBuffer;

class MainWindow : public Gtk::Window
{
public:
    // **audio_buffer contains pointers that we'll read data from.
    MainWindow(AudioBuffer **audio_buffer, FFT::Processor **fft_processor, int num_graphs = 2, AxisType axis_type = AxisType::LINEAR);

    Graph **graphs;
    AudioBuffer **audio_buffer;
    FFT::Processor **fft_processor;

    Gtk::Grid main_grid;

    int get_num_graphs() {return num_graphs; }
private:


    int num_graphs;

    const int timeout_value = 40; // interval to run tasks fn in ms.

    // resolution for graphing audio data, 1 is highest, infty is lowest
    // (what this does is it only reads and plots the nth data point.)
    const int audio_resolution = 10;

    const int graph_data_count = 8192/2 + 1; // this multiplied by resolution divided by sample rate gives you the time in seconds that the graph will graph.

    bool tasks(); // this is run every couple milliseconds for background processing.

    bool update_graphs();

    sigc::connection tasks_connection;

};


#endif