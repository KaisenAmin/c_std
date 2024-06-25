#include <raylib.h>
#include <math.h>
#include "turtle.h"

#define MAX_LINES 1024

typedef struct {
    Vector2 start;
    Vector2 end;
    Color color;
} Line;

static Turtle turtle;
static Line lines[MAX_LINES];
static int lineCount = 0;
static float distanceRemaining = 0;
static float stepSize = 0;

void turtle_create() {
    turtle.position = (Vector2){400.0f, 300.0f};
    turtle.heading = 0.0f;
    turtle.color = BLACK;
    turtle.penDown = true;
    turtle.speed = 1.0f;
    lineCount = 0;
    distanceRemaining = 0;
    stepSize = 1.0f;
}

void turtle_close_window(void) {
    CloseWindow();
}

void turtle_init_window(int width, int height, const char *title) {
    InitWindow(width, height, title);
}

void turtle_set_fps(int fps) {
    SetTargetFPS(fps);
}

void turtle_set_speed(float speed) {
    turtle.speed = speed;
    stepSize = speed * 5;
}

void turtle_forward(float distance) {
    distanceRemaining = distance;
    float rad = DEG2RAD * turtle.heading;

    while (distanceRemaining > 0) {
        float move = fminf(stepSize, distanceRemaining);
        Vector2 start = turtle.position;

        turtle.position.x += move * cosf(rad);
        turtle.position.y += move * sinf(rad);

        if (turtle.penDown && lineCount < MAX_LINES) {
            lines[lineCount].start = start;
            lines[lineCount].end = turtle.position;
            lines[lineCount].color = turtle.color;
            lineCount++;
        }

        distanceRemaining -= move;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < lineCount; i++) {
            DrawLineV(lines[i].start, lines[i].end, lines[i].color);
        }
        DrawCircleV(turtle.position, 5, turtle.color);
        EndDrawing();

        if (distanceRemaining > 0) {
            WaitTime(0.01f);
        }
    }
}

void turtle_position(float *x, float *y) {
    *x = turtle.position.x;
    *y = turtle.position.y;
}

void turtle_turn(float angle) {
    turtle.heading += angle;
}

void turtle_right(float angle) {
    turtle.heading -= angle;
}

void turtle_left(float angle) {
    turtle.heading += angle;
}

void turtle_pen_up() {
    turtle.penDown = false;
}

void turtle_pen_down() {
    turtle.penDown = true;
}

void turtle_set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    turtle.color = (Color){ r, g, b, a };
}

void turtle_draw() {
    for (int i = 0; i < lineCount; i++) {
        DrawLineV(lines[i].start, lines[i].end, lines[i].color);
    }
    DrawCircleV(turtle.position, 5, turtle.color);
}

void turtle_begin_drawing(void) {
    BeginDrawing();
}

void turtle_clear_background(Color color) {
    ClearBackground(color);
}

void turtle_end_drawing(void) {
    EndDrawing();
}

void turtle_run(TurtleDrawFunc user_draw) {
    while (!WindowShouldClose()) {
        turtle_begin_drawing();
        turtle_clear_background(TURTLE_WHITE);
        user_draw();
        turtle_draw();
        turtle_end_drawing();
    }

    turtle_close_window();
}

void turtle_set_position(float x, float y) {
    if (turtle.penDown && lineCount < MAX_LINES) {
        lines[lineCount].start = turtle.position;
        lines[lineCount].end = (Vector2){x, y};
        lines[lineCount].color = turtle.color;
        lineCount++;
    }
    
    turtle.position = (Vector2){x, y};
}
