/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Turtle
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in turtle.c (file-level convention).
 */

#ifndef TURTLE_H
#define TURTLE_H

#include <raylib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Project convention: logging is OFF by default. Define
 * TURTLE_LOGGING_ENABLE (e.g. via -D) to turn on diagnostic output. */
/* #define TURTLE_LOGGING_ENABLE */

#ifdef TURTLE_LOGGING_ENABLE
    #define TURTLE_LOG(fmt, ...) \
        do { fprintf(stderr, "[TURTLE LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define TURTLE_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

/* Persisted line segment drawn by the turtle. */
typedef struct {
    Vector2 start;
    Vector2 end;
    Color   color;
    float   thickness;
} Line;

/* Snapshot of the turtle's image at a point in time. */
typedef struct {
    Vector2 position;
    Color   color;
    int     id;
} Stamp;

/* A text label recorded by `turtle_write`, rendered every frame. */
typedef struct {
    char*   text;
    Vector2 position;
    int     fontSize;
    Color   color;
} TurtleText;

/* Single turtle's state (one cursor in the world). */
typedef struct {
    Vector2 position;
    float   heading;
    Color   pencolor;
    Color   fillcolor;
    bool    penDown;
    bool    filling;
    float   speed;
    float   thickness;
} Tu;

/* Owning context holding the active turtle, drawn lines, stamps,
 * pending fill polygon, and per-run config. Allocate with
 * `turtle_create`, release with `turtle_deallocate`. */
typedef struct {
    Tu*      turtle;
    Line*    lines;
    int      lineCount;
    int      lineCapacity;
    Stamp*   stamps;
    int      stampCount;
    int      stampCapacity;
    int      nextStampID;
    float    distanceRemaining;
    float    stepSize;
    float    fullcircle_degrees;
    Color    backgroundColor;
    bool     isFilling;
    Color    fillColor;

    struct {
        float size;
        Color color;
        bool  drawn;
    } dot;

    Vector2* fillPoints;
    int      fillPointCount;
    int      fillPointCapacity;

    TurtleText* texts;
    int         textCount;
    int         textCapacity;
} Turtle;

/* User draw callback invoked once per frame by `turtle_done`. */
typedef void (*TurtleDrawFunc)(Turtle*);


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

Turtle* turtle_create                    (void);
void    turtle_deallocate                (Turtle* state);


/* ------------------------------------------------------------------ */
/* Window / run loop                                                  */
/* ------------------------------------------------------------------ */

void    turtle_init_window               (int width, int height, const char* title);
void    turtle_close_window              (void);
void    turtle_set_fps                   (int fps);
void    turtle_begin_drawing             (void);
void    turtle_clear_background          (Color color);
void    turtle_end_drawing               (void);
void    turtle_draw                      (Turtle* state);
void    turtle_done                      (Turtle* state, TurtleDrawFunc user_draw);


/* ------------------------------------------------------------------ */
/* Motion                                                             */
/* ------------------------------------------------------------------ */

void    turtle_forward                   (Turtle* state, float distance);
void    turtle_backward                  (Turtle* state, float distance);
void    turtle_turn                      (Turtle* state, float angle);
void    turtle_right                     (Turtle* state, float angle);
void    turtle_left                      (Turtle* state, float angle);
void    turtle_circle                    (Turtle* state, float radius, float extent, int steps);
void    turtle_face_towards              (Turtle* state, float x, float y);


/* ------------------------------------------------------------------ */
/* Position / orientation                                             */
/* ------------------------------------------------------------------ */

void    turtle_position                  (Turtle* state, float* x, float* y);
void    turtle_set_position              (Turtle* state, float x, float y);
void    turtle_set_x                     (Turtle* state, float x);
void    turtle_set_y                     (Turtle* state, float y);
void    turtle_set_heading               (Turtle* state, float to_angle);
void    turtle_home                      (Turtle* state);


/* ------------------------------------------------------------------ */
/* Pen control                                                        */
/* ------------------------------------------------------------------ */

void    turtle_pen_up                    (Turtle* state);
void    turtle_pen_down                  (Turtle* state);
void    turtle_pen_size                  (Turtle* state, float width);
void    turtle_set_speed                 (Turtle* state, float speed);


/* ------------------------------------------------------------------ */
/* Colors                                                             */
/* ------------------------------------------------------------------ */

void    turtle_set_color                 (Turtle* state, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void    turtle_color                     (Turtle* state, const char* pencolor_str, const char* fillcolor_str);
void    turtle_color_rgb                 (Turtle* state, unsigned char pr, unsigned char pg, unsigned char pb, unsigned char fr, unsigned char fg, unsigned char fb);
void    turtle_set_pen_color_rgb         (Turtle* state, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void    turtle_set_background_color      (Turtle* state, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void    turtle_get_pen_color             (const Turtle* state, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
void    turtle_get_fill_color            (const Turtle* state, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
void    turtle_get_background_color      (const Turtle* state, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);


/* ------------------------------------------------------------------ */
/* Fill                                                               */
/* ------------------------------------------------------------------ */

void    turtle_begin_fill                (Turtle* state);
void    turtle_end_fill                  (Turtle* state);


/* ------------------------------------------------------------------ */
/* Stamps / dots / clearing                                           */
/* ------------------------------------------------------------------ */

int     turtle_stamp                     (Turtle* state);
void    turtle_clear_stamp               (Turtle* state, int stamp_id);
void    turtle_dot                       (Turtle* state, float size, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void    turtle_clear                     (Turtle* state);
void    turtle_reset                     (Turtle* state);


/* ------------------------------------------------------------------ */
/* Angle mode                                                         */
/* ------------------------------------------------------------------ */

void    turtle_degrees                   (Turtle* state, float fullcircle);
void    turtle_radians                   (Turtle* state);


/* ------------------------------------------------------------------ */
/* Introspection — geometry                                           */
/* ------------------------------------------------------------------ */

float   turtle_get_x                     (const Turtle* state);
float   turtle_get_y                     (const Turtle* state);
float   turtle_heading                   (Turtle* state);
float   turtle_towards                   (const Turtle* state, float x, float y);
float   turtle_distance                  (Turtle* state, float x, float y);
float   turtle_distance_turtle           (Turtle* state, Tu* other_turtle);


/* ------------------------------------------------------------------ */
/* Introspection — pen / state                                        */
/* ------------------------------------------------------------------ */

float   turtle_get_pen_size              (Turtle* state);
float   turtle_get_speed                 (const Turtle* state);
int     turtle_get_line_count            (const Turtle* state);
bool    turtle_get_line                  (const Turtle* state, int index, Line* out);
int     turtle_get_stamp_count           (const Turtle* state);
bool    turtle_filling                   (Turtle* state);
bool    turtle_is_down                   (Turtle* state);


/* ------------------------------------------------------------------ */
/* History                                                            */
/* ------------------------------------------------------------------ */

bool    turtle_undo                      (Turtle* state);


/* ------------------------------------------------------------------ */
/* Text labels, screenshots & screen size                             */
/* ------------------------------------------------------------------ */

void    turtle_write                     (Turtle* state, const char* text, int fontSize, Color color);
void    turtle_write_at                  (Turtle* state, float x, float y, const char* text, int fontSize, Color color);
bool    turtle_save_image                (const char* filename);
bool    turtle_export_svg                (const Turtle* state, const char* filename);
void    turtle_clear_stamps              (Turtle* state);
void    turtle_screen_size               (int* width, int* height);


#ifdef __cplusplus
}
#endif

#endif 
