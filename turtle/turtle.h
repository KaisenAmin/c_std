#ifndef TURTLE_H
#define TURTLE_H

#include <raylib.h>

typedef struct {
    Vector2 start;
    Vector2 end;
    Color color;
    float thickness;
} Line;

typedef struct {
    Vector2 position;
    Color color;
    int id;
} Stamp;

typedef struct {
    Vector2 position;
    float heading;
    Color pencolor;
    Color fillcolor;
    bool penDown;
    bool filling;
    float speed;
    float thickness;
} Tu;

typedef struct {
    Tu *turtle;
    Line *lines;
    int lineCount;
    int lineCapacity;
    Stamp *stamps;
    int stampCount;
    int stampCapacity;
    int nextStampID;
    float distanceRemaining;
    float stepSize;
    float fullcircle_degrees;
    Color backgroundColor;
    bool isFilling;
    Color fillColor;
    struct {
        float size;
        Color color;
        bool drawn;
    } dot;
} Turtle;

typedef void (*TurtleDrawFunc)(Turtle *);

Turtle *turtle_create(void);

void turtle_deallocate(Turtle *state);
void turtle_init_window(int width, int height, const char *title);
void turtle_close_window(void);
void turtle_set_fps(int fps);
void turtle_set_speed(Turtle *state, float speed);
void turtle_pen_size(Turtle *state, float width);
void turtle_forward(Turtle *state, float distance);
void turtle_position(Turtle *state, float *x, float *y);
void turtle_turn(Turtle *state, float angle);
void turtle_right(Turtle *state, float angle);
void turtle_left(Turtle *state, float angle);
void turtle_pen_up(Turtle *state);
void turtle_pen_down(Turtle *state);
void turtle_set_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void turtle_color(Turtle *state, const char *pencolor_str, const char *fillcolor_str);
void turtle_color_rgb(Turtle *state, unsigned char pr, unsigned char pg, unsigned char pb, unsigned char fr, unsigned char fg, unsigned char fb);
void turtle_begin_fill(Turtle *state);
void turtle_end_fill(Turtle *state);
void turtle_draw(Turtle *state);
void turtle_begin_drawing(void);
void turtle_clear_background(Color color);
void turtle_end_drawing(void);
void turtle_done(Turtle *state, TurtleDrawFunc user_draw);
void turtle_set_position(Turtle *state, float x, float y);
void turtle_set_x(Turtle *state, float x);
void turtle_set_y(Turtle *state, float y);
void turtle_set_heading(Turtle *state, float to_angle);
void turtle_home(Turtle *state);
void turtle_circle(Turtle *state, float radius, float extent, int steps);
void turtle_dot(Turtle *state, float size, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void turtle_clear_stamp(Turtle *state, int stamp_id);
void turtle_degrees(Turtle *state, float fullcircle);
void turtle_radians(Turtle *state);
void turtle_set_background_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

int turtle_stamp(Turtle *state);

float turtle_distance(Turtle *state, float x, float y);
float turtle_heading(Turtle *state);
float turtle_distance_turtle(Turtle *state, Tu *other_turtle);
float turtle_get_pen_size(Turtle *state);

bool turtle_filling(Turtle *state);
bool turtle_is_down(Turtle *state);

#endif 
