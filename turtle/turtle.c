/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Turtle
*/

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include "turtle.h"
#if defined(_WIN32)
#include <wchar.h>
/* MultiByteToWideChar without <windows.h>, whose names (Rectangle,
   CloseWindow, DrawText, ...) clash with raylib's. */
__declspec(dllimport) int __stdcall MultiByteToWideChar(unsigned int, unsigned long, const char*, int, wchar_t*, int);
#endif

/* fopen for a UTF-8 path (narrow fopen on Windows reads the path in the
   ANSI code page, so non-ASCII names only worked with a UTF-8 code page). */
static FILE* turtle_fopen_utf8(const char* path, const char* mode) {
#if defined(_WIN32)
    wchar_t wmode[8];
    size_t i = 0;
    for (; mode[i] && i < 7; ++i) {
        wmode[i] = (wchar_t)(unsigned char)mode[i];
    }
    wmode[i] = L'\0';
    int n = MultiByteToWideChar(65001 /* CP_UTF8 */, 0x8 /* MB_ERR_INVALID_CHARS */, path, -1, NULL, 0);
    if (n <= 0) {
        return NULL;
    }
    wchar_t* wpath = (wchar_t*)malloc((size_t)n * sizeof(wchar_t));
    if (!wpath) {
        return NULL;
    }
    FILE* f = NULL;
    if (MultiByteToWideChar(65001, 0x8, path, -1, wpath, n) > 0) {
        f = _wfopen(wpath, wmode);
    }
    free(wpath);
    return f;
#else
    return fopen(path, mode);
#endif
}


#define INITIAL_CAPACITY 1024

static void turtle_free_fills(Turtle *state);

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


/**
 * @brief Creates a new turtle state.
 * 
 * This function allocates memory for a new Turtle instance, including its associated state, such as
 * position, heading, colors, speed, and more. Additionally, it initializes the internal lines and stamps 
 * structures to hold turtle drawings.
 * 
 * @return A pointer to the newly created Turtle state. Exits the program if memory allocation fails.
 */
/* Angles given to and returned by the API are in the current unit
   (turtle_degrees / turtle_radians); the heading is stored in degrees. Only
   turtle_heading used to convert, so turtle_left(t, PI/2) in radians mode
   turned 1.57 degrees. */
static float turtle_to_degrees(const Turtle* state, float angle) {
    float full = state->fullcircle_degrees;
    return (full != 0.0f) ? angle * (360.0f / full) : angle;
}

static float turtle_from_degrees(const Turtle* state, float degrees) {
    return degrees * (state->fullcircle_degrees / 360.0f);
}

/* Bearing from the turtle to (x, y) in degrees, [0, 360), counterclockwise
   on screen like the heading (screen y grows downwards). */
static float turtle_bearing_degrees(const Turtle* state, float x, float y) {
    float dx = x - state->turtle->position.x;
    float dy = y - state->turtle->position.y;
    float deg = atan2f(-dy, dx) * (180.0f / PI);
    if (deg < 0.0f) {
        deg += 360.0f;
    }
    /* -0 (a point straight right, dy == 0) prints as "-0.0"; a tiny negative
       angle can round up to 360 */
    if (deg == 0.0f || deg >= 360.0f) {
        deg = 0.0f;
    }
    return deg;
}

Turtle *turtle_create(void) {
    TURTLE_LOG("[turtle_create]: Creating new turtle.");
    Turtle *state = (Turtle *)calloc(1, sizeof(Turtle));
    if (!state) {
        TURTLE_LOG("Error: Failed to allocate memory for turtle state");
        // Project convention: libraries return NULL on OOM instead of exit().
        return NULL;
    }

    state->turtle = (Tu *)malloc(sizeof(Tu));
    if (!state->turtle) {
        TURTLE_LOG("Error: Failed to allocate memory for turtle");
        free(state);
        return NULL;
    }

    state->turtle->position = (Vector2){400.0f, 300.0f};
    state->homePosition = state->turtle->position;
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

    // Regression: turtle_circle reads isFilling and fillColor; these were
    // previously uninitialized (undefined behavior on every circle call).
    state->isFilling = false;
    state->fillColor = (Color){0, 0, 0, 0};

    state->lines = (Line *)malloc(INITIAL_CAPACITY * sizeof(Line));
    if (!state->lines) {
        TURTLE_LOG("Error: Failed to allocate memory for lines");
        free(state->turtle);
        free(state);
        return NULL;
    }
    state->lineCapacity = INITIAL_CAPACITY;

    state->stamps = (Stamp *)malloc(INITIAL_CAPACITY * sizeof(Stamp));
    if (!state->stamps) {
        TURTLE_LOG("Error: Failed to allocate memory for stamps");
        free(state->lines);
        free(state->turtle);
        free(state);
        return NULL;
    }
    state->stampCapacity = INITIAL_CAPACITY;

    state->fillPoints = (Vector2 *)malloc(INITIAL_CAPACITY * sizeof(Vector2));
    if (!state->fillPoints) {
        TURTLE_LOG("Error: Failed to allocate memory for fill points");
        free(state->stamps);
        free(state->lines);
        free(state->turtle);
        free(state);
        return NULL;
    }
    state->fillPointCapacity = INITIAL_CAPACITY;
    state->fillPointCount = 0;

    TURTLE_LOG("[turtle_create]: Turtle created successfully.");
    return state;
}


/**
 * @brief Deallocates the turtle and its associated resources.
 * 
 * This function frees all memory allocated for the turtle state, including the turtle itself, 
 * its lines, stamps, and fill points.
 * 
 * @param state A pointer to the Turtle structure to be deallocated. Does nothing if `state` is NULL.
 */
void turtle_deallocate(Turtle *state) {
    TURTLE_LOG("[turtle_deallocate]: Deallocating turtle.");
    if (state) {
        free(state->turtle);
        free(state->lines);
        free(state->stamps);
        free(state->fillPoints);
        turtle_free_fills(state);
        free(state->fills);
        for (int i = 0; i < state->textCount; ++i) {
            free(state->texts[i].text);
        }
        free(state->texts);
        free(state);

        TURTLE_LOG("[turtle_deallocate]: Turtle deallocated successfully.");
    }
}


/**
 * @brief Closes the window used by the turtle graphics.
 * 
 * This function closes the window where turtle drawings are rendered. 
 * It should be called when the turtle graphics are no longer needed.
 */
void turtle_close_window(void) {
    TURTLE_LOG("[turtle_close_window]: Closing window.");
    /* turtle_done already closes the window, and the examples call this
       afterwards as well: raylib's CloseWindow must not run twice. */
    if (IsWindowReady()) {
        CloseWindow();
    }
}


/**
 * @brief Initializes a window for turtle graphics.
 * 
 * This function opens a window with the specified width, height, and title for rendering turtle graphics.
 * 
 * @param width The width of the window in pixels.
 * @param height The height of the window in pixels.
 * @param title The title of the window displayed on the window's title bar.
 */
void turtle_init_window(int width, int height, const char *title) {
    TURTLE_LOG("[turtle_init_window]: Initializing window (%d x %d) with title: %s", width, height, title);
    InitWindow(width, height, title);
}


/**
 * @brief Sets the frames per second (FPS) for the turtle window.
 * 
 * This function defines the target FPS at which the window will render updates, controlling how 
 * fast the graphics will be updated.
 * 
 * @param fps The target frames per second (FPS) to set.
 */
void turtle_set_fps(int fps) {
    TURTLE_LOG("[turtle_set_fps]: Setting FPS to %d", fps);
    SetTargetFPS(fps);
}


/**
 * @brief Sets the movement speed of the turtle.
 * 
 * This function adjusts the turtle's movement speed and recalculates the step size, 
 * affecting how far the turtle moves per frame.
 * 
 * @param state A pointer to the Turtle structure whose speed is to be set.
 * @param speed The desired turtle speed as a float.
 */
void turtle_set_speed(Turtle *state, float speed) {
    if (!state || !state->turtle) {
        return;
    }

    TURTLE_LOG("[turtle_set_speed]: Setting turtle speed to %.2f", speed);
    state->turtle->speed = speed;
    /* A step of 0 (speed 0, as in Python's "fastest") or below made
       turtle_forward loop forever; such a speed now draws instantly. */
    state->stepSize = (speed > 0.0f && isfinite(speed)) ? speed * 5 : 0.0f;
}


/**
 * @brief Sets the thickness of the turtle's pen.
 * 
 * This function adjusts the thickness of the pen used by the turtle to draw lines.
 * 
 * @param state A pointer to the Turtle structure whose pen size is to be set.
 * @param width The thickness of the pen as a float.
 */
void turtle_pen_size(Turtle *state, float width) {
    if (!state || !state->turtle) {
        return;
    }

    TURTLE_LOG("[turtle_pen_size]: Setting pen size to %.2f", width);
    state->turtle->thickness = width;
}


/**
 * @brief Retrieves the current thickness of the turtle's pen.
 * 
 * This function returns the current pen size (thickness) used by the turtle for drawing.
 * 
 * @param state A pointer to the Turtle structure from which to retrieve the pen size.
 * @return The thickness of the pen as a float.
 */
float turtle_get_pen_size(Turtle *state) {
    if (!state || !state->turtle) {
        return 0.0f;
    }

    TURTLE_LOG("[turtle_get_pen_size]: Getting pen size.");
    return state->turtle->thickness;
}


/**
 * @brief Checks if the turtle's pen is currently down.
 * 
 * This function checks the pen state of the turtle and returns true if the pen is down (drawing mode), 
 * and false if the pen is up (not drawing).
 * 
 * @param state A pointer to the Turtle structure whose pen state is being checked.
 * 
 * @return `true` if the pen is down, `false` otherwise.
 */
bool turtle_is_down(Turtle *state) {
    if (!state || !state->turtle) {
        return false;
    }

    TURTLE_LOG("[turtle_is_down]: Checking if pen is down.");
    return state->turtle->penDown;
}


/**
 * @brief Resizes the line buffer if needed.
 * 
 * This function checks if the current number of lines exceeds the allocated capacity. If the capacity 
 * is reached, the buffer is resized to hold more lines.
 * 
 * @param state A pointer to the Turtle structure whose line buffer might be resized.
 */
/* The capacity after doubling @p cap for elements of @p elem bytes, or 0
   when it would overflow int / size_t. */
static int turtle_grown_capacity(int cap, size_t elem) {
    if (cap > INT_MAX / 2) {
        return 0;
    }
    int newCap = cap > 0 ? cap * 2 : INITIAL_CAPACITY;
    return ((size_t)newCap > SIZE_MAX / elem) ? 0 : newCap;
}

/* The resize helpers return false - leaving the array as it was - when the
   allocation fails; callers then skip the write (they used to write one
   element past the buffer). */
static bool resize_lines_if_needed(Turtle *state) {
    if (state->lineCount >= state->lineCapacity) {
        int newCap = turtle_grown_capacity(state->lineCapacity, sizeof(Line));
        Line *resized = newCap ? (Line *)realloc(state->lines, (size_t)newCap * sizeof(Line)) : NULL;

        if (!resized) {
            TURTLE_LOG("Error: Failed to reallocate memory for lines (capacity %d)", newCap);
            return false;
        }

        state->lines = resized;
        state->lineCapacity = newCap;
    }
    return true;
}


/**
 * @brief Resizes the stamp buffer if needed.
 * 
 * This function checks if the current number of stamps exceeds the allocated capacity. If the capacity 
 * is reached, the buffer is resized to hold more stamps.
 * 
 * @param state A pointer to the Turtle structure whose stamp buffer might be resized.
 */
static bool resize_stamps_if_needed(Turtle *state) {
    if (state->stampCount >= state->stampCapacity) {
        int newCap = turtle_grown_capacity(state->stampCapacity, sizeof(Stamp));
        TURTLE_LOG("[resize_stamps_if_needed]: Resizing stamps array. New capacity: %d", newCap);

        Stamp *resized = newCap ? (Stamp *)realloc(state->stamps, (size_t)newCap * sizeof(Stamp)) : NULL;
        if (!resized) {
            TURTLE_LOG("Error: Failed to reallocate memory for stamps (capacity %d)", newCap);
            return false;
        }

        state->stamps = resized;
        state->stampCapacity = newCap;
    }
    return true;
}


/**
 * @brief Resizes the fill points buffer if needed.
 * 
 * This function checks if the current number of fill points exceeds the allocated capacity. If the capacity 
 * is reached, the buffer is resized to hold more points.
 * 
 * @param state A pointer to the Turtle structure whose fill points buffer might be resized.
 */
static bool resize_fill_points_if_needed(Turtle *state) {
    if (state->fillPointCount >= state->fillPointCapacity) {
        int newCap = turtle_grown_capacity(state->fillPointCapacity, sizeof(Vector2));
        TURTLE_LOG("[resize_fill_points_if_needed]: Resizing fill points array. New capacity: %d", newCap);

        Vector2 *resized = newCap ? (Vector2 *)realloc(state->fillPoints, (size_t)newCap * sizeof(Vector2)) : NULL;
        if (!resized) {
            TURTLE_LOG("Error: Failed to reallocate memory for fill points (capacity %d)", newCap);
            return false;
        }

        state->fillPoints = resized;
        state->fillPointCapacity = newCap;
    }
    return true;
}

/* Record a vertex of the fill in progress (skipped when out of memory). */
static void turtle_add_fill_point(Turtle *state) {
    if (state->turtle->filling && resize_fill_points_if_needed(state)) {
        state->fillPoints[state->fillPointCount++] = state->turtle->position;
    }
}

/* Append a pen-down segment; returns its index, or -1 when out of memory. */
static int turtle_add_line(Turtle *state, Vector2 start, Vector2 end) {
    if (!resize_lines_if_needed(state)) {
        return -1;
    }
    Line *l = &state->lines[state->lineCount];
    l->start = start;
    l->end = end;
    l->color = state->turtle->pencolor;
    l->thickness = state->turtle->thickness;
    return state->lineCount++;
}

static void turtle_free_fills(Turtle *state) {
    for (int i = 0; i < state->fillCount; ++i) {
        free(state->fills[i].triangles);
    }
    state->fillCount = 0;
}

static float turtle_cross(Vector2 o, Vector2 a, Vector2 b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

static bool turtle_in_triangle(Vector2 p, Vector2 a, Vector2 b, Vector2 c) {
    float d1 = turtle_cross(a, b, p);
    float d2 = turtle_cross(b, c, p);
    float d3 = turtle_cross(c, a, p);
    bool neg = (d1 < 0.0f) || (d2 < 0.0f) || (d3 < 0.0f);
    bool pos = (d1 > 0.0f) || (d2 > 0.0f) || (d3 > 0.0f);
    return !(neg && pos);
}

/* raylib's DrawTriangle wants counter-clockwise vertices as seen on screen
   (y grows downwards), i.e. a negative cross product here. */
static void turtle_emit_triangle(Vector2* out, int* count, Vector2 a, Vector2 b, Vector2 c) {
    if (turtle_cross(a, b, c) > 0.0f) {
        Vector2 t = b;
        b = c;
        c = t;
    }
    out[3 * *count] = a;
    out[3 * *count + 1] = b;
    out[3 * *count + 2] = c;
    (*count)++;
}

/* Ear-clipping triangulation of the fill outline, so concave shapes fill
   correctly (DrawTriangleFan from the first vertex covered areas outside a
   concave outline). A self-intersecting outline falls back to a fan for the
   part that is left. Returns 3 * *count vertices (malloc'ed) or NULL. */
static Vector2* turtle_triangulate(const Vector2* pts, int n, int* count) {
    *count = 0;
    int* idx = (int*)malloc((size_t)n * sizeof(int));
    if (!idx) {
        return NULL;
    }
    int m = 0;
    for (int i = 0; i < n; ++i) {   /* drop repeated points */
        if (m == 0 || pts[i].x != pts[idx[m - 1]].x || pts[i].y != pts[idx[m - 1]].y) {
            idx[m++] = i;
        }
    }
    while (m > 1 && pts[idx[m - 1]].x == pts[idx[0]].x && pts[idx[m - 1]].y == pts[idx[0]].y) {
        m--;                        /* a closing point equal to the first */
    }
    if (m < 3) {
        free(idx);
        return NULL;
    }
    double area2 = 0.0;
    for (int i = 0; i < m; ++i) {
        Vector2 p = pts[idx[i]], q = pts[idx[(i + 1) % m]];
        area2 += (double)p.x * q.y - (double)q.x * p.y;
    }
    float orient = (area2 > 0.0) ? 1.0f : -1.0f;   /* sign of a convex corner */

    Vector2* tris = (Vector2*)malloc((size_t)3 * (size_t)(m - 2) * sizeof(Vector2));
    if (!tris) {
        free(idx);
        return NULL;
    }
    while (m > 3) {
        bool clipped = false;
        for (int i = 0; i < m && !clipped; ++i) {
            int ip = (i + m - 1) % m, in = (i + 1) % m;
            Vector2 a = pts[idx[ip]], b = pts[idx[i]], c = pts[idx[in]];
            float cr = turtle_cross(a, b, c);
            if (cr == 0.0f) {
                memmove(&idx[i], &idx[i + 1], (size_t)(m - i - 1) * sizeof(int));   /* collinear: no area */
                m--;
                clipped = true;
                break;
            }
            if (cr * orient < 0.0f) {
                continue;                   /* reflex corner */
            }
            bool ear = true;
            for (int j = 0; j < m && ear; ++j) {
                if (j != i && j != ip && j != in && turtle_in_triangle(pts[idx[j]], a, b, c)) {
                    ear = false;
                }
            }
            if (ear) {
                turtle_emit_triangle(tris, count, a, b, c);
                memmove(&idx[i], &idx[i + 1], (size_t)(m - i - 1) * sizeof(int));
                m--;
                clipped = true;
            }
        }
        if (!clipped) {
            break;                          /* self-intersecting: fan the rest */
        }
    }
    for (int i = 1; i + 1 < m; ++i) {
        turtle_emit_triangle(tris, count, pts[idx[0]], pts[idx[i]], pts[idx[i + 1]]);
    }
    free(idx);
    return tris;
}

/* Everything recorded so far: fills (under the lines, each in its own
   colour), lines, stamps and text. Shared by every routine that draws a
   frame, so a fill no longer vanishes during later animation. */
static void turtle_render_scene(const Turtle *state) {
    for (int f = 0; f < state->fillCount; ++f) {
        const TurtleFill *fill = &state->fills[f];
        for (int t = 0; t < fill->triangleCount; ++t) {
            DrawTriangle(fill->triangles[3 * t], fill->triangles[3 * t + 1], fill->triangles[3 * t + 2], fill->color);
        }
    }
    for (int i = 0; i < state->lineCount; i++) {
        DrawLineEx(state->lines[i].start, state->lines[i].end, state->lines[i].thickness, state->lines[i].color);
    }
    for (int i = 0; i < state->stampCount; i++) {
        DrawCircleV(state->stamps[i].position, 5, state->stamps[i].color);
    }
    for (int i = 0; i < state->textCount; ++i) {
        DrawText(state->texts[i].text, (int)state->texts[i].position.x, (int)state->texts[i].position.y,
                 state->texts[i].fontSize, state->texts[i].color);
    }
}

static void turtle_render_cursor(const Turtle *state) {
    DrawCircleV(state->turtle->position, 5, state->turtle->pencolor);
    if (state->dot.drawn) {
        DrawCircleV(state->turtle->position, state->dot.size / 2, state->dot.color);
    }
}


/**
 * @brief Moves the turtle forward by a specified distance.
 *
 * This function moves the turtle forward by the given distance, drawing a line if the pen is down. 
 * It updates the turtle's position based on its heading and step size, and dynamically adjusts the 
 * movement to ensure smooth transitions. If the turtle is in fill mode, it also adds points to the 
 * fill buffer.
 * 
 * Heading 0 points to the right and angles grow counterclockwise on screen
 * (turtle_left turns counterclockwise). A negative distance moves backwards;
 * a speed of 0 or less moves the whole distance at once. The move is
 * animated in steps but recorded as ONE line (it was one line per pixel:
 * forward(100) left 100 segments, every frame redrew all of them, and
 * turtle_undo removed 1/100 of the move).
 *
 * @param state A pointer to the Turtle structure representing the turtle's state.
 * @param distance The distance (in units) by which to move the turtle forward.
 */
void turtle_forward(Turtle *state, float distance) {
    if (!state || !state->turtle || !isfinite(distance)) {
        return;   /* an infinite distance would never finish */
    }
    TURTLE_LOG("[turtle_forward]: Moving turtle forward by %.2f units.", distance);
    /* A negative distance walks backwards (as in Python); it used to be
       ignored. The heading is counterclockwise on screen: y grows downwards
       there, so the y step is -sin (adding +sin turned turtle_left into a
       clockwise turn and mirrored every drawing). */
    float direction = (distance < 0.0f) ? -1.0f : 1.0f;
    state->distanceRemaining = fabsf(distance);
    float rad = DEG2RAD * state->turtle->heading;
    int lineIndex = (state->turtle->penDown && state->distanceRemaining > 0.0f)
                    ? turtle_add_line(state, state->turtle->position, state->turtle->position) : -1;

    while (state->distanceRemaining > 0) {
        float move = (state->stepSize > 0.0f) ? fminf(state->stepSize, state->distanceRemaining)
                                              : state->distanceRemaining;   /* speed <= 0: in one go */
        Vector2 start = state->turtle->position;

        state->turtle->position.x += direction * move * cosf(rad);
        state->turtle->position.y -= direction * move * sinf(rad);

        TURTLE_LOG("[turtle_forward]: Turtle position updated to (%.2f, %.2f).", state->turtle->position.x, state->turtle->position.y);

        (void)start;
        if (lineIndex >= 0) {
            state->lines[lineIndex].end = state->turtle->position;   /* the move's line grows */
        }

        float next = state->distanceRemaining - move;
        /* a step below the float resolution of a huge distance would not
           change it: finish instead of looping forever */
        state->distanceRemaining = (next < state->distanceRemaining) ? next : 0.0f;

        BeginDrawing();
        ClearBackground(state->backgroundColor);
        turtle_render_scene(state);
        turtle_render_cursor(state);
        EndDrawing();

        if (state->distanceRemaining > 0) {
            WaitTime(0.005f);  // Adjust wait time for smoother animation
        }
    }
    if (fabsf(distance) > 0.0f) {
        turtle_add_fill_point(state);   /* one vertex per move */
    }
}


/**
 * @brief Retrieves the current position of the turtle.
 * 
 * This function returns the current x and y coordinates of the turtle's position.
 * 
 * @param state A pointer to the Turtle structure.
 * @param x Pointer to store the x-coordinate of the turtle's position.
 * @param y Pointer to store the y-coordinate of the turtle's position.
 */
void turtle_position(Turtle *state, float *x, float *y) {
    if (!state || !state->turtle) {
        if (x) {
            *x = 0.0f;
        }
        if (y) { 
            *y = 0.0f;
        }
        return;
    }

    TURTLE_LOG("[turtle_position]: Turtle position requested. Current position: (%.2f, %.2f).", state->turtle->position.x, state->turtle->position.y);
    if (x) {
        *x = state->turtle->position.x;
    }
    if (y) { 
        *y = state->turtle->position.y;
    }
}

/**
 * @brief Turns the turtle by a specified angle.
 * 
 * This function adjusts the turtle's heading by the specified angle, in the
 * current angle unit (degrees, or radians after turtle_radians).
 *
 * @param state A pointer to the Turtle structure.
 * @param angle The angle to turn the turtle (positive = counterclockwise).
 */
void turtle_turn(Turtle *state, float angle) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_turn]: Turning turtle by %.2f.", angle);
    state->turtle->heading += turtle_to_degrees(state, angle);
}


/**
 * @brief Turns the turtle to the right by a specified angle.
 * 
 * This function turns the turtle to the right (clockwise) by decreasing the turtle's heading by the specified angle.
 * 
 * @param state A pointer to the Turtle structure.
 * @param angle The angle (in degrees) to turn the turtle right.
 */
void turtle_right(Turtle *state, float angle) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_right]: Turning turtle right by %.2f.", angle);
    state->turtle->heading -= turtle_to_degrees(state, angle);
}


/**
 * @brief Turns the turtle to the left by a specified angle.
 * 
 * This function turns the turtle to the left (counterclockwise) by increasing the turtle's heading by the specified angle.
 * 
 * @param state A pointer to the Turtle structure.
 * @param angle The angle (in degrees) to turn the turtle left.
 */
void turtle_left(Turtle *state, float angle) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_left]: Turning turtle left by %.2f.", angle);
    state->turtle->heading += turtle_to_degrees(state, angle);
}


/**
 * @brief Lifts the turtle's pen up, preventing it from drawing.
 * 
 * This function lifts the pen, preventing the turtle from drawing lines while moving.
 * 
 * @param state A pointer to the Turtle structure.
 */
void turtle_pen_up(Turtle *state) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_pen_up]: Lifting pen up.");
    state->turtle->penDown = false;
}


/**
 * @brief Lowers the turtle's pen down, allowing it to draw.
 * 
 * This function lowers the pen, allowing the turtle to draw lines while moving.
 * 
 * @param state A pointer to the Turtle structure.
 */
void turtle_pen_down(Turtle *state) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_pen_down]: Putting pen down.");
    state->turtle->penDown = true;
}


/**
 * @brief Sets the turtle's pen color using RGBA values.
 * 
 * This function sets the pen color to the specified RGBA values.
 * 
 * @param state A pointer to the Turtle structure.
 * @param r Red value (0-255).
 * @param g Green value (0-255).
 * @param b Blue value (0-255).
 * @param a Alpha (transparency) value (0-255).
 */
void turtle_set_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    if (!state || !state->turtle) { 
        return;
    }
    TURTLE_LOG("[turtle_set_color]: Setting pen color to RGBA(%d, %d, %d, %d).", r, g, b, a);
    state->turtle->pencolor = (Color){r, g, b, a};
}


/**
 * @brief Sets the turtle's pen and fill colors using hexadecimal color strings.
 * 
 * This function sets both the pen and fill colors using hexadecimal string values (e.g., "#FF0000").
 * 
 * @param state A pointer to the Turtle structure.
 * @param pencolor_str Hexadecimal string representing the pen color.
 * @param fillcolor_str Hexadecimal string representing the fill color.
 */
void turtle_color(Turtle *state, const char *pencolor_str, const char *fillcolor_str) {
    if (!state || !state->turtle || !pencolor_str || !fillcolor_str) { 
        return;
    }
    TURTLE_LOG("[turtle_color]: Setting pen color to %s and fill color to %s.", pencolor_str, fillcolor_str);
    
    state->turtle->pencolor = get_color_from_hex(pencolor_str);
    state->turtle->fillcolor = get_color_from_hex(fillcolor_str);
}


/**
 * @brief Sets the turtle's pen and fill colors using RGB values.
 * 
 * This function sets the pen color and the fill color using RGB values.
 * 
 * @param state A pointer to the Turtle structure.
 * @param pr Red value for the pen color (0-255).
 * @param pg Green value for the pen color (0-255).
 * @param pb Blue value for the pen color (0-255).
 * @param fr Red value for the fill color (0-255).
 * @param fg Green value for the fill color (0-255).
 * @param fb Blue value for the fill color (0-255).
 */
void turtle_color_rgb(Turtle *state, unsigned char pr, unsigned char pg, unsigned char pb, unsigned char fr, unsigned char fg, unsigned char fb) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_color_rgb]: Setting pen color to RGB(%d, %d, %d) and fill color to RGB(%d, %d, %d).", pr, pg, pb, fr, fg, fb);

    state->turtle->pencolor = (Color){pr, pg, pb, 255};
    state->turtle->fillcolor = (Color){fr, fg, fb, 255};
}


/**
 * @brief Checks if the turtle is currently filling a shape.
 * 
 * This function returns a boolean value indicating whether the turtle is currently in fill mode.
 * 
 * @param state A pointer to the Turtle structure.
 * 
 * @return `true` if the turtle is filling, `false` otherwise.
 */
bool turtle_filling(Turtle *state) {
    if (!state || !state->turtle) {
        return false;
    }

    TURTLE_LOG("[turtle_filling]: Checking if turtle is filling: %s.", state->turtle->filling ? "true" : "false");
    return state->turtle->filling;
}


/**
 * @brief Starts the fill operation for the turtle.
 * 
 * This function enables the turtle's filling mode and begins tracking the points to be filled.
 * 
 * @param state A pointer to the Turtle structure.
 */
void turtle_begin_fill(Turtle *state) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_begin_fill]: Starting fill.");

    state->turtle->filling = true;
    state->fillPointCount = 0; // a new outline (finished fills are kept in state->fills)
    turtle_add_fill_point(state);
}


/**
 * @brief Ends the fill operation for the turtle.
 * 
 * This function completes the fill operation: the outline collected since turtle_begin_fill is
 * triangulated (concave outlines fill correctly) and kept, in the current fill color, with every
 * earlier fill; all of them are drawn under the lines in every later frame. (Only the last fill
 * used to survive, redrawn in whatever the fill color was at drawing time.)
 * 
 * @param state A pointer to the Turtle structure.
 */
void turtle_end_fill(Turtle *state) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_end_fill]: Ending fill with %d points.", state->fillPointCount);

    if (state->turtle->filling && state->fillPointCount > 2) {
        int triangles = 0;
        Vector2 *tris = turtle_triangulate(state->fillPoints, state->fillPointCount, &triangles);
        bool stored = false;
        if (tris && triangles > 0) {
            if (state->fillCount >= state->fillCapacity) {
                int newCap = turtle_grown_capacity(state->fillCapacity, sizeof(TurtleFill));
                TurtleFill *grown = newCap ? (TurtleFill *)realloc(state->fills, (size_t)newCap * sizeof(TurtleFill)) : NULL;
                if (grown) {
                    state->fills = grown;
                    state->fillCapacity = newCap;
                }
            }
            if (state->fillCount < state->fillCapacity) {
                state->fills[state->fillCount].triangles = tris;
                state->fills[state->fillCount].triangleCount = triangles;
                state->fills[state->fillCount].color = state->turtle->fillcolor;
                state->fillCount++;
                stored = true;
            }
        }
        if (!stored) {
            free(tris);
        }
        TURTLE_LOG("[turtle_end_fill]: Drawing filled shape.");
        BeginDrawing();
        ClearBackground(state->backgroundColor);
        turtle_render_scene(state);
        turtle_render_cursor(state);
        EndDrawing();
    }
    state->turtle->filling = false;
    state->fillPointCount = 0;
}


/**
 * @brief Renders the current state of the turtle and its drawings.
 * 
 * This function draws the turtle, the lines it has drawn, and any filled shapes or stamps. It clears the background
 * before drawing the updated scene.
 * 
 * @param state A pointer to the Turtle structure.
 */
void turtle_draw(Turtle *state) {
    TURTLE_LOG("[turtle_draw]: Begin drawing.");
    if (!state || !state->turtle) {
        return;
    }

    BeginDrawing();
    ClearBackground(state->backgroundColor);
    turtle_render_scene(state);
    turtle_render_cursor(state);
    EndDrawing();
    TURTLE_LOG("[turtle_draw]: End drawing.");
}


/**
 * @brief Begins a drawing operation.
 * 
 * This function begins a new drawing operation, preparing the canvas for rendering shapes or lines.
 */
void turtle_begin_drawing(void) {
    TURTLE_LOG("[turtle_begin_drawing]: Begin drawing.");
    BeginDrawing();
}


/**
 * @brief Clears the screen with the specified background color.
 * 
 * This function fills the background with the specified color.
 * 
 * @param color The color to clear the background with.
 */
void turtle_clear_background(Color color) {
    TURTLE_LOG("[turtle_clear_background]: Clearing background with color (r: %d, g: %d, b: %d, a: %d).", color.r, color.g, color.b, color.a);
    ClearBackground(color);
}


/**
 * @brief Ends a drawing operation.
 * 
 * This function concludes the current drawing operation, finalizing the rendering process.
 */
void turtle_end_drawing(void) {
    TURTLE_LOG("[turtle_end_drawing]: End drawing.");
    EndDrawing();
}


/**
 * @brief Runs the main drawing loop for the turtle and allows for user-defined drawing.
 * 
 * This function enters a loop that continues until the window is closed. It handles drawing the turtle,
 * lines, stamps, and any filled shapes, while also calling a user-provided drawing function.
 * 
 * @param state A pointer to the Turtle structure.
 * @param user_draw A user-defined drawing function that takes a Turtle pointer as a parameter.
 */
void turtle_done(Turtle *state, TurtleDrawFunc user_draw) {
    TURTLE_LOG("[turtle_done]: Begin main draw loop.");
    if (!state || !state->turtle) {
        return;
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(state->backgroundColor);
        turtle_render_scene(state);

        if (user_draw) {   /* NULL is allowed: nothing extra to draw */
            user_draw(state);
        }

        turtle_render_cursor(state);
        EndDrawing();
    }

    TURTLE_LOG("[turtle_done]: Exiting main draw loop and closing window.");
    turtle_close_window();
}

/**
 * @brief Sets the turtle's position to the specified coordinates.
 * 
 * This function moves the turtle to the specified (x, y) position. If the pen is down, a line is drawn from 
 * the current position to the new position. If the turtle is filling, the new position is added to the fill points.
 * 
 * @param state A pointer to the Turtle structure.
 * @param x The new X-coordinate for the turtle's position.
 * @param y The new Y-coordinate for the turtle's position.
 */
void turtle_set_position(Turtle *state, float x, float y) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_set_position]: Setting position to (%.2f, %.2f)", x, y);

    if (state->turtle->penDown) {
        TURTLE_LOG("[turtle_set_position]: Pen is down. Drawing line from (%.2f, %.2f) to (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, x, y);
        turtle_add_line(state, state->turtle->position, (Vector2){x, y});
    }
    state->turtle->position = (Vector2){x, y};
    turtle_add_fill_point(state);
}


/**
 * @brief Sets the turtle's X-coordinate.
 * 
 * This function changes the turtle's X-coordinate while keeping the Y-coordinate the same. If the pen is down,
 * a line is drawn from the current position to the new position. If the turtle is filling, the new position is added to the fill points.
 * 
 * @param state A pointer to the Turtle structure.
 * @param x The new X-coordinate for the turtle's position.
 */
void turtle_set_x(Turtle *state, float x) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_set_x]: Setting X position to %.2f", x);

    if (state->turtle->penDown) {
        TURTLE_LOG("[turtle_set_x]: Pen is down. Drawing line from (%.2f, %.2f) to (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, x, state->turtle->position.y);
        turtle_add_line(state, state->turtle->position, (Vector2){x, state->turtle->position.y});
    }
    state->turtle->position.x = x;
    turtle_add_fill_point(state);
}


/**
 * @brief Sets the turtle's Y-coordinate.
 * 
 * This function changes the turtle's Y-coordinate while keeping the X-coordinate the same. If the pen is down,
 * a line is drawn from the current position to the new position. If the turtle is filling, the new position is added to the fill points.
 * 
 * @param state A pointer to the Turtle structure.
 * @param y The new Y-coordinate for the turtle's position.
 */
void turtle_set_y(Turtle *state, float y) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_set_y]: Setting Y position to %.2f", y);

    if (state->turtle->penDown) {
        TURTLE_LOG("[turtle_set_y]: Pen is down. Drawing line from (%.2f, %.2f) to (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, state->turtle->position.x, y);
        turtle_add_line(state, state->turtle->position, (Vector2){state->turtle->position.x, y});
    }
    state->turtle->position.y = y;
    turtle_add_fill_point(state);
}


/**
 * @brief Sets the turtle's heading to a specified angle.
 * 
 * This function sets the direction in which the turtle is facing to the given angle in degrees.
 * 
 * @param state A pointer to the Turtle structure.
 * @param to_angle The new heading angle for the turtle, in degrees.
 */
void turtle_set_heading(Turtle *state, float to_angle) {
    if (!state || !state->turtle) {
        return;
    }

    TURTLE_LOG("[turtle_set_heading]: Setting heading to %.2f", to_angle);
    state->turtle->heading = turtle_to_degrees(state, to_angle);
}


/**
 * @brief Moves the turtle to its home position and resets its heading.
 *
 * Home is where the turtle started (the centre of the default 800x600
 * window, as set by turtle_create / turtle_reset); it used to be the screen
 * corner (0, 0). A line is drawn on the way when the pen is down.
 * 
 * @param state A pointer to the Turtle structure.
 */
void turtle_home(Turtle *state) {
    if (!state || !state->turtle) {
        return;
    }

    float dx = state->homePosition.x - state->turtle->position.x;
    float dy = state->homePosition.y - state->turtle->position.y;
    float distance = sqrtf(dx * dx + dy * dy);
    float headingToHome = turtle_bearing_degrees(state, state->homePosition.x, state->homePosition.y);

    TURTLE_LOG("[turtle_home]: Heading towards home with angle %.2f and distance %.2f", headingToHome, distance);

    state->turtle->heading = headingToHome;
    turtle_forward(state, distance);

    TURTLE_LOG("[turtle_home]: Reached home. Setting heading to 0.");
    state->turtle->heading = 0.0f;
}


/**
 * @brief Draws a circle or an arc based on the given radius and extent.
 * 
 * This function draws a circle or an arc with the specified radius, extent, and number of steps. If the number of steps is not provided, it is automatically calculated.
 * 
 * @param state A pointer to the Turtle structure.
 * As in Python's turtle the centre lies `radius` units to the left of the
 * turtle; a negative radius puts it on the right (clockwise), and a negative
 * extent draws the arc backwards.
 *
 * @param radius The radius of the circle.
 * @param extent The angular extent of the circle or arc in the current angle unit (360 degrees for a full circle).
 * @param steps The number of steps to divide the circle into (0 = automatic).
 */
void turtle_circle(Turtle *state, float radius, float extent, int steps) {
    if (!state || !state->turtle) {
        TURTLE_LOG("[turtle_circle]: NULL state — bailing out.");
        return;
    }
    TURTLE_LOG("[turtle_circle]: Drawing circle with radius %.2f, extent %.2f, steps %d", radius, extent, steps);
    if (!isfinite(radius) || !isfinite(extent)) {
        return;
    }

    if (steps <= 0) {
        // Auto-calculate steps, guarding against zero (which would have
        // caused division-by-zero in the angle_step / length_step calcs).
        float autoSteps = fabsf(radius) * PI / 4.0f;
        steps = (autoSteps > 3600.0f) ? 3600 : (int)autoSteps;   /* no int overflow for huge radii */
        if (steps < 8) {
            steps = 8;  // floor for small radii
        }
        TURTLE_LOG("[turtle_circle]: Calculated steps as %d", steps);
    }

    /* Python turtle's circle: the centre is `radius` to the LEFT of the
       turtle (right for a negative radius), and a negative extent goes the
       other way round. Each step moves along a chord. A negative extent used
       to draw nothing and only turn the heading. */
    float w = turtle_to_degrees(state, extent) / (float)steps;
    float w2 = 0.5f * w;
    float l = 2.0f * radius * sinf(w2 * (PI / 180.0f));
    if (radius < 0.0f) {
        l = -l;
        w = -w;
        w2 = -w2;
    }
    Vector2 startPosition = state->turtle->position;

    TURTLE_LOG("[turtle_circle]: Starting circle at position (%.2f, %.2f), angle step %.2f, chord %.2f", startPosition.x, startPosition.y, w, l);

    state->turtle->heading += w2;
    for (int i = 0; i < steps; i++) {
        turtle_forward(state, l);
        state->turtle->heading += w;
    }
    state->turtle->heading -= w2;
    (void)startPosition;   /* a circle inside begin/end_fill is filled from its chords */
    TURTLE_LOG("[turtle_circle]: Circle drawing completed.");
}


/**
 * @brief Draws a dot at the turtle's current position.
 * 
 * This function draws a dot of the specified size and color at the turtle's current position.
 * 
 * @param state A pointer to the Turtle structure.
 * @param size The size of the dot to be drawn.
 * @param r The red component of the dot color (0-255).
 * @param g The green component of the dot color (0-255).
 * @param b The blue component of the dot color (0-255).
 * @param a The alpha component of the dot color (0-255).
 */
void turtle_dot(Turtle *state, float size, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    TURTLE_LOG("[turtle_dot]: Drawing dot of size %.2f with color (R: %d, G: %d, B: %d, A: %d)", size, r, g, b, a);

    if (!state) {
        TURTLE_LOG("[turtle_dot]: NULL state, no-op.");
        return;
    }

    state->dot.size = size;
    state->dot.color = (Color){r, g, b, a};
    state->dot.drawn = true;

    TURTLE_LOG("[turtle_dot]: Dot drawn.");
}


/**
 * @brief Stamps the turtle's current position and color.
 * 
 * This function stamps the turtle's current position and pencolor onto the screen. Each stamp is assigned a unique ID.
 * 
 * @param state A pointer to the Turtle structure.
 * @return int The unique ID of the created stamp.
 */
int turtle_stamp(Turtle *state) {
    if (!state || !state->turtle) return 0;
    TURTLE_LOG("[turtle_stamp]: Stamping at position (%.2f, %.2f) with color (R: %d, G: %d, B: %d, A: %d)", state->turtle->position.x, state->turtle->position.y, state->turtle->pencolor.r, state->turtle->pencolor.g, state->turtle->pencolor.b, state->turtle->pencolor.a);
    if (!resize_stamps_if_needed(state)) {
        return 0;   /* out of memory: no stamp */
    }

    state->stamps[state->stampCount].position = state->turtle->position;
    state->stamps[state->stampCount].color = state->turtle->pencolor;
    state->stamps[state->stampCount].id = state->nextStampID;
    state->stampCount++;

    TURTLE_LOG("[turtle_stamp]: Stamp created with ID %d", state->nextStampID);
    return state->nextStampID++;
}


/**
 * @brief Clears a specific stamp by its ID.
 * 
 * This function removes a stamp with the given ID from the screen.
 * 
 * @param state A pointer to the Turtle structure.
 * @param stamp_id The ID of the stamp to be cleared.
 */
void turtle_clear_stamp(Turtle *state, int stamp_id) {
    TURTLE_LOG("[turtle_clear_stamp]: Clearing stamp with ID %d", stamp_id);
    if (!state) {
        return;
    }

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


/**
 * @brief Calculates the distance from the turtle's current position to a specified point.
 * 
 * This function returns the Euclidean distance between the turtle's current position and the specified (x, y) coordinates.
 * 
 * @param state A pointer to the Turtle structure.
 * @param x The X-coordinate of the point to calculate the distance to.
 * @param y The Y-coordinate of the point to calculate the distance to.
 * @return float The distance from the turtle's current position to the specified point.
 */
float turtle_distance(Turtle *state, float x, float y) {
    if (!state || !state->turtle) {
        return 0.0f;
    }
    TURTLE_LOG("[turtle_distance]: Calculating distance from (%.2f, %.2f) to (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, x, y);
    
    float dx = state->turtle->position.x - x;
    float dy = state->turtle->position.y - y;
    float distance = sqrtf(dx * dx + dy * dy);
    
    TURTLE_LOG("[turtle_distance]: Distance calculated: %.2f", distance);
    return distance;
}


/**
 * @brief Calculates the distance between two turtles.
 * 
 * This function returns the Euclidean distance between the turtle's current position and the position of another turtle.
 * 
 * @param state A pointer to the Turtle structure.
 * @param other_turtle A pointer to the other turtle to calculate the distance to.
 * @return float The distance between the two turtles.
 */
float turtle_distance_turtle(Turtle *state, Tu *other_turtle) {
    if (!state || !state->turtle || !other_turtle) {
        return 0.0f;
    }
    TURTLE_LOG("[turtle_distance_turtle]: Calculating distance between turtles at (%.2f, %.2f) and (%.2f, %.2f)", state->turtle->position.x, state->turtle->position.y, other_turtle->position.x, other_turtle->position.y);

    float distance = sqrtf((state->turtle->position.x - other_turtle->position.x) * (state->turtle->position.x - other_turtle->position.x) + (state->turtle->position.y - other_turtle->position.y) * (state->turtle->position.y - other_turtle->position.y));
    
    TURTLE_LOG("[turtle_distance_turtle]: Distance calculated: %.2f", distance);
    return distance;
}


/**
 * @brief Sets the turtle's movement mode to use degrees.
 * 
 * This function sets the full circle degrees for turtle movements. The default is 360 degrees.
 * 
 * @param state A pointer to the Turtle structure.
 * @param fullcircle The number of degrees in a full circle (e.g., 360 for degrees, 2 * PI for radians).
 */
void turtle_degrees(Turtle *state, float fullcircle) {
    TURTLE_LOG("[turtle_degrees]: Setting full circle degrees to %.2f", fullcircle);
    if (!state || !(fullcircle > 0.0f) || !isfinite(fullcircle)) {
        return;
    }
    state->fullcircle_degrees = fullcircle;
}


/**
 * @brief Sets the turtle's movement mode to use radians.
 * 
 * This function sets the turtle to use radians for its angular movements (i.e., 2 * PI radians for a full circle).
 * 
 * @param state A pointer to the Turtle structure.
 */
void turtle_radians(Turtle *state) {
    TURTLE_LOG("[turtle_radians]: Setting full circle to 2 * PI (radians mode)");
    if (!state) {
        return;
    }
    state->fullcircle_degrees = 2 * PI;
}


/**
 * @brief Retrieves the turtle's current heading in degrees.
 * 
 * This function returns the turtle's current heading, normalized to the full circle degrees (default is 360).
 * 
 * @param state A pointer to the Turtle structure.
 * @return float The turtle's current heading in degrees.
 */
float turtle_heading(Turtle *state) {
    if (!state || !state->turtle || state->fullcircle_degrees == 0.0f) { 
        return 0.0f;
    }

    float heading = fmodf(state->turtle->heading * (state->fullcircle_degrees / 360.0f), state->fullcircle_degrees);
    if (heading < 0.0f) { 
        heading += state->fullcircle_degrees;
    }

    TURTLE_LOG("[turtle_heading]: Current heading is %.2f degrees", heading);
    return heading;
}


/**
 * @brief Sets the background color for the drawing area.
 * 
 * This function sets the background color of the drawing window to the specified RGBA values.
 * 
 * @param state A pointer to the Turtle structure.
 * @param r The red component of the background color (0-255).
 * @param g The green component of the background color (0-255).
 * @param b The blue component of the background color (0-255).
 * @param a The alpha component of the background color (0-255).
 */
void turtle_set_background_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    if (!state) {
        return;
    }
    TURTLE_LOG("[turtle_set_background_color]: Setting background color to (R: %d, G: %d, B: %d, A: %d)", r, g, b, a);
    state->backgroundColor = (Color){r, g, b, a};
}


/**
 * @brief Move the turtle in the opposite of its current heading.
 *
 * Equivalent to `turtle_forward(state, -distance)` semantically, but
 * implemented as a thin wrapper so logging and animation stay
 * consistent.
 *
 * @param state    Turtle to move. NULL is a no-op.
 * @param distance How far to walk backward (in pixels).
 */
void turtle_backward(Turtle *state, float distance) {
    if (!state || !state->turtle) {
        return;
    }
    TURTLE_LOG("[turtle_backward]: Stepping back %.2f units", distance);
    /* turning by 180 and back left rounding noise in the heading */
    turtle_forward(state, -distance);
}


/**
 * @brief Read back the turtle's current x coordinate.
 * @return The x value, or 0 if @p state is NULL.
 */
float turtle_get_x(const Turtle *state) {
    if (!state || !state->turtle) {
        return 0.0f;
    }
    return state->turtle->position.x;
}


/**
 * @brief Read back the turtle's current y coordinate.
 * @return The y value, or 0 if @p state is NULL.
 */
float turtle_get_y(const Turtle *state) {
    if (!state || !state->turtle) {
        return 0.0f;
    }
    return state->turtle->position.y;
}


/**
 * @brief Read back the turtle's current speed (as set by
 *        `turtle_set_speed`).
 */
float turtle_get_speed(const Turtle *state) {
    if (!state || !state->turtle) { 
        return 0.0f;
    }
    return state->turtle->speed;
}


/* Helper: copy a Color into 4 optional out-pointers. */
static void color_to_rgba(Color c, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
    if (r) {
        *r = c.r;
    }
    if (g) { 
        *g = c.g;
    }
    if (b) { 
        *b = c.b;
    }
    if (a) { 
        *a = c.a;
    }
}


/**
 * @brief Read back the current pen color into 4 out-pointers.
 *
 * Any of the out-pointers may be NULL if the caller only cares about
 * some channels.
 */
void turtle_get_pen_color(const Turtle *state, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
    if (!state || !state->turtle) { 
        color_to_rgba((Color){0,0,0,0}, r, g, b, a); 
        return; 
    }
    color_to_rgba(state->turtle->pencolor, r, g, b, a);
}


/**
 * @brief Read back the current fill color into 4 out-pointers.
 */
void turtle_get_fill_color(const Turtle *state, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
    if (!state || !state->turtle) { 
        color_to_rgba((Color){0,0,0,0}, r, g, b, a); 
        return; 
    }
    color_to_rgba(state->turtle->fillcolor, r, g, b, a);
}


/**
 * @brief Read back the current background color into 4 out-pointers.
 */
void turtle_get_background_color(const Turtle *state, unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
    if (!state) { 
        color_to_rgba((Color){0,0,0,0}, r, g, b, a); 
        return; 
    }
    color_to_rgba(state->backgroundColor, r, g, b, a);
}


/**
 * @brief Restore the turtle to its initial state.
 *
 * Clears every drawn line, every stamp, and every fill point; resets
 * position to (400, 300), heading to 0, pen down, default colors, and
 * speed/thickness to 1. Capacities are kept so the next draw cycle
 * doesn't need to re-grow.
 */
void turtle_reset(Turtle *state) {
    if (!state) {
        return;
    }

    TURTLE_LOG("[turtle_reset]: Resetting turtle to initial state");
    state->lineCount      = 0;
    state->stampCount     = 0;
    state->fillPointCount = 0;
    turtle_free_fills(state);
    for (int i = 0; i < state->textCount; ++i) {
        free(state->texts[i].text);
    }
    state->textCount = 0;
    state->nextStampID    = 1;
    state->distanceRemaining = 0;
    state->stepSize  = 1.0f;
    state->dot.size  = 0;
    state->dot.color = (Color){0, 0, 0, 0};
    state->dot.drawn = false;
    state->fullcircle_degrees = 360.0f;
    state->backgroundColor    = WHITE;
    state->isFilling = false;
    state->fillColor = (Color){0, 0, 0, 0};

    if (state->turtle) {
        state->turtle->position  = (Vector2){400.0f, 300.0f};
        state->homePosition      = state->turtle->position;
        state->turtle->heading   = 0.0f;
        state->turtle->pencolor  = BLACK;
        state->turtle->fillcolor = BLACK;
        state->turtle->penDown   = true;
        state->turtle->filling   = false;
        state->turtle->speed     = 1.0f;
        state->turtle->thickness = 1.0f;
    }
}


/**
 * @brief Drop every drawn line / stamp without touching the turtle's
 *        position, heading, color, or pen state.
 *
 * Useful for "redraw" scenarios where you keep the turtle where it is
 * but want a blank canvas.
 */
void turtle_clear(Turtle *state) {
    if (!state) {
        return;
    }
    TURTLE_LOG("[turtle_clear]: Clearing %d lines and %d stamps", state->lineCount, state->stampCount);

    state->lineCount      = 0;
    state->stampCount     = 0;
    state->fillPointCount = 0;
    turtle_free_fills(state);
    for (int i = 0; i < state->textCount; ++i) {
        free(state->texts[i].text);
    }
    state->textCount = 0;
}


/**
 * @brief Undo the most recent drawn move.
 *
 * Removes the last line (one turtle_forward / turtle_set_position / ...
 * with the pen down - a move is recorded as one line) and puts the turtle
 * back at that line's start. Pen-up moves and turns are not recorded, so
 * they are not undone. Returns false if there is nothing to undo.
 *
 * @return true if a move was undone, false on empty / NULL.
 */
bool turtle_undo(Turtle *state) {
    if (!state || state->lineCount == 0) {
        return false;
    }
    state->lineCount--;
    if (state->turtle) {
        state->turtle->position = state->lines[state->lineCount].start;
    }
    TURTLE_LOG("[turtle_undo]: One line removed (%d remain)", state->lineCount);
    
    return true;
}


/**
 * @brief Angle (in degrees, 0..360) from the turtle's current
 *        position to the point @p (x, y).
 *
 * Uses the same convention as `turtle_forward` (heading 0 is +x and the
 * angle grows counterclockwise on screen), in the current angle unit, so
 * `turtle_set_heading(t, turtle_towards(t, x, y))` faces the point.
 * Returns 0 on NULL.
 */
float turtle_towards(const Turtle *state, float x, float y) {
    if (!state || !state->turtle) {
        return 0.0f;
    }

    /* Same orientation as the heading (counterclockwise on screen), in the
       current angle unit. */
    float deg = turtle_bearing_degrees(state, x, y);
    TURTLE_LOG("[turtle_towards]: bearing to (%.2f,%.2f) = %.2f deg", x, y, deg);
    return turtle_from_degrees(state, deg);
}


/**
 * @brief Convenience: rotate the turtle so it faces (x, y).
 *
 * Equivalent to `turtle_set_heading(state, turtle_towards(state, x, y))`.
 */
void turtle_face_towards(Turtle *state, float x, float y) {
    if (!state || !state->turtle) { 
        return;
    }

    state->turtle->heading = turtle_bearing_degrees(state, x, y);
    TURTLE_LOG("[turtle_face_towards]: heading set to %.2f", state->turtle->heading);
}


/**
 * @brief Number of line segments currently in the turtle's draw
 *        buffer.
 */
int turtle_get_line_count(const Turtle *state) {
    return state ? state->lineCount : 0;
}


/**
 * @brief Number of stamps currently in the turtle's draw buffer.
 */
int turtle_get_stamp_count(const Turtle *state) {
    return state ? state->stampCount : 0;
}


/**
 * @brief Pen-color-only setter using explicit RGBA.
 *
 * `turtle_set_color` does the same thing but its name is ambiguous
 * (turtle "color" usually means both pen and fill in Logo). This
 * helper makes the intent explicit and won't touch `fillcolor`.
 */
void turtle_set_pen_color_rgb(Turtle* state, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    if (!state || !state->turtle) { 
        return;
    }

    state->turtle->pencolor = (Color){r, g, b, a};
    TURTLE_LOG("[turtle_set_pen_color_rgb]: pen color -> (%d,%d,%d,%d)", r, g, b, a);
}


/* Internal: append a text label to the turtle's recorded labels. */
static void turtle_push_text(Turtle *state, const char *text, Vector2 position, int fontSize, Color color) {
    if (!state || !text) {
        return;
    }
    if (state->textCount >= state->textCapacity) {
        int newCap = state->textCapacity ? state->textCapacity * 2 : 8;
        TurtleText *resized = (TurtleText *)realloc(state->texts, newCap * sizeof(TurtleText));
        if (!resized) {
            TURTLE_LOG("[turtle_push_text]: realloc failed (capacity %d)", newCap);
            return;
        }
        state->texts = resized;
        state->textCapacity = newCap;
    }

    size_t len = strlen(text) + 1;
    char *copy = (char *)malloc(len);
    if (!copy) {
        return;
    }
    memcpy(copy, text, len);

    state->texts[state->textCount].text = copy;
    state->texts[state->textCount].position = position;
    state->texts[state->textCount].fontSize = fontSize;
    state->texts[state->textCount].color = color;
    state->textCount++;
}


/**
 * @brief Draw a text label at the turtle's current position.
 *
 * The label is recorded (like a line) and re-rendered every frame, so it
 * persists through `turtle_draw` / `turtle_done`. Text is drawn with raylib's
 * built-in font at the requested pixel size. Mirrors Python turtle's `write`.
 *
 * @param state    The turtle. NULL is a no-op.
 * @param text     NUL-terminated string to draw (copied internally). NULL → no-op.
 * @param fontSize Pixel height of the text.
 * @param color    Text color (a raylib `Color`).
 */
void turtle_write(Turtle *state, const char *text, int fontSize, Color color) {
    if (!state || !state->turtle || !text) {
        return;
    }
    TURTLE_LOG("[turtle_write]: '%s' at (%.1f, %.1f) size %d", text, state->turtle->position.x, state->turtle->position.y, fontSize);
    turtle_push_text(state, text, state->turtle->position, fontSize, color);
}


/**
 * @brief Draw a text label at an explicit (x, y) position.
 *
 * Like `turtle_write`, but the label is placed at the given screen
 * coordinates instead of at the turtle's current position. Useful for titles,
 * axis labels and annotations independent of where the turtle is.
 *
 * @param state    The turtle. NULL is a no-op.
 * @param x        X coordinate (screen pixels, origin top-left).
 * @param y        Y coordinate (screen pixels).
 * @param text     NUL-terminated string to draw (copied internally). NULL → no-op.
 * @param fontSize Pixel height of the text.
 * @param color    Text color.
 */
void turtle_write_at(Turtle *state, float x, float y, const char *text, int fontSize, Color color) {
    if (!state || !text) {
        return;
    }
    TURTLE_LOG("[turtle_write_at]: '%s' at (%.1f, %.1f) size %d", text, x, y, fontSize);
    turtle_push_text(state, text, (Vector2){x, y}, fontSize, color);
}


/**
 * @brief Save the current window contents to a PNG file.
 *
 * Thin wrapper over raylib's `TakeScreenshot`, so callers don't need to
 * include `raylib.h` directly. Call it after rendering a frame
 * (`turtle_draw` or inside a `turtle_done` callback) and before
 * `turtle_close_window`. The path is relative to the current working
 * directory; raylib infers the format from the extension (use `.png`).
 *
 * @param filename Output path. NULL is rejected.
 * @return `true` if a screenshot was requested, `false` if @p filename is NULL.
 */
bool turtle_save_image(const char *filename) {
    if (!filename) {
        TURTLE_LOG("[turtle_save_image]: NULL filename");
        return false;
    }
    TURTLE_LOG("[turtle_save_image]: saving screenshot to '%s'", filename);
    TakeScreenshot(filename);
    return true;
}


/**
 * @brief Remove every stamp from the canvas.
 *
 * The companion to `turtle_clear_stamp` (which removes a single stamp by id):
 * this clears them all at once. Lines and text labels are left untouched.
 *
 * @param state The turtle. NULL is a no-op.
 */
void turtle_clear_stamps(Turtle *state) {
    if (!state) {
        return;
    }
    TURTLE_LOG("[turtle_clear_stamps]: clearing %d stamps", state->stampCount);
    state->stampCount = 0;
}


/**
 * @brief Query the current drawing window size, in pixels.
 *
 * Wraps raylib's `GetScreenWidth` / `GetScreenHeight`, so you can center or
 * scale drawings to the window. Must be called after `turtle_init_window`.
 *
 * @param width  If non-NULL, receives the window width in pixels.
 * @param height If non-NULL, receives the window height in pixels.
 */
void turtle_screen_size(int *width, int *height) {
    if (width) {
        *width = GetScreenWidth();
    }
    if (height) {
        *height = GetScreenHeight();
    }
    TURTLE_LOG("[turtle_screen_size]: %d x %d", width ? *width : -1, height ? *height : -1);
}


/**
 * @brief Read back one recorded line segment by index.
 *
 * The turtle accumulates every line it draws (via forward/backward/circle/
 * set_position with the pen down) in an internal array. `turtle_get_line_count`
 * reports how many there are; this function exposes the actual geometry so a
 * caller can serialize the drawing, export it, or unit-test the path — without
 * a graphics context.
 *
 * @param state The turtle to query. Must not be NULL.
 * @param index Zero-based line index in `[0, turtle_get_line_count(state))`.
 * @param out   Receives a copy of the line (start, end, color, thickness).
 *              Must not be NULL.
 * @return true if the index was in range (and @p out was written); false on a
 *         NULL argument or an out-of-range index.
 */
bool turtle_get_line(const Turtle *state, int index, Line *out) {
    if (!state || !out || index < 0 || index >= state->lineCount) {
        TURTLE_LOG("[turtle_get_line]: invalid argument or out-of-range index %d.", index);
        return false;
    }
    *out = state->lines[index];
    return true;
}


/**
 * @brief Export the drawing to a standalone SVG file (headless, no window).
 *
 * Serializes every recorded line segment to an SVG `<line>`, on top of a
 * background `<rect>` in the turtle's background color. Unlike
 * `turtle_save_image` (which needs a live raylib/OpenGL window to grab a
 * screenshot), this works with no graphics context at all — ideal for CI,
 * servers, or producing scalable vector output. The `viewBox` is sized to the
 * drawing's bounding box plus a small margin, so the whole picture is framed
 * regardless of where the turtle wandered (including negative coordinates).
 *
 * Coordinates are written in the same top-left, y-down space the turtle draws
 * in, so the SVG matches the on-screen rendering.
 *
 * @param state    The turtle whose lines to export. Must not be NULL.
 * @param filename Destination path. Must not be NULL.
 * @return true if the file was written; false on a NULL argument or if the
 *         file could not be opened/written.
 */
bool turtle_export_svg(const Turtle *state, const char *filename) {
    if (!state || !filename) {
        TURTLE_LOG("[turtle_export_svg]: NULL argument.");
        return false;
    }

    /* Bounding box over every endpoint, so the viewBox frames the drawing. */
    float minX = 0.0f, minY = 0.0f, maxX = 0.0f, maxY = 0.0f;
    bool first = true;
    for (int i = 0; i < state->lineCount; ++i) {
        Vector2 pts[2] = { state->lines[i].start, state->lines[i].end };
        for (int k = 0; k < 2; ++k) {
            if (first) {
                minX = maxX = pts[k].x;
                minY = maxY = pts[k].y;
                first = false;
            } else {
                if (pts[k].x < minX) minX = pts[k].x;
                if (pts[k].x > maxX) maxX = pts[k].x;
                if (pts[k].y < minY) minY = pts[k].y;
                if (pts[k].y > maxY) maxY = pts[k].y;
            }
        }
    }
    if (first) {                 /* no lines: emit a small empty canvas */
        minX = minY = 0.0f;
        maxX = maxY = 100.0f;
    }

    const float margin = 10.0f;
    float vbX = minX - margin;
    float vbY = minY - margin;
    float vbW = (maxX - minX) + 2.0f * margin;
    float vbH = (maxY - minY) + 2.0f * margin;
    if (vbW <= 0.0f) vbW = 1.0f;
    if (vbH <= 0.0f) vbH = 1.0f;

    FILE *f = turtle_fopen_utf8(filename, "w");
    if (!f) {
        TURTLE_LOG("[turtle_export_svg]: cannot open '%s' for writing.", filename);
        return false;
    }

    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"%g %g %g %g\">\n",
            (double)vbX, (double)vbY, (double)vbW, (double)vbH);

    Color bg = state->backgroundColor;
    fprintf(f, "  <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" fill=\"#%02x%02x%02x\"",
            (double)vbX, (double)vbY, (double)vbW, (double)vbH,
            (unsigned)bg.r, (unsigned)bg.g, (unsigned)bg.b);
    if (bg.a < 255) {
        fprintf(f, " fill-opacity=\"%.3f\"", (double)bg.a / 255.0);
    }
    fprintf(f, "/>\n");

    for (int i = 0; i < state->lineCount; ++i) {
        Line ln = state->lines[i];
        fprintf(f, "  <line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\" stroke=\"#%02x%02x%02x\" stroke-width=\"%g\"",
                (double)ln.start.x, (double)ln.start.y, (double)ln.end.x, (double)ln.end.y,
                (unsigned)ln.color.r, (unsigned)ln.color.g, (unsigned)ln.color.b, (double)ln.thickness);
        if (ln.color.a < 255) {
            fprintf(f, " stroke-opacity=\"%.3f\"", (double)ln.color.a / 255.0);
        }
        fprintf(f, " stroke-linecap=\"round\"/>\n");
    }

    fprintf(f, "</svg>\n");

    if (fclose(f) != 0) {
        TURTLE_LOG("[turtle_export_svg]: error closing '%s'.", filename);
        return false;
    }
    TURTLE_LOG("[turtle_export_svg]: wrote %d line(s) to '%s'.", state->lineCount, filename);
    return true;
}
