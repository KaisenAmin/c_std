#include "plot/plot.h"

int main() {
    float x[5] = {1,2,3,4,5};
    float y[5] = {2,4,6,8,10};
    Plot* p = plot_create("Line Plot", "X", "Y");

    plot_add_series(p, x, y, 5, "y=2x", PLTYPE_LINE, plot_color(0,0,255,255));
    plot_draw(p);
    plot_destroy(p);

    return 0;
}