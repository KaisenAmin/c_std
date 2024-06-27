#include "turtle/turtle.h"

void draw_circle(Turtle *state) {
    static bool drawn = false; 
    int n = 10;

    if (!drawn) {
        while (n <= 60) {
            turtle_circle(state, n, 360, 36);  
            n += 10;
        }
        drawn = true; 
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Circle Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_circle);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
