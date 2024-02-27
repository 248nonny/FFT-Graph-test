

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

    graphs = new Graph*[num_graphs];

    compass.allocate_data(100);

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

    submit_settings_button.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_button_submit_settings));

    main_grid.attach(left_box, 0, 0);
    main_grid.attach(graph_box, 1, 0);

    left_box.set_hexpand(true);
    graph_box.set_hexpand(false);
    graph_box.set_size_request(800,-1);
    left_box.append(compass);
    settings_entry.set_max_length(100);
    // settings_entry.set_expand(true);
    left_box.append(settings_entry);
    left_box.append(submit_settings_button);
    submit_settings_button.set_size_request(-1,50);

    // create and connect timer to update_graphs() function.
    sigc::slot<bool()> m_slot = sigc::bind(sigc::mem_fun(*this, &MainWindow::tasks));

    auto conn = Glib::signal_timeout().connect(m_slot, timeout_value);
    
    tasks_connection = conn;

    DLOG(INFO) << "Finished initializing main window.";

}

bool MainWindow::tasks() {

    wave_tracer->process_data();

    update_compass();

    return update_graphs();
}

bool MainWindow::update_graphs() {


    // this is a test that reads raw audio and writes it to the graphs.
    int buf_index;

    for (int i = 0; i < num_graphs; i++) {
        buf_index = audio_buffer[i]->get_current_index();

        for (int j = 0; j < graph_data_count; j++) {
            // graphs[i]->data[0][j][0] = (double)j / graph_data_count;
            // graphs[i]->data[0][j][1] = 10 * audio_buffer[i]->read_value(audio_resolution * (j - graph_data_count), buf_index);
            // DLOG(INFO) << "ggasdasd";

            graphs[i]->data[0][j][0] = (double)(j) * 44200 / fft_processor[i]->get_arr_size() + (j == 0) * graphs[i]->grid.xstart;
            // graphs[i]->data[0][j][0] = 10;
            // graphs[i]->data[0][0][0] = 0;
            graphs[i]->data[0][j][1] = sqrt(sqrt(fft_processor[i]->out[j][0] * fft_processor[i]->out[j][0] + fft_processor[i]->out[j][1] * fft_processor[i]->out[j][1]));
        }

        graphs[i]->queue_draw();
    }

    return true;
}

void MainWindow::update_compass() {

    std::function<double(double)> fn;
    double data_arg;
    double data_mag;

    double interval = 2 * M_PI / compass.get_data_size();

    // clear data
    for (int j = 0; j < compass.get_data_size(); j++) {
        compass.data[j] = 0;
    }

    for (int i = 0; i < wave_tracer->data_size; i++) {
        // data_arg = wave_tracer->data[i][0];
        // data_mag = wave_tracer->data[i][1];
        wave_tracer->get_avg_data(i, &data_arg, &data_mag);

        for (int j = 0; j < compass.get_data_size(); j++) {
            // compass.data[i] = 0;
            if (data_arg <= j * interval && data_arg >= 0.001) {
                compass.data[j] += pow(data_mag * .05,3);
                // if (data_mag > 0.3)
                //     printf("i: %4d, j: %4d, arg: %9.5lf, real arg: %9.5lf mag: %9.5lf\n", i, j, data_arg, j * interval, data_mag);
                break;
            }
        }
    }

    // calculate and feed mic volumes.
    double volume[3] = {0,0,0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 1000; j++) {
            volume[i] += audio_buffer[wave_tracer->mic_order[i] - 1]->read_value(j-1000);
        }
        compass.mic_volumes[i] = volume[i] / 1000;
    }

    compass.queue_draw();
}

void MainWindow::on_button_submit_settings() {
    Glib::ustring settings = settings_entry.get_text();

    if (settings.length() == 3) {
        microphone_order[0] = settings[0] <= 3 + 48 ? settings[0] - 48 : microphone_order[0];
        microphone_order[1] = settings[1] <= 3 + 48 ? settings[1] - 48 : microphone_order[1];
        microphone_order[2] = settings[2] <= 3 + 48 ? settings[2] - 48 : microphone_order[2];
        wave_tracer->mic_order[0] = microphone_order[0];
        wave_tracer->mic_order[1] = microphone_order[1];
        wave_tracer->mic_order[2] = microphone_order[2];
        printf("%d, %d, %d\n", wave_tracer->mic_order[0],wave_tracer->mic_order[1],wave_tracer->mic_order[2]);
    }
}


#endif