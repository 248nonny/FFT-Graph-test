#pragma once
#include "src/WaveTracer.hpp"
#ifdef USE_GTK // only compile if this is defined in CMakeLists.txt.

#include <gtkmm.h>
#include <vector>

#include "src/graph/Graph.hpp"
#include "src/audio/AudioBuffer.hpp"
#include "sigc++/connection.h"
#include "src/fft/FFTProcessor.hpp"
#include "src/gtk/Compass.hpp"

using Audio::AudioBuffer;

class MainWindow : public Gtk::Window
{
public:
    // **audio_buffer contains pointers that we'll read data from.
    MainWindow(AudioBuffer **audio_buffer, FFT::Processor **fft_processor, WaveTracer *wave_tracer, int num_graphs = 2, AxisType axis_type = AxisType::LINEAR);

    Graph **graphs;
    AudioBuffer **audio_buffer;
    FFT::Processor **fft_processor;
    WaveTracer *wave_tracer;

    Gtk::Grid main_grid;

    int get_num_graphs() {return num_graphs; }
private:


    int num_graphs;

    int microphone_order[3] = {1,2,3};

    const int timeout_value = 100; // interval to run tasks fn in ms.

    // resolution for graphing audio data, 1 is highest, infty is lowest
    // (what this does is it only reads and plots the nth data point.)
    const int audio_resolution = 10;

    const int graph_data_count = 8192/2 + 1; // this multiplied by resolution divided by sample rate gives you the time in seconds that the graph will graph.

    bool tasks(); // this is run every couple milliseconds for background processing.

    bool update_graphs();
    void update_compass();

    void on_button_submit_settings();

    Gtk::Entry settings_entry;
    Gtk::Button submit_settings_button, cal1_button, cal2_button, cal3_button;
    Gtk::Box left_box, graph_box;

    Compass compass;

    sigc::connection tasks_connection;

};


#endif