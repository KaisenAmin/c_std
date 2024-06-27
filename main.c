#include "turtle/turtle.h"
#include <math.h>


void draw_spiral(Turtle *state) {
    static bool drawn = false;

    if (!drawn) {
        for (int i = 0; i < 360; i++) {
            // Set the color based on the angle to create a gradient effect
            int red = (int)(127 * (1 + cos(DEG2RAD * i)));
            int green = (int)(127 * (1 + sin(DEG2RAD * i)));
            int blue = 255 - red;

            turtle_set_color(state, red, green, blue, 255);
            turtle_forward(state, i * 0.5);
            turtle_right(state, 59); 
        }
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Spiral Pattern Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 5.0f);

    turtle_done(state, draw_spiral);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}