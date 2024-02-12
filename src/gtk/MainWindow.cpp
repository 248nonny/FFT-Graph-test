

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

MainWindow::MainWindow(AudioBuffer **audio_buffer, int num_graphs, AxisType axis_type) :
    num_graphs(num_graphs),
    audio_buffer(audio_buffer)
{


    // setup graph widgets:
    set_child(main_grid);

    graphs = new Graph*[num_graphs];

    for (int i = 0; i < num_graphs; i++) {
        DLOG(INFO) << "i: " << i;
        graphs[i] = Gtk::make_managed<Graph>();
        graphs[i]->data.resize(1);
        graphs[i]->data[0].resize(graph_data_count);

        graphs[i]->grid.ystart = -10;
        graphs[i]->grid.ystop = 10;
        graphs[i]->grid.main_y_line_increment = 1;

        for (int j = 0; j < graph_data_count; j++) {
            graphs[i]->data[0][j].resize(2);
        }

        DLOG(INFO) << "Wrote data";
        main_grid.attach(*graphs[i], 0, i);
        graphs[i]->set_size_request(400 * GUI_SCALE, 100 * GUI_SCALE);
    }

    // create and connect timer to update_graphs() function.
    sigc::slot<bool()> m_slot = sigc::bind(sigc::mem_fun(*this, &MainWindow::tasks));

    auto conn = Glib::signal_timeout().connect(m_slot, timeout_value);
    
    tasks_connection = conn;

}

bool MainWindow::tasks() {
    return update_graphs();;
}

bool MainWindow::update_graphs() {


    // this is a test that reads raw audio and writes it to the graphs.
    int buf_index;

    for (int i = 0; i < num_graphs; i++) {
        buf_index = audio_buffer[i]->get_current_index();

        for (int j = 0; j < graph_data_count; j++) {
            graphs[i]->data[0][j][0] = (double)j / graph_data_count;
            graphs[i]->data[0][j][1] = 10 * audio_buffer[i]->read_value(audio_resolution * (j - graph_data_count), buf_index);
        }

        graphs[i]->queue_draw();
    }

    return true;
}



#endif