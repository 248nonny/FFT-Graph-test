#include <gtkmm.h>

#include "src/gtk/Compass.hpp"

class TestWindow : public Gtk::Window
{
public:
    TestWindow();

    Gtk::Box box;

    Compass audio_compass;    
};

TestWindow::TestWindow() {
    audio_compass.allocate_data(1000);
    box.append(audio_compass);
    set_child(box);
}


int main(int argc, char *argv[]) {

    auto app = Gtk::Application::create("org.gtkmm.example");
    app->make_window_and_run<TestWindow>(argc, argv);
}