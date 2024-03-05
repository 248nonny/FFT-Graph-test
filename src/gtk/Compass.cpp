#include "Compass.hpp"

#include <cmath>
#include <glog/logging.h>


Compass::Compass() {

    // set Gtk related params
    set_vexpand(true);
    set_hexpand(true);
    set_margin(0);
    set_draw_func(sigc::mem_fun(*this, &Compass::on_draw));

    // set minimum size in px
    set_size_request(100,100);

}

void Compass::allocate_data(int data_size) {
    delete data;
    data = new double[data_size];

    this->data_size = data_size;
}

void Compass::on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
    

    this->width = width;
    this->height = height;
    get_trnfrm();


    draw_data(cr);

    // draw compass
    cr->set_line_width(scale * 6);
    cr->arc(trnfrm[0](0), trnfrm[1](0), min * outer_compass_radius / 200.0, 0, 2 * M_PI);
    cr->move_to(trnfrm[0](inner_compass_radius),trnfrm[1](0));
    cr->arc(trnfrm[0](0), trnfrm[1](0), min * inner_compass_radius / 200.0, 0, 2 * M_PI);
    cr->set_source_rgb(.8, .8, .8);
    cr->stroke();

    // draw microphones with volumes.
    cr->arc(trnfrm[0](0),trnfrm[1](mic_drawing_distance), scale * 1.6 * mic_drawing_radius, 0, 2 * M_PI);
    cr->set_source_rgba(0, 1, 1, .5);
    cr->set_line_width(scale * 30 * mic_volumes[0]);
    cr->stroke_preserve();
    cr->set_source_rgba(.8, .8, .8, 1);
    cr->fill();

    cr->arc(trnfrm[0](mic_drawing_distance * cos(M_PI_2 / 3.0)), trnfrm[1](-mic_drawing_distance * sin(M_PI_2 / 3.0)), scale * 1.6 * mic_drawing_radius, 0, 2 * M_PI);
    cr->set_source_rgba(0, 1, 1, .5);
    cr->set_line_width(scale * 30 * mic_volumes[1]);
    cr->stroke_preserve();
    cr->set_source_rgba(.8, .8, .8, 1);
    cr->fill();

    cr->arc(trnfrm[0](-mic_drawing_distance * cos(M_PI_2 / 3.0)), trnfrm[1](-mic_drawing_distance * sin(M_PI_2 / 3.0)), scale * 1.6 * mic_drawing_radius, 0, 2 * M_PI);
    cr->set_source_rgba(0, 1, 1, .5);
    cr->set_line_width(scale * 30 * mic_volumes[2]);
    cr->stroke_preserve();
    cr->set_source_rgba(.8, .8, .8, 1);
    cr->fill();



}


void Compass::get_trnfrm() {
    // we want to draw in a square.
    // the square will have coordinate (0,0) in its center,
    // and will have limits ±100 for both x and y coords.

    this->min = width < height ? width : height;
    this->scale = min / (GUI_SCALE * 400.0);

    trnfrm[0] = [this](double xval) {return width / 2.0 + min * xval / 200.0;};
    trnfrm[1] = [this](double yval) {return height / 2.0 - min * yval / 200.0;};
}

// use arcs to draw an enclosed sector given starting and finishing angles and starting and finishing radii.
void Compass::draw_sector(const Cairo::RefPtr<Cairo::Context>& cr, double xc, double yc, double r1, double r2, double a1, double a2) {

    cr->move_to(trnfrm[0](xc + r1 * cos(a1)), trnfrm[1](yc + r1 * sin(a1)));
    cr->arc_negative(trnfrm[0](xc), trnfrm[1](yc), min * r1 / 200.0, -a1, -a2);
    cr->line_to(trnfrm[0](xc + r2 * cos(a2)), trnfrm[1](yc + r2 * sin(a2)));
    cr->arc(trnfrm[0](xc), trnfrm[1](yc), min * r2 / 200.0, -a2, -a1);
    cr->line_to(trnfrm[0](xc + r1 * cos(a1)), trnfrm[1](yc + r1 * sin(a1)));
    cr->close_path();
}

// a more intuitive arc function that can be used with respect to the new x,y coordinate system.
void Compass::draw_arc(const Cairo::RefPtr<Cairo::Context>& cr, double xc, double yc, double r, double a1, double a2) {
    cr->arc_negative(trnfrm[0](xc),trnfrm[1](yc),min * r / 200.0,-a1,-a2);
}

// draw sectors with opacity proportional to the magnitude of the data that corresponds to that sector.
void Compass::draw_data(const Cairo::RefPtr<Cairo::Context>& cr) {

    cr->set_line_width(scale/3);

    for (int i = 0; i < data_size; i++) {
        draw_sector(cr, 0, 0, outer_compass_radius, inner_compass_radius, i * 2 * M_PI / data_size, (i+1) * 2 * M_PI / data_size);
        cr->set_source_rgba(data_rgb[0], data_rgb[1], data_rgb[2], data[i]);
        cr->fill();
    }
}