#pragma once

#include "gtkmm/drawingarea.h"

// This class is a custom Gtk widget based on the DrawingArea, which serves the purpose of
// visualizing the directional audio data that the WaveTrace POC spits out.
class Compass : public Gtk::DrawingArea {
public:
    Compass();

    double *data = nullptr; // values should range from 0 to 1.

    void allocate_data(int data_size);

    double get_data_size() {
        return this->data_size;
    }

    double mic_volumes[3] = {0.4, 0.8, 0.3}; // should also range from 0 to 1.

private:

    int width = 0;
    int height = 0;
    int min = 0;
    double scale = 1;
    int data_size = 0;

    double outer_compass_radius = 80;
    double inner_compass_radius = 50;
    double mic_drawing_distance = 35;
    double mic_drawing_radius   = 10;

    double data_rgb[3] = {1, 0.2, 0};

    void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
    std::function<double(double)> trnfrm[2];

    void draw_data(const Cairo::RefPtr<Cairo::Context>& cr);

    // custom drawing functions to draw relevant shapes more easily
    void draw_sector(const Cairo::RefPtr<Cairo::Context>& cr, double xc, double yc, double r1, double r2, double a1, double a2);
    void draw_arc(const Cairo::RefPtr<Cairo::Context>& cr, double xc, double yc, double r, double a1, double a2);

    // function that calculates a transformation from an arbitrary x,y coordinate system to literal pixels;
    // useful for easy scaling.
    void get_trnfrm();

};