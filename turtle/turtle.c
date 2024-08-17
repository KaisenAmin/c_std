// #include "turtle.h"
// #include <math.h>
// #include <stdio.h>
// #include <stdlib.h>

// #define INITIAL_CAPACITY 1024

// static Color get_color_from_hex(const char *hex) {
//     unsigned int r, g, b;
//     if (sscanf(hex, "#%02x%02x%02x", &r, &g, &b) == 3) {
//         return (Color){r, g, b, 255};
//     }
//     return BLACK; // Default color if parsing fails
// }

// Turtle *turtle_create(void) {
//     Turtle *state = (Turtle *)malloc(sizeof(Turtle));
//     if (!state) {
//         fprintf(stderr, "Failed to allocate memory for turtle state\n");
//         exit(EXIT_FAILURE);
//     }

//     state->turtle = (Tu *)malloc(sizeof(Tu));
//     if (!state->turtle) {
//         fprintf(stderr, "Failed to allocate memory for turtle\n");
//         exit(EXIT_FAILURE);
//     }

//     state->turtle->position = (Vector2){400.0f, 300.0f};
//     state->turtle->heading = 0.0f;
//     state->turtle->pencolor = BLACK;
//     state->turtle->fillcolor = BLACK;
//     state->turtle->penDown = true;
//     state->turtle->filling = false;
//     state->turtle->speed = 1.0f;
//     state->turtle->thickness = 1.0f; // Initialize thickness

//     state->lineCount = 0;
//     state->stampCount = 0;
//     state->nextStampID = 1;
//     state->distanceRemaining = 0;
//     state->stepSize = 1.0f;

//     state->dot.size = 0;
//     state->dot.color = (Color){0, 0, 0, 0};
//     state->dot.drawn = false;
//     state->fullcircle_degrees = 360.0f; // Reset to default when creating turtle
//     state->backgroundColor = WHITE;

//     state->lines = (Line *)malloc(INITIAL_CAPACITY * sizeof(Line));
//     if (!state->lines) {
//         fprintf(stderr, "Failed to allocate memory for lines\n");
//         exit(EXIT_FAILURE);
//     }
//     state->lineCapacity = INITIAL_CAPACITY;

//     state->stamps = (Stamp *)malloc(INITIAL_CAPACITY * sizeof(Stamp));
//     if (!state->stamps) {
//         fprintf(stderr, "Failed to allocate memory for stamps\n");
//         exit(EXIT_FAILURE);
//     }
//     state->stampCapacity = INITIAL_CAPACITY;

//     return state;
// }

// void turtle_deallocate(Turtle *state) {
//     if (state) {
//         free(state->turtle);
//         free(state->lines);
//         free(state->stamps);
//         free(state);
//     }
// }

// void turtle_close_window(void) {
//     CloseWindow();
// }

// void turtle_init_window(int width, int height, const char *title) {
//     InitWindow(width, height, title);
// }

// void turtle_set_fps(int fps) {
//     SetTargetFPS(fps);
// }

// void turtle_set_speed(Turtle *state, float speed) {
//     state->turtle->speed = speed;
//     state->stepSize = speed * 5;
// }

// void turtle_pen_size(Turtle *state, float width) {
//     state->turtle->thickness = width;
// }

// float turtle_get_pen_size(Turtle *state) {
//     return state->turtle->thickness;
// }

// bool turtle_is_down(Turtle *state) {
//     return state->turtle->penDown;
// }

// static void resize_lines_if_needed(Turtle *state) {
//     if (state->lineCount >= state->lineCapacity) {
//         state->lineCapacity *= 2;
//         state->lines = (Line *)realloc(state->lines, state->lineCapacity * sizeof(Line));

//         if (!state->lines) {
//             fprintf(stderr, "Failed to reallocate memory for lines\n");
//             exit(EXIT_FAILURE);
//         }
//     }
// }

// static void resize_stamps_if_needed(Turtle *state) {
//     if (state->stampCount >= state->stampCapacity) {
//         state->stampCapacity *= 2;
//         state->stamps = (Stamp *)realloc(state->stamps, state->stampCapacity * sizeof(Stamp));

//         if (!state->stamps) {
//             fprintf(stderr, "Failed to reallocate memory for stamps\n");
//             exit(EXIT_FAILURE);
//         }
//     }
// }

// void turtle_forward(Turtle *state, float distance) {
//     state->distanceRemaining = distance;
//     float rad = DEG2RAD * state->turtle->heading;

//     while (state->distanceRemaining > 0) {
//         float move = fminf(state->stepSize, state->distanceRemaining);  // Use a smaller step size for smoother movement
//         Vector2 start = state->turtle->position;

//         state->turtle->position.x += move * cosf(rad);
//         state->turtle->position.y += move * sinf(rad);

//         if (state->turtle->penDown) {
//             resize_lines_if_needed(state);
//             state->lines[state->lineCount].start = start;
//             state->lines[state->lineCount].end = state->turtle->position;
//             state->lines[state->lineCount].color = state->turtle->pencolor;
//             state->lines[state->lineCount].thickness = state->turtle->thickness;
//             state->lineCount++;
//         }

//         state->distanceRemaining -= move;

//         BeginDrawing();
//         ClearBackground(state->backgroundColor);
//         for (int i = 0; i < state->lineCount; i++) {
//             DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
//         }
//         for (int i = 0; i < state->stampCount; i++) {
//             DrawCircleV(state->stamps[i].position, 5, state->stamps[i].color);
//         }
//         DrawCircleV(state->turtle->position, 5, state->turtle->pencolor);
//         if (state->dot.drawn) {
//             DrawCircleV(state->turtle->position, state->dot.size / 2, state->dot.color);
//         }
//         EndDrawing();

//         if (state->distanceRemaining > 0) {
//             WaitTime(0.005f);  // Adjust wait time for smoother animation
//         }
//     }
// }

// void turtle_position(Turtle *state, float *x, float *y) {
//     *x = state->turtle->position.x;
//     *y = state->turtle->position.y;
// }

// void turtle_turn(Turtle *state, float angle) {
//     state->turtle->heading += angle;
// }

// void turtle_right(Turtle *state, float angle) {
//     state->turtle->heading -= angle;
// }

// void turtle_left(Turtle *state, float angle) {
//     state->turtle->heading += angle;
// }

// void turtle_pen_up(Turtle *state) {
//     state->turtle->penDown = false;
// }

// void turtle_pen_down(Turtle *state) {
//     state->turtle->penDown = true;
// }

// void turtle_set_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
//     state->turtle->pencolor = (Color){r, g, b, a};
// }

// void turtle_color(Turtle *state, const char *pencolor_str, const char *fillcolor_str) {
//     state->turtle->pencolor = get_color_from_hex(pencolor_str);
//     state->turtle->fillcolor = get_color_from_hex(fillcolor_str);
// }

// void turtle_color_rgb(Turtle *state, unsigned char pr, unsigned char pg, unsigned char pb, unsigned char fr, unsigned char fg, unsigned char fb) {
//     state->turtle->pencolor = (Color){pr, pg, pb, 255};
//     state->turtle->fillcolor = (Color){fr, fg, fb, 255};
// }

// bool turtle_filling(Turtle *state) {
//     return state->turtle->filling;
// }

// void turtle_begin_fill(Turtle *state) {
//     state->isFilling = true;
//     state->fillColor = state->turtle->fillcolor; // Store the fill color
// }

// void turtle_end_fill(Turtle *state) {
//     state->isFilling = false;
// }

// void turtle_draw(Turtle *state) {
//     BeginDrawing();
//     ClearBackground(state->backgroundColor);

//     for (int i = 0; i < state->lineCount; i++) {
//         DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
//     }
//     for (int i = 0; i < state->stampCount; i++) {
//         DrawCircleV(state->stamps[i].position, 5, state->stamps[i].color);
//     }
//     DrawCircleV(state->turtle->position, 5, state->turtle->pencolor);
//     if (state->dot.drawn) {
//         DrawCircleV(state->turtle->position, state->dot.size / 2, state->dot.color);
//     }

//     // Ensure filled shapes are redrawn
//     if (state->isFilling) {
//         DrawCircleSector(state->turtle->position, 50, 0, 360, 36, state->fillColor);
//     }

//     EndDrawing();
// }

// void turtle_begin_drawing(void) {
//     BeginDrawing();
// }

// void turtle_clear_background(Color color) {
//     ClearBackground(color);
// }

// void turtle_end_drawing(void) {
//     EndDrawing();
// }

// void turtle_done(Turtle *state, TurtleDrawFunc user_draw) {
//     while (!WindowShouldClose()) {
//         BeginDrawing();
//         ClearBackground(state->backgroundColor); 

//         // Draw existing lines and stamps
//         for (int i = 0; i < state->lineCount; i++) {
//             DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
//         }
//         for (int i = 0; i < state->stampCount; i++) {
//             DrawCircleV(state->stamps[i].position, 5, state->stamps[i].color);
//         }

//         // Execute user draw function
//         user_draw(state);

//         // Draw the current turtle position
//         DrawCircleV(state->turtle->position, 5, state->turtle->pencolor);

//         // Draw dot if set
//         if (state->dot.drawn) {
//             DrawCircleV(state->turtle->position, state->dot.size / 2, state->dot.color);
//         }

//         EndDrawing();
//     }
//     turtle_close_window();
// }


// void turtle_set_position(Turtle *state, float x, float y) {
//     if (state->turtle->penDown) {
//         resize_lines_if_needed(state);
//         state->lines[state->lineCount].start = state->turtle->position;
//         state->lines[state->lineCount].end = (Vector2){x, y};
//         state->lines[state->lineCount].color = state->turtle->pencolor;
//         state->lines[state->lineCount].thickness = state->turtle->thickness;
//         state->lineCount++;
//     }
//     state->turtle->position = (Vector2){x, y};
// }

// void turtle_set_x(Turtle *state, float x) {
//     if (state->turtle->penDown) {
//         resize_lines_if_needed(state);
//         state->lines[state->lineCount].start = state->turtle->position;
//         state->lines[state->lineCount].end = (Vector2){x, state->turtle->position.y};
//         state->lines[state->lineCount].color = state->turtle->pencolor;
//         state->lines[state->lineCount].thickness = state->turtle->thickness;
//         state->lineCount++;
//     }
//     state->turtle->position.x = x;
// }

// void turtle_set_y(Turtle *state, float y) {
//     if (state->turtle->penDown) {
//         resize_lines_if_needed(state);
//         state->lines[state->lineCount].start = state->turtle->position;
//         state->lines[state->lineCount].end = (Vector2){state->turtle->position.x, y};
//         state->lines[state->lineCount].color = state->turtle->pencolor;
//         state->lines[state->lineCount].thickness = state->turtle->thickness;
//         state->lineCount++;
//     }
//     state->turtle->position.y = y;
// }

// void turtle_set_heading(Turtle *state, float to_angle) {
//     state->turtle->heading = to_angle;
// }

// void turtle_home(Turtle *state) {
//     float currentX = state->turtle->position.x;
//     float currentY = state->turtle->position.y;
//     float distance = sqrtf(currentX * currentX + currentY * currentY);
//     float headingToHome = atan2f(-currentY, -currentX) * (180.0f / PI);

//     // Set heading towards home and move forward in smaller steps
//     turtle_set_heading(state, headingToHome);
//     turtle_forward(state, distance);

//     // Set the heading back to 0 degrees at home
//     turtle_set_heading(state, 0.0f);
// }

// void turtle_circle(Turtle *state, float radius, float extent, int steps) {
//     if (steps == 0) {
//         steps = (int)fabs(radius) * 3.14 / 4;  // Automatically calculate steps if not given
//     }

//     float angle_step = extent / steps;
//     float length_step = (2 * 3.14 * fabs(radius) * (extent / 360)) / steps;
//     Vector2 startPosition = state->turtle->position;

//     for (int i = 0; i < steps; i++) {
//         turtle_forward(state, length_step);
//         turtle_left(state, radius > 0 ? angle_step : -angle_step);
//     }

//     if (state->isFilling) {
//         DrawCircleSector(startPosition, fabs(radius), 0, 360, 36, state->fillColor);
//     }
// }

// void turtle_dot(Turtle *state, float size, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
//     state->dot.size = size;
//     state->dot.color = (Color){r, g, b, a};
//     state->dot.drawn = true;
// }

// int turtle_stamp(Turtle *state) {
//     resize_stamps_if_needed(state);
//     state->stamps[state->stampCount].position = state->turtle->position;
//     state->stamps[state->stampCount].color = state->turtle->pencolor;
//     state->stamps[state->stampCount].id = state->nextStampID;
//     state->stampCount++;
//     return state->nextStampID++;
// }

// void turtle_clear_stamp(Turtle *state, int stamp_id) {
//     for (int i = 0; i < state->stampCount; i++) {
//         if (state->stamps[i].id == stamp_id) {
//             for (int j = i; j < state->stampCount - 1; j++) {
//                 state->stamps[j] = state->stamps[j + 1];
//             }
//             state->stampCount--;
//             break;
//         }
//     }
// }

// float turtle_distance(Turtle *state, float x, float y) {
//     return sqrtf((state->turtle->position.x - x) * (state->turtle->position.x - x) + (state->turtle->position.y - y) * (state->turtle->position.y - y));
// }

// float turtle_distance_turtle(Turtle *state, Tu *other_turtle) {
//     return sqrtf((state->turtle->position.x - other_turtle->position.x) * (state->turtle->position.x - other_turtle->position.x) + (state->turtle->position.y - other_turtle->position.y) * (state->turtle->position.y - other_turtle->position.y));
// }

// void turtle_degrees(Turtle *state, float fullcircle) {
//     state->fullcircle_degrees = fullcircle;
// }

// void turtle_radians(Turtle *state) {
//     state->fullcircle_degrees = 2 * PI;
// }

// float turtle_heading(Turtle *state) {
//     return fmodf(state->turtle->heading * (state->fullcircle_degrees / 360.0f), state->fullcircle_degrees);
// }

// void turtle_set_background_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
//     state->backgroundColor = (Color){r, g, b, a};
// }

#include "turtle.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_CAPACITY 1024

static Color get_color_from_hex(const char *hex) {
    unsigned int r, g, b;
    if (sscanf(hex, "#%02x%02x%02x", &r, &g, &b) == 3) {
        return (Color){r, g, b, 255};
    }
    return BLACK; // Default color if parsing fails
}

Turtle *turtle_create(void) {
    Turtle *state = (Turtle *)malloc(sizeof(Turtle));
    if (!state) {
        fprintf(stderr, "Failed to allocate memory for turtle state\n");
        exit(EXIT_FAILURE);
    }

    state->turtle = (Tu *)malloc(sizeof(Tu));
    if (!state->turtle) {
        fprintf(stderr, "Failed to allocate memory for turtle\n");
        exit(EXIT_FAILURE);
    }

    state->turtle->position = (Vector2){400.0f, 300.0f};
    state->turtle->heading = 0.0f;
    state->turtle->pencolor = BLACK;
    state->turtle->fillcolor = BLACK;
    state->turtle->penDown = true;
    state->turtle->filling = false;
    state->turtle->speed = 1.0f;
    state->turtle->thickness = 1.0f; // Initialize thickness

    state->lineCount = 0;
    state->stampCount = 0;
    state->nextStampID = 1;
    state->distanceRemaining = 0;
    state->stepSize = 1.0f;

    state->dot.size = 0;
    state->dot.color = (Color){0, 0, 0, 0};
    state->dot.drawn = false;
    state->fullcircle_degrees = 360.0f; // Reset to default when creating turtle
    state->backgroundColor = WHITE;

    state->lines = (Line *)malloc(INITIAL_CAPACITY * sizeof(Line));
    if (!state->lines) {
        fprintf(stderr, "Failed to allocate memory for lines\n");
        exit(EXIT_FAILURE);
    }
    state->lineCapacity = INITIAL_CAPACITY;

    state->stamps = (Stamp *)malloc(INITIAL_CAPACITY * sizeof(Stamp));
    if (!state->stamps) {
        fprintf(stderr, "Failed to allocate memory for stamps\n");
        exit(EXIT_FAILURE);
    }
    state->stampCapacity = INITIAL_CAPACITY;

    state->fillPoints = (Vector2 *)malloc(INITIAL_CAPACITY * sizeof(Vector2));
    if (!state->fillPoints) {
        fprintf(stderr, "Failed to allocate memory for fill points\n");
        exit(EXIT_FAILURE);
    }
    state->fillPointCapacity = INITIAL_CAPACITY;
    state->fillPointCount = 0;

    return state;
}

void turtle_deallocate(Turtle *state) {
    if (state) {
        free(state->turtle);
        free(state->lines);
        free(state->stamps);
        free(state->fillPoints);
        free(state);
    }
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

void turtle_set_speed(Turtle *state, float speed) {
    state->turtle->speed = speed;
    state->stepSize = speed * 5;
}

void turtle_pen_size(Turtle *state, float width) {
    state->turtle->thickness = width;
}

float turtle_get_pen_size(Turtle *state) {
    return state->turtle->thickness;
}

bool turtle_is_down(Turtle *state) {
    return state->turtle->penDown;
}

static void resize_lines_if_needed(Turtle *state) {
    if (state->lineCount >= state->lineCapacity) {
        state->lineCapacity *= 2;
        state->lines = (Line *)realloc(state->lines, state->lineCapacity * sizeof(Line));

        if (!state->lines) {
            fprintf(stderr, "Failed to reallocate memory for lines\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void resize_stamps_if_needed(Turtle *state) {
    if (state->stampCount >= state->stampCapacity) {
        state->stampCapacity *= 2;
        state->stamps = (Stamp *)realloc(state->stamps, state->stampCapacity * sizeof(Stamp));

        if (!state->stamps) {
            fprintf(stderr, "Failed to reallocate memory for stamps\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void resize_fill_points_if_needed(Turtle *state) {
    if (state->fillPointCount >= state->fillPointCapacity) {
        state->fillPointCapacity *= 2;
        state->fillPoints = (Vector2 *)realloc(state->fillPoints, state->fillPointCapacity * sizeof(Vector2));

        if (!state->fillPoints) {
            fprintf(stderr, "Failed to reallocate memory for fill points\n");
            exit(EXIT_FAILURE);
        }
    }
}

void turtle_forward(Turtle *state, float distance) {
    state->distanceRemaining = distance;
    float rad = DEG2RAD * state->turtle->heading;

    while (state->distanceRemaining > 0) {
        float move = fminf(state->stepSize, state->distanceRemaining);  // Use a smaller step size for smoother movement
        Vector2 start = state->turtle->position;

        state->turtle->position.x += move * cosf(rad);
        state->turtle->position.y += move * sinf(rad);

        if (state->turtle->penDown) {
            resize_lines_if_needed(state);
            state->lines[state->lineCount].start = start;
            state->lines[state->lineCount].end = state->turtle->position;
            state->lines[state->lineCount].color = state->turtle->pencolor;
            state->lines[state->lineCount].thickness = state->turtle->thickness;
            state->lineCount++;
        }

        if (state->turtle->filling) {
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
    *x = state->turtle->position.x;
    *y = state->turtle->position.y;
}

void turtle_turn(Turtle *state, float angle) {
    state->turtle->heading += angle;
}

void turtle_right(Turtle *state, float angle) {
    state->turtle->heading -= angle;
}

void turtle_left(Turtle *state, float angle) {
    state->turtle->heading += angle;
}

void turtle_pen_up(Turtle *state) {
    state->turtle->penDown = false;
}

void turtle_pen_down(Turtle *state) {
    state->turtle->penDown = true;
}

void turtle_set_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    state->turtle->pencolor = (Color){r, g, b, a};
}

void turtle_color(Turtle *state, const char *pencolor_str, const char *fillcolor_str) {
    state->turtle->pencolor = get_color_from_hex(pencolor_str);
    state->turtle->fillcolor = get_color_from_hex(fillcolor_str);
}

void turtle_color_rgb(Turtle *state, unsigned char pr, unsigned char pg, unsigned char pb, unsigned char fr, unsigned char fg, unsigned char fb) {
    state->turtle->pencolor = (Color){pr, pg, pb, 255};
    state->turtle->fillcolor = (Color){fr, fg, fb, 255};
}

bool turtle_filling(Turtle *state) {
    return state->turtle->filling;
}

void turtle_begin_fill(Turtle *state) {
    state->turtle->filling = true;
    state->fillPointCount = 0; // Reset fill point count
    resize_fill_points_if_needed(state);
    state->fillPoints[state->fillPointCount++] = state->turtle->position;
}

void turtle_end_fill(Turtle *state) {
    
    if (state->fillPointCount > 2) {
        printf("in condition\n");
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
    BeginDrawing();
    ClearBackground(state->backgroundColor);

    if (state->fillPointCount > 2) {
        DrawTriangleFan(state->fillPoints, state->fillPointCount, state->turtle->fillcolor);
    }

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

void turtle_done(Turtle *state, TurtleDrawFunc user_draw) {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(state->backgroundColor); 

        // Draw existing lines and stamps
        if (state->fillPointCount > 2) {
            DrawTriangleFan(state->fillPoints, state->fillPointCount, state->turtle->fillcolor);
        }

        for (int i = 0; i < state->lineCount; i++) {
            DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
        }
        for (int i = 0; i < state->stampCount; i++) {
            DrawCircleV(state->stamps[i].position, 5, state->stamps[i].color);
        }

        // Execute user draw function
        user_draw(state);

        // Draw the current turtle position
        DrawCircleV(state->turtle->position, 5, state->turtle->pencolor);

        // Draw dot if set
        if (state->dot.drawn) {
            DrawCircleV(state->turtle->position, state->dot.size / 2, state->dot.color);
        }

        EndDrawing();
    }
    turtle_close_window();
}

void turtle_set_position(Turtle *state, float x, float y) {
    if (state->turtle->penDown) {
        resize_lines_if_needed(state);
        state->lines[state->lineCount].start = state->turtle->position;
        state->lines[state->lineCount].end = (Vector2){x, y};
        state->lines[state->lineCount].color = state->turtle->pencolor;
        state->lines[state->lineCount].thickness = state->turtle->thickness;
        state->lineCount++;
    }
    state->turtle->position = (Vector2){x, y};

    if (state->turtle->filling) {
        resize_fill_points_if_needed(state);
        state->fillPoints[state->fillPointCount++] = state->turtle->position;
    }
}

void turtle_set_x(Turtle *state, float x) {
    if (state->turtle->penDown) {
        resize_lines_if_needed(state);
        state->lines[state->lineCount].start = state->turtle->position;
        state->lines[state->lineCount].end = (Vector2){x, state->turtle->position.y};
        state->lines[state->lineCount].color = state->turtle->pencolor;
        state->lines[state->lineCount].thickness = state->turtle->thickness;
        state->lineCount++;
    }
    state->turtle->position.x = x;

    if (state->turtle->filling) {
        resize_fill_points_if_needed(state);
        state->fillPoints[state->fillPointCount++] = state->turtle->position;
    }
}

void turtle_set_y(Turtle *state, float y) {
    if (state->turtle->penDown) {
        resize_lines_if_needed(state);
        state->lines[state->lineCount].start = state->turtle->position;
        state->lines[state->lineCount].end = (Vector2){state->turtle->position.x, y};
        state->lines[state->lineCount].color = state->turtle->pencolor;
        state->lines[state->lineCount].thickness = state->turtle->thickness;
        state->lineCount++;
    }
    state->turtle->position.y = y;

    if (state->turtle->filling) {
        resize_fill_points_if_needed(state);
        state->fillPoints[state->fillPointCount++] = state->turtle->position;
    }
}

void turtle_set_heading(Turtle *state, float to_angle) {
    state->turtle->heading = to_angle;
}

void turtle_home(Turtle *state) {
    float currentX = state->turtle->position.x;
    float currentY = state->turtle->position.y;
    float distance = sqrtf(currentX * currentX + currentY * currentY);
    float headingToHome = atan2f(-currentY, -currentX) * (180.0f / PI);

    // Set heading towards home and move forward in smaller steps
    turtle_set_heading(state, headingToHome);
    turtle_forward(state, distance);

    // Set the heading back to 0 degrees at home
    turtle_set_heading(state, 0.0f);
}

void turtle_circle(Turtle *state, float radius, float extent, int steps) {
    if (steps == 0) {
        steps = (int)fabs(radius) * 3.14 / 4;  // Automatically calculate steps if not given
    }

    float angle_step = extent / steps;
    float length_step = (2 * 3.14 * fabs(radius) * (extent / 360)) / steps;
    Vector2 startPosition = state->turtle->position;

    for (int i = 0; i < steps; i++) {
        turtle_forward(state, length_step);
        turtle_left(state, radius > 0 ? angle_step : -angle_step);
    }

    if (state->isFilling) {
        DrawCircleSector(startPosition, fabs(radius), 0, 360, 36, state->fillColor);
    }
}

void turtle_dot(Turtle *state, float size, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    state->dot.size = size;
    state->dot.color = (Color){r, g, b, a};
    state->dot.drawn = true;
}

int turtle_stamp(Turtle *state) {
    resize_stamps_if_needed(state);
    state->stamps[state->stampCount].position = state->turtle->position;
    state->stamps[state->stampCount].color = state->turtle->pencolor;
    state->stamps[state->stampCount].id = state->nextStampID;
    state->stampCount++;
    return state->nextStampID++;
}

void turtle_clear_stamp(Turtle *state, int stamp_id) {
    for (int i = 0; i < state->stampCount; i++) {
        if (state->stamps[i].id == stamp_id) {
            for (int j = i; j < state->stampCount - 1; j++) {
                state->stamps[j] = state->stamps[j + 1];
            }
            state->stampCount--;
            break;
        }
    }
}

float turtle_distance(Turtle *state, float x, float y) {
    return sqrtf((state->turtle->position.x - x) * (state->turtle->position.x - x) + (state->turtle->position.y - y) * (state->turtle->position.y - y));
}

float turtle_distance_turtle(Turtle *state, Tu *other_turtle) {
    return sqrtf((state->turtle->position.x - other_turtle->position.x) * (state->turtle->position.x - other_turtle->position.x) + (state->turtle->position.y - other_turtle->position.y) * (state->turtle->position.y - other_turtle->position.y));
}

void turtle_degrees(Turtle *state, float fullcircle) {
    state->fullcircle_degrees = fullcircle;
}

void turtle_radians(Turtle *state) {
    state->fullcircle_degrees = 2 * PI;
}

float turtle_heading(Turtle *state) {
    return fmodf(state->turtle->heading * (state->fullcircle_degrees / 360.0f), state->fullcircle_degrees);
}

void turtle_set_background_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    state->backgroundColor = (Color){r, g, b, a};
}
