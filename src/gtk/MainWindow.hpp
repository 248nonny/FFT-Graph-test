#ifdef GTK_AUDIO_TEST // only compile if this is defined in CMakeLists.txt.

#include <gtkmm.h>
#include <vector>

#include "src/graph/Graph.hpp"
#include "src/audio/AudioBuffer.hpp"


class MainWindow : public Gtk::Window
{
public:
    MainWindow(int num_graphs = 2, AxisType axis_type = AxisType::LINEAR);

    Graph **graphs;

    Gtk::Grid main_grid;

    int get_num_graphs() {return num_graphs; }
private:


    int num_graphs;

    GraphDataSet **data_set;

};


#endif