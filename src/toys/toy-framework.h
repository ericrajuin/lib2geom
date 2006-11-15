#include <iostream>
#include <sstream>
#include <gtk/gtk.h>
#include <cairo.h>
#include <vector>

#include "interactive-bits.h"
#include "point.h"
#include "geom.h"

using std::vector;

//Utility functions
double uniform();
void draw_number(cairo_t *cr, Geom::Point pos, int num);

class Toy {
public:
    vector<Geom::Point> handles;
    bool mouse_down;
    Geom::Point *selected_handle, old_mouse_point;

    Toy() { mouse_down = false; }

    virtual void draw(cairo_t *cr, std::ostringstream *notify, int w, int h, bool save);

    virtual void mouse_moved(GdkEventMotion* e);
    virtual void mouse_pressed(GdkEventButton* e);
    virtual void mouse_released(GdkEventButton* e);

    virtual void key_hit(GdkEventKey *e) {}

    virtual bool should_draw_numbers() { return true; }
};

//Framework Accesors
void redraw();
void init(int argc, char **argv, char *title, Toy *t);