#include <math.h>
#include <stdlib.h>
#include "turtle.h"


#define INITIAL_CAPACITY 1024

static Color get_color_from_hex(const char *hex) {
    unsigned int r, g, b;
    TURTLE_LOG("[get_color_from_hex]: Parsing color from hex %s", hex);

    if (sscanf(hex, "#%02x%02x%02x", &r, &g, &b) == 3) {
        TURTLE_LOG("[get_color_from_hex]: Parsed color - R: %u, G: %u, B: %u", r, g, b);
        return (Color){r, g, b, 255};
    }

    TURTLE_LOG("[get_color_from_hex]: Failed to parse color, returning BLACK.");
    return BLACK; // Default color if parsing fails
}

Turtle *turtle_create(void) {
    TURTLE_LOG("[turtle_create]: Creating new turtle.");
    Turtle *state = (Turtle *)malloc(sizeof(Turtle));
    if (!state) {
        TURTLE_LOG("Error: Failed to allocate memory for turtle state\n");
        exit(EXIT_FAILURE);
    }

    state->turtle = (Tu *)malloc(sizeof(Tu));
    if (!state->turtle) {
        TURTLE_LOG("Error: Failed to allocate memory for turtle\n");
        exit(EXIT_FAILURE);
    }

    state->turtle->position = (Vector2){400.0f, 300.0f};
    state->turtle->heading = 0.0f;
    state->turtle->pencolor = BLACK;
    state->turtle->fillcolor = BLACK;
    state->turtle->penDown = true;
    state->turtle->filling = false;
    state->turtle->speed = 1.0f;
    state->turtle->thickness = 1.0f;

    state->lineCount = 0;
    state->stampCount = 0;
    state->nextStampID = 1;
    state->distanceRemaining = 0;
    state->stepSize = 1.0f;

    state->dot.size = 0;
    state->dot.color = (Color){0, 0, 0, 0};
    state->dot.drawn = false;
    state->fullcircle_degrees = 360.0f;
    state->backgroundColor = WHITE;

    state->lines = (Line *)malloc(INITIAL_CAPACITY * sizeof(Line));
    if (!state->lines) {
        TURTLE_LOG("Error: Failed to allocate memory for lines\n");
        exit(EXIT_FAILURE);
    }
    state->lineCapacity = INITIAL_CAPACITY;

    state->stamps = (Stamp *)malloc(INITIAL_CAPACITY * sizeof(Stamp));
    if (!state->stamps) {
        TURTLE_LOG("Error: Failed to allocate memory for stamps\n");
        exit(EXIT_FAILURE);
    }
    state->stampCapacity = INITIAL_CAPACITY;

    state->fillPoints = (Vector2 *)malloc(INITIAL_CAPACITY * sizeof(Vector2));
    if (!state->fillPoints) {
        TURTLE_LOG("Error: Failed to allocate memory for fill points\n");
        exit(EXIT_FAILURE);
    }
    state->fillPointCapacity = INITIAL_CAPACITY;
    state->fillPointCount = 0;

    TURTLE_LOG("[turtle_create]: Turtle created successfully.");
    return state;
}

void turtle_deallocate(Turtle *state) {
    TURTLE_LOG("[turtle_deallocate]: Deallocating turtle.");
    if (state) {
        free(state->turtle);
        free(state->lines);
        free(state->stamps);
        free(state->fillPoints);
        free(state);
        TURTLE_LOG("[turtle_deallocate]: Turtle deallocated successfully.");
    }
}

void turtle_close_window(void) {
    TURTLE_LOG("[turtle_close_window]: Closing window.");
    CloseWindow();
}

void turtle_init_window(int width, int height, const char *title) {
    TURTLE_LOG("[turtle_init_window]: Initializing window (%d x %d) with title: %s", width, height, title);
    InitWindow(width, height, title);
}

void turtle_set_fps(int fps) {
    TURTLE_LOG("[turtle_set_fps]: Setting FPS to %d", fps);
    SetTargetFPS(fps);
}

void turtle_set_speed(Turtle *state, float speed) {
    TURTLE_LOG("[turtle_set_speed]: Setting turtle speed to %.2f", speed);
    state->turtle->speed = speed;
    state->stepSize = speed * 5;
}

void turtle_pen_size(Turtle *state, float width) {
    TURTLE_LOG("[turtle_pen_size]: Setting pen size to %.2f", width);
    state->turtle->thickness = width;
}

float turtle_get_pen_size(Turtle *state) {
    TURTLE_LOG("[turtle_get_pen_size]: Getting pen size.");
    return state->turtle->thickness;
}

bool turtle_is_down(Turtle *state) {
    TURTLE_LOG("[turtle_is_down]: Checking if pen is down.");
    return state->turtle->penDown;
}

static void resize_lines_if_needed(Turtle *state) {
    if (state->lineCount >= state->lineCapacity) {
        state->lineCapacity *= 2;
        state->lines = (Line *)realloc(state->lines, state->lineCapacity * sizeof(Line));

        if (!state->lines) {
            TURTLE_LOG("Error: Failed to reallocate memory for lines\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void resize_stamps_if_needed(Turtle *state) {
    if (state->stampCount >= state->stampCapacity) {
        TURTLE_LOG("[resize_stamps_if_needed]: Resizing stamps array. New capacity: %d", state->stampCapacity * 2);
        state->stampCapacity *= 2;
        state->stamps = (Stamp *)realloc(state->stamps, state->stampCapacity * sizeof(Stamp));

        if (!state->stamps) {
            TURTLE_LOG("Error: Failed to reallocate memory for stamps\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void resize_fill_points_if_needed(Turtle *state) {
    if (state->fillPointCount >= state->fillPointCapacity) {
        TURTLE_LOG("[resize_fill_points_if_needed]: Resizing fill points array. New capacity: %d", state->fillPointCapacity * 2);
        state->fillPointCapacity *= 2;
        state->fillPoints = (Vector2 *)realloc(state->fillPoints, state->fillPointCapacity * sizeof(Vector2));

        if (!state->fillPoints) {
            TURTLE_LOG("Error: Failed to reallocate memory for fill points\n");
            exit(EXIT_FAILURE);
        }
    }
}

void turtle_forward(Turtle *state, float distance) {
    TURTLE_LOG("[turtle_forward]: Moving turtle forward by %.2f units.", distance);
    state->distanceRemaining = distance;
    float rad = DEG2RAD * state->turtle->heading;

    while (state->distanceRemaining > 0) {
        float move = fminf(state->stepSize, state->distanceRemaining);  // Use a smaller step size for smoother movement
        Vector2 start = state->turtle->position;

        state->turtle->position.x += move * cosf(rad);
        state->turtle->position.y += move * sinf(rad);

        TURTLE_LOG("[turtle_forward]: Turtle position updated to (%.2f, %.2f).", state->turtle->position.x, state->turtle->position.y);

        if (state->turtle->penDown) {
            TURTLE_LOG("[turtle_forward]: Drawing line from (%.2f, %.2f) to (%.2f, %.2f).", start.x, start.y, state->turtle->position.x, state->turtle->position.y);
            resize_lines_if_needed(state);
            state->lines[state->lineCount].start = start;
            state->lines[state->lineCount].end = state->turtle->position;
            state->lines[state->lineCount].color = state->turtle->pencolor;
            state->lines[state->lineCount].thickness = state->turtle->thickness;
            state->lineCount++;
        }

        if (state->turtle->filling) {
            TURTLE_LOG("[turtle_forward]: Adding point to fill at (%.2f, %.2f).", state->turtle->position.x, state->turtle->position.y);
            resize_fill_points_if_needed(state);
            state->fillPoints[state->fillPointCount++] = state->turtle->position;
        }

        state->distanceRemaining -= move;

        BeginDrawing();
        ClearBackground(state->backgroundColor);
        for (int i = 0; i < state->lineCount; i++) {
            DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
        }
        for (int i = 0; i < state->stampCount; i++) {
            DrawCircleV(state->stamps[i].position, 5, state->stamps[i].color);
        }
        DrawCircleV(state->turtle->position, 5, state->turtle->pencolor);
        if (state->dot.drawn) {
            DrawCircleV(state->turtle->position, state->dot.size / 2, state->dot.color);
        }
        EndDrawing();

        if (state->distanceRemaining > 0) {
            WaitTime(0.005f);  // Adjust wait time for smoother animation
        }
    }
}

void turtle_position(Turtle *state, float *x, float *y) {
    TURTLE_LOG("[turtle_position]: Turtle position requested. Current position: (%.2f, %.2f).", state->turtle->position.x, state->turtle->position.y);
    *x = state->turtle->position.x;
    *y = state->turtle->position.y;
}

void turtle_turn(Turtle *state, float angle) {
    TURTLE_LOG("[turtle_turn]: Turning turtle by %.2f degrees.", angle);
    state->turtle->heading += angle;
}

void turtle_right(Turtle *state, float angle) {
    TURTLE_LOG("[turtle_right]: Turning turtle right by %.2f degrees.", angle);
    state->turtle->heading -= angle;
}

void turtle_left(Turtle *state, float angle) {
    TURTLE_LOG("[turtle_left]: Turning turtle left by %.2f degrees.", angle);
    state->turtle->heading += angle;
}

void turtle_pen_up(Turtle *state) {
    TURTLE_LOG("[turtle_pen_up]: Lifting pen up.");
    state->turtle->penDown = false;
}

void turtle_pen_down(Turtle *state) {
    TURTLE_LOG("[turtle_pen_down]: Putting pen down.");
    state->turtle->penDown = true;
}

void turtle_set_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    TURTLE_LOG("[turtle_set_color]: Setting pen color to RGBA(%d, %d, %d, %d).", r, g, b, a);
    state->turtle->pencolor = (Color){r, g, b, a};
}

void turtle_color(Turtle *state, const char *pencolor_str, const char *fillcolor_str) {
    TURTLE_LOG("[turtle_color]: Setting pen color to %s and fill color to %s.", pencolor_str, fillcolor_str);
    state->turtle->pencolor = get_color_from_hex(pencolor_str);
    state->turtle->fillcolor = get_color_from_hex(fillcolor_str);
}

void turtle_color_rgb(Turtle *state, unsigned char pr, unsigned char pg, unsigned char pb, unsigned char fr, unsigned char fg, unsigned char fb) {
    TURTLE_LOG("[turtle_color_rgb]: Setting pen color to RGB(%d, %d, %d) and fill color to RGB(%d, %d, %d).", pr, pg, pb, fr, fg, fb);
    state->turtle->pencolor = (Color){pr, pg, pb, 255};
    state->turtle->fillcolor = (Color){fr, fg, fb, 255};
}

bool turtle_filling(Turtle *state) {
    TURTLE_LOG("[turtle_filling]: Checking if turtle is filling: %s.", state->turtle->filling ? "true" : "false");
    return state->turtle->filling;
}

void turtle_begin_fill(Turtle *state) {
    TURTLE_LOG("[turtle_begin_fill]: Starting fill.");
    state->turtle->filling = true;
    state->fillPointCount = 0; // Reset fill point count
    resize_fill_points_if_needed(state);
    state->fillPoints[state->fillPointCount++] = state->turtle->position;
}

void turtle_end_fill(Turtle *state) {
    TURTLE_LOG("[turtle_end_fill]: Ending fill with %d points.", state->fillPointCount);

    if (state->fillPointCount > 2) {
        TURTLE_LOG("[turtle_end_fill]: Drawing filled shape.");
        BeginDrawing();
        ClearBackground(state->backgroundColor);
        
        // Draw existing lines
        for (int i = 0; i < state->lineCount; i++) {
            DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
        }

        // Draw the filled shape
        DrawTriangleFan(state->fillPoints, state->fillPointCount, state->turtle->fillcolor);
        
        // Draw lines again on top
        for (int i = 0; i < state->lineCount; i++) {
            DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
        }

        EndDrawing();
    }
    state->turtle->filling = false;
}

void turtle_draw(Turtle *state) {
    TURTLE_LOG("[turtle_draw]: Begin drawing.");

    BeginDrawing();
    ClearBackground(state->backgroundColor);

    if (state->fillPointCount > 2) {
        TURTLE_LOG("[turtle_draw]: Drawing fill with %d points.", state->fillPointCount);
        DrawTriangleFan(state->fillPoints, state->fillPointCount, state->turtle->fillcolor);
    }

    for (int i = 0; i < state->lineCount; i++) {
        TURTLE_LOG("[turtle_draw]: Drawing line %d from (%.2f, %.2f) to (%.2f, %.2f).", i, state->lines[i].start.x, state->lines[i].start.y, state->lines[i].end.x, state->lines[i].end.y);
        DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
    }

    for (int i = 0; i < state->stampCount; i++) {
        TURTLE_LOG("[turtle_draw]: Drawing stamp %d at position (%.2f, %.2f).", i, state->stamps[i].position.x, state->stamps[i].position.y);
        DrawCircleV(state->stamps[i].position, 5, state->stamps[i].color);
    }

    TURTLE_LOG("[turtle_draw]: Drawing turtle at position (%.2f, %.2f).", state->turtle->position.x, state->turtle->position.y);
    DrawCircleV(state->turtle->position, 5, state->turtle->pencolor);

    if (state->dot.drawn) {
        TURTLE_LOG("[turtle_draw]: Drawing dot at position (%.2f, %.2f) with size %.2f.", state->turtle->position.x, state->turtle->position.y, state->dot.size);
        DrawCircleV(state->turtle->position, state->dot.size / 2, state->dot.color);
    }

    EndDrawing();
    TURTLE_LOG("[turtle_draw]: End drawing.");
}

void turtle_begin_drawing(void) {
    TURTLE_LOG("[turtle_begin_drawing]: Begin drawing.");
    BeginDrawing();
}

void turtle_clear_background(Color color) {
    TURTLE_LOG("[turtle_clear_background]: Clearing background with color (r: %d, g: %d, b: %d, a: %d).", color.r, color.g, color.b, color.a);
    ClearBackground(color);
}

void turtle_end_drawing(void) {
    TURTLE_LOG("[turtle_end_drawing]: End drawing.");
    EndDrawing();
}

void turtle_done(Turtle *state, TurtleDrawFunc user_draw) {
    TURTLE_LOG("[turtle_done]: Begin main draw loop.");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(state->backgroundColor); 

        if (state->fillPointCount > 2) {
            TURTLE_LOG("[turtle_done]: Drawing fill with %d points.", state->fillPointCount);
            DrawTriangleFan(state->fillPoints, state->fillPointCount, state->turtle->fillcolor);
        }

        for (int i = 0; i < state->lineCount; i++) {
            TURTLE_LOG("[turtle_done]: Drawing line %d from (%.2f, %.2f) to (%.2f, %.2f).", i, state->lines[i].start.x, state->lines[i].start.y, state->lines[i].end.x, state->lines[i].end.y);
            DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
        }

        for (int i = 0; i < state->stampCount; i++) {
            TURTLE_LOG("[turtle_done]: Drawing stamp %d at position (%.2f, %.2f).", i, state->stamps[i].position.x, state->stamps[i].position.y);
            DrawCircleV(state->stamps[i].position, 5, state->stamps[i].color);
        }

        user_draw(state);

        TURTLE_LOG("[turtle_done]: Drawing turtle at position (%.2f, %.2f).", state->turtle->position.x, state->turtle->position.y);
        DrawCircleV(state->turtle->position, 5, state->turtle->pencolor);

        if (state->dot.drawn) {
            TURTLE_LOG("[turtle_done]: Drawing dot at position (%.2f, %.2f) with size %.2f.", state->turtle->position.x, state->turtle->position.y, state->dot.size);
            DrawCircleV(state->turtle->position, state->dot.size / 2, state->dot.color);
        }

        EndDrawing();
    }

    TURTLE_LOG("[turtle_done]: Exiting main draw loop and closing window.");
    turtle_close_window();
}

void turtle_set_position(Turtle *state, float x, float y) {
    TURTLE_LOG("[turtle_set_position]: Setting position to (%.2f, %.2f)", x, y);
    
    if (state->turtle->penDown) {
        TURTLE_LOG("[turtle_set_position]: Pen is down. Drawing line from (%.2f, %.2f) to (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, x, y);
        resize_lines_if_needed(state);
        state->lines[state->lineCount].start = state->turtle->position;
        state->lines[state->lineCount].end = (Vector2){x, y};
        state->lines[state->lineCount].color = state->turtle->pencolor;
        state->lines[state->lineCount].thickness = state->turtle->thickness;
        state->lineCount++;
    }
    state->turtle->position = (Vector2){x, y};

    if (state->turtle->filling) {
        TURTLE_LOG("[turtle_set_position]: Adding fill point at (%.2f, %.2f)", x, y);
        resize_fill_points_if_needed(state);
        state->fillPoints[state->fillPointCount++] = state->turtle->position;
    }
}

void turtle_set_x(Turtle *state, float x) {
    TURTLE_LOG("[turtle_set_x]: Setting X position to %.2f", x);
    
    if (state->turtle->penDown) {
        TURTLE_LOG("[turtle_set_x]: Pen is down. Drawing line from (%.2f, %.2f) to (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, x, state->turtle->position.y);
        resize_lines_if_needed(state);

        state->lines[state->lineCount].start = state->turtle->position;
        state->lines[state->lineCount].end = (Vector2){x, state->turtle->position.y};
        state->lines[state->lineCount].color = state->turtle->pencolor;
        state->lines[state->lineCount].thickness = state->turtle->thickness;
        state->lineCount++;
    }
    state->turtle->position.x = x;

    if (state->turtle->filling) {
        TURTLE_LOG("[turtle_set_x]: Adding fill point at (%.2f, %.2f)", x, state->turtle->position.y);
        resize_fill_points_if_needed(state);
        state->fillPoints[state->fillPointCount++] = state->turtle->position;
    }
}

void turtle_set_y(Turtle *state, float y) {
    TURTLE_LOG("[turtle_set_y]: Setting Y position to %.2f", y);
    
    if (state->turtle->penDown) {
        TURTLE_LOG("[turtle_set_y]: Pen is down. Drawing line from (%.2f, %.2f) to (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, state->turtle->position.x, y);
        resize_lines_if_needed(state);

        state->lines[state->lineCount].start = state->turtle->position;
        state->lines[state->lineCount].end = (Vector2){state->turtle->position.x, y};
        state->lines[state->lineCount].color = state->turtle->pencolor;
        state->lines[state->lineCount].thickness = state->turtle->thickness;
        state->lineCount++;
    }
    state->turtle->position.y = y;

    if (state->turtle->filling) {
        TURTLE_LOG("[turtle_set_y]: Adding fill point at (%.2f, %.2f)", state->turtle->position.x, y);
        resize_fill_points_if_needed(state);
        state->fillPoints[state->fillPointCount++] = state->turtle->position;
    }
}

void turtle_set_heading(Turtle *state, float to_angle) {
    TURTLE_LOG("[turtle_set_heading]: Setting heading to %.2f degrees", to_angle);
    state->turtle->heading = to_angle;
}

void turtle_home(Turtle *state) {
    float currentX = state->turtle->position.x;
    float currentY = state->turtle->position.y;
    float distance = sqrtf(currentX * currentX + currentY * currentY);
    float headingToHome = atan2f(-currentY, -currentX) * (180.0f / PI);

    TURTLE_LOG("[turtle_home]: Heading towards home with angle %.2f and distance %.2f", headingToHome, distance);

    turtle_set_heading(state, headingToHome);
    turtle_forward(state, distance);

    TURTLE_LOG("[turtle_home]: Reached home. Setting heading to 0.");
    turtle_set_heading(state, 0.0f);
}

void turtle_circle(Turtle *state, float radius, float extent, int steps) {
    TURTLE_LOG("[turtle_circle]: Drawing circle with radius %.2f, extent %.2f, steps %d", radius, extent, steps);

    if (steps == 0) {
        steps = (int)fabs(radius) * 3.14 / 4;  // Automatically calculate steps if not given
        TURTLE_LOG("[turtle_circle]: Calculated steps as %d", steps);
    }

    float angle_step = extent / steps;
    float length_step = (2 * 3.14 * fabs(radius) * (extent / 360)) / steps;
    Vector2 startPosition = state->turtle->position;

    TURTLE_LOG("[turtle_circle]: Starting circle at position (%.2f, %.2f), angle step %.2f, length step %.2f", startPosition.x, startPosition.y, angle_step, length_step);

    for (int i = 0; i < steps; i++) {
        turtle_forward(state, length_step);
        turtle_left(state, radius > 0 ? angle_step : -angle_step);
    }

    if (state->isFilling) {
        TURTLE_LOG("[turtle_circle]: Filling the circle sector.");
        DrawCircleSector(startPosition, fabs(radius), 0, 360, 36, state->fillColor);
    }
    TURTLE_LOG("[turtle_circle]: Circle drawing completed.");
}

void turtle_dot(Turtle *state, float size, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    TURTLE_LOG("[turtle_dot]: Drawing dot of size %.2f with color (R: %d, G: %d, B: %d, A: %d)", size, r, g, b, a);
    
    state->dot.size = size;
    state->dot.color = (Color){r, g, b, a};
    state->dot.drawn = true;

    TURTLE_LOG("[turtle_dot]: Dot drawn.");
}

int turtle_stamp(Turtle *state) {
    TURTLE_LOG("[turtle_stamp]: Stamping at position (%.2f, %.2f) with color (R: %d, G: %d, B: %d, A: %d)", state->turtle->position.x, state->turtle->position.y, state->turtle->pencolor.r, state->turtle->pencolor.g, state->turtle->pencolor.b, state->turtle->pencolor.a);
    resize_stamps_if_needed(state);
    
    state->stamps[state->stampCount].position = state->turtle->position;
    state->stamps[state->stampCount].color = state->turtle->pencolor;
    state->stamps[state->stampCount].id = state->nextStampID;
    state->stampCount++;

    TURTLE_LOG("[turtle_stamp]: Stamp created with ID %d", state->nextStampID);
    return state->nextStampID++;
}

void turtle_clear_stamp(Turtle *state, int stamp_id) {
    TURTLE_LOG("[turtle_clear_stamp]: Clearing stamp with ID %d", stamp_id);
    for (int i = 0; i < state->stampCount; i++) {
        if (state->stamps[i].id == stamp_id) {
            TURTLE_LOG("[turtle_clear_stamp]: Stamp ID %d found. Removing it.", stamp_id);
            for (int j = i; j < state->stampCount - 1; j++) {
                state->stamps[j] = state->stamps[j + 1];
            }
            state->stampCount--;
            TURTLE_LOG("[turtle_clear_stamp]: Stamp removed. Remaining stamp count: %d", state->stampCount);

            break;
        }
    }
}

float turtle_distance(Turtle *state, float x, float y) {
    TURTLE_LOG("[turtle_distance]: Calculating distance from (%.2f, %.2f) to (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, x, y);
    float distance = sqrtf((state->turtle->position.x - x) * (state->turtle->position.x - x) + (state->turtle->position.y - y) * (state->turtle->position.y - y));
    TURTLE_LOG("[turtle_distance]: Distance calculated: %.2f", distance);

    return distance;
}

float turtle_distance_turtle(Turtle *state, Tu *other_turtle) {
    TURTLE_LOG("[turtle_distance_turtle]: Calculating distance between turtles at (%.2f, %.2f) and (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, other_turtle->position.x, other_turtle->position.y);
    float distance = sqrtf((state->turtle->position.x - other_turtle->position.x) * (state->turtle->position.x - other_turtle->position.x) + (state->turtle->position.y - other_turtle->position.y) * (state->turtle->position.y - other_turtle->position.y));
    TURTLE_LOG("[turtle_distance_turtle]: Distance calculated: %.2f", distance);

    return distance;
}

void turtle_degrees(Turtle *state, float fullcircle) {
    TURTLE_LOG("[turtle_degrees]: Setting full circle degrees to %.2f", fullcircle);
    state->fullcircle_degrees = fullcircle;
}

void turtle_radians(Turtle *state) {
    TURTLE_LOG("[turtle_radians]: Setting full circle to 2 * PI (radians mode)");
    state->fullcircle_degrees = 2 * PI;
}

float turtle_heading(Turtle *state) {
    float heading = fmodf(state->turtle->heading * (state->fullcircle_degrees / 360.0f), state->fullcircle_degrees);
    TURTLE_LOG("[turtle_heading]: Current heading is %.2f degrees", heading);

    return heading;
}

void turtle_set_background_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    TURTLE_LOG("[turtle_set_background_color]: Setting background color to (R: %d, G: %d, B: %d, A: %d)", r, g, b, a);
    state->backgroundColor = (Color){r, g, b, a};
}
