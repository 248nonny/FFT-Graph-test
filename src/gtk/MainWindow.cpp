

#include "glibmm/ustring.h"
#include "gtkmm/enums.h"
#include "src/WaveTracer.hpp"
#include "src/fft/FFTProcessor.hpp"
#include <cstring>
#include <functional>
#ifdef USE_GTK


#include <glog/logging.h>
#include <vector>
#include <portaudio.h>
#include <thread>


#include "MainWindow.hpp"
#include "src/graph/Graph.hpp"
#include "glibmm/main.h"
#include "sigc++/adaptors/bind.h"
#include "src/audio/AudioBuffer.hpp"


// initialize object variables based on constructor parameters.
MainWindow::MainWindow(AudioBuffer **audio_buffer, FFT::Processor **fft_processor,WaveTracer *wave_tracer, int num_graphs, AxisType axis_type) :
    num_graphs(num_graphs),
    audio_buffer(audio_buffer),
    fft_processor(fft_processor),
    submit_settings_button("Submit Settings"),
    left_box(Gtk::Orientation::VERTICAL),
    graph_box(Gtk::Orientation::VERTICAL),
    wave_tracer(wave_tracer)
{
    DLOG(INFO) << "initializing main window...";

    // setup graph widgets:
    set_child(main_grid);

    // make array of pointers to graphs
    graphs = new Graph*[num_graphs];

    // initialize audio compass
    compass.allocate_data(100);

    // create and initialize graphs
    for (int i = 0; i < num_graphs; i++) {
        DLOG(INFO) << "i: " << i;
        graphs[i] = Gtk::make_managed<Graph>();
        graphs[i]->data.resize(1);
        graphs[i]->data[0].resize(graph_data_count);

        graphs[i]->grid.x_type = axis_type;

        // graphs[i]->grid.xstart = 0.001;
        graphs[i]->grid.main_x_line_increment = 2000;
        graphs[i]->grid.xstart = 3;
        graphs[i]->grid.xstop = 22100;

        graphs[i]->grid.ystart = -1;
        graphs[i]->grid.ystop = 10;
        graphs[i]->grid.main_y_line_increment = 1;

        for (int j = 0; j < graph_data_count; j++) {
            graphs[i]->data[0][j].resize(2);
        }

        DLOG(INFO) << "Wrote data";
        graph_box.append(*graphs[i]);
        graphs[i]->set_size_request(400 * GUI_SCALE, 100 * GUI_SCALE);
    }

    // setup settings button
    submit_settings_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_submit_settings));

    // set up Gtk window structure and child properties.
    main_grid.attach(left_box, 0, 0);
    main_grid.attach(graph_box, 1, 0);

    left_box.set_hexpand(true);
    graph_box.set_hexpand(false);
    graph_box.set_size_request(800,-1);
    left_box.append(compass);
    settings_entry.set_max_length(100);
    left_box.append(settings_entry);
    left_box.append(submit_settings_button);
    submit_settings_button.set_size_request(-1,50);


    // create and connect timer to update_graphs() function; this will cause the function to be run repeatedly.
    sigc::slot<bool()> m_slot = sigc::bind(sigc::mem_fun(*this, &MainWindow::tasks));
    auto conn = Glib::signal_timeout().connect(m_slot, timeout_value);
    tasks_connection = conn;

    DLOG(INFO) << "Finished initializing main window.";

}

// this runs every few ms (see "timeout_value" variable.)
bool MainWindow::tasks() {

    wave_tracer->process_data();
    update_compass();
    return update_graphs();
}

bool MainWindow::update_graphs() {

    // read FFT data from fft processors and write to graphs.
    for (int i = 0; i < num_graphs; i++) {

        for (int j = 0; j < graph_data_count; j++) {

            graphs[i]->data[0][j][0] = (double)(j) * 44200 / fft_processor[i]->get_arr_size() + (j == 0) * graphs[i]->grid.xstart;
            graphs[i]->data[0][j][1] = sqrt(sqrt(fft_processor[i]->out[j][0] * fft_processor[i]->out[j][0] + fft_processor[i]->out[j][1] * fft_processor[i]->out[j][1]));
        }

        // tell graphs to redraw.
        graphs[i]->queue_draw();
    }

    return true;
}

void MainWindow::update_compass() {

    // we must map the direction data provided by the WaveTracer object and turn it into regular intervalled data for the compass.

    double data_arg;
    double data_mag;

    // "interval" represents the angular size of each datapoint for the compass object.
    double interval = 2 * M_PI / compass.get_data_size();

    // clear compass data
    for (int j = 0; j < compass.get_data_size(); j++) {
        compass.data[j] = 0;
    }

    for (int i = 0; i < wave_tracer->data_size; i++) {
        
        // get data point from wavetracer
        wave_tracer->get_avg_data(i, &data_arg, &data_mag);
        for (int j = 0; j < compass.get_data_size(); j++) {
            // add the data to the proper interval in compass's data.
            if (data_arg <= j * interval && data_arg >= 0.001) {
                compass.data[j] += pow(data_mag * .05,3);
                break;
            }
        }
        // repeat for the rest of the data points.
    }

    // calculate and feed mic volumes.
    double volume[3] = {0,0,0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 1000; j++) {
            volume[i] += abs(audio_buffer[wave_tracer->mic_order[i] - 1]->read_value(j-1000));
        }
        compass.mic_volumes[i] = volume[i] / 100;
    }

    compass.queue_draw();
}

// The settings field and button provide a way to change the assigned microphone order to match their order in reality.
// This is necessary since the microphones are indistinguishable (to my knowledge) as usb mics, so they have to be manually
// assigned the correct order.
void MainWindow::on_button_submit_settings() {

    // read text from input field
    Glib::ustring settings = settings_entry.get_text();

    // if there are precicely three input characters, assign each character's ascii value - 48 as the new mic order ( -48 converts the char to the respective int).
    if (settings.length() == 3) {
        microphone_order[0] = settings[0] <= 3 + 48 ? settings[0] - 48 : microphone_order[0];
        microphone_order[1] = settings[1] <= 3 + 48 ? settings[1] - 48 : microphone_order[1];
        microphone_order[2] = settings[2] <= 3 + 48 ? settings[2] - 48 : microphone_order[2];

        // write new mic order to wave tracer for proper directional computing.
        wave_tracer->mic_order[0] = microphone_order[0];
        wave_tracer->mic_order[1] = microphone_order[1];
        wave_tracer->mic_order[2] = microphone_order[2];
        // printf("%d, %d, %d\n", wave_tracer->mic_order[0],wave_tracer->mic_order[1],wave_tracer->mic_order[2]);
    }
}


#endif