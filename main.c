#include <raylib.h>
#include "turtle/turtle.h"

void draw_left_turn() {
    turtle_forward(100);
    turtle_left(90);
    turtle_forward(100);
    turtle_left(90);
    turtle_forward(100);
    turtle_left(90);
    turtle_forward(100);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Left Turn Example");
    turtle_set_fps(60);
    turtle_set_speed(2.0f);

    turtle_run(draw_left_turn);

    return 0;
}
