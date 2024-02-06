

#ifdef GTK_AUDIO_TEST


#include <glog/logging.h>
#include <vector>
#include <portaudio.h>
#include <thread>


#include "MainWindow.hpp"
#include "src/graph/Graph.hpp"

MainWindow::MainWindow(int num_graphs, AxisType axis_type) :
    num_graphs(num_graphs)
{


    // setup graph widgets:
    set_child(main_grid);

    graphs = new Graph*[num_graphs];

    data_set = new GraphDataSet*[num_graphs];

    for (int i = 0; i < num_graphs; i++) {
        data_set[i] = new GraphDataSet;
        data_set[i][0].resize(1);
        data_set[i][0][0].resize(512);
        DLOG(INFO) << "i: " << i;
        graphs[i] = Gtk::make_managed<Graph>();
        main_grid.attach(*graphs[i], 0, i);
        graphs[i]->set_size_request(400 * GUI_SCALE, 100 * GUI_SCALE);
    }

}




#endif