#ifndef TURTLE_H
#define TURTLE_H

#include <raylib.h>

#define TURTLE_WHITE RAYWHITE

typedef struct {
    Vector2 position;
    float heading;
    Color color;
    bool penDown;
    float speed;
} Turtle;

typedef void (*TurtleDrawFunc)(void);

void turtle_create(void);
void turtle_close_window(void);
void turtle_init_window(int width, int height, const char* title);
void turtle_set_fps(int fps);
void turtle_set_speed(float speed);
void turtle_forward(float distance);
void turtle_position(float *x, float *y);
void turtle_turn(float angle);
void turtle_right(float angle);
void turtle_left(float angle);
void turtle_pen_up(void);
void turtle_pen_down(void);
void turtle_set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void turtle_draw(void);
void turtle_begin_drawing(void);
void turtle_clear_background(Color color);
void turtle_end_drawing(void);
void turtle_run(TurtleDrawFunc user_draw);
void turtle_set_position(float x, float y);

#endif
