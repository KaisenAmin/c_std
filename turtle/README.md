## Turtle Graphics Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

The Turtle Graphics library in C provides functions for drawing shapes and lines using a "turtle" that moves around the screen. Inspired by the Python `turtle` module, this library allows C developers to easily create graphics and animations.

## Key Features

- **Turtle Movement:** Functions to move the turtle forward and turn it to different angles.
- **Drawing Control:** Functions to lift and lower the pen, control the drawing color, and set the turtle's speed.
- **Window Management:** Functions to initialize the drawing window, set the frame rate, and clear the background.
- **User-Defined Drawing Functions:** Support for user-defined drawing functions to be executed within the main drawing loop.
- **Documentation:** Fully documented with Doxygen format for easy integration and usage.

## Installation and Compilation

### Installing Dependencies

To use this library, you need to have the `raylib` library installed. Follow the instructions below for your operating system:

#### Linux

Install `raylib` using your package manager or build from source. On Debian-based systems, you can use:

```bash
sudo apt-get install libraylib-dev
```

#### Windows

For Windows, it is recommended to use MSYS2 to install `raylib`. First, download and install MSYS2 from [msys2.org](https://www.msys2.org/). Then, open the MSYS2 terminal and run the following commands:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-raylib
```


To use this library, include `turtle.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./turtle/turtle.c -lraylib -lm
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with usage examples. It covers basic operations like moving the turtle and drawing shapes.

### Function Descriptions

- `void turtle_create(void)`: Initializes the turtle's position, heading, color, and other settings.
- `void turtle_close_window(void)`: Closes the drawing window.
- `void turtle_init_window(int width, int height, const char* title)`: Initializes the drawing window with the specified width, height, and title.
- `void turtle_set_fps(int fps)`: Sets the frame rate for the drawing window.
- `void turtle_set_speed(float speed)`: Sets the speed of the turtle's movement.
- `void turtle_forward(float distance)`: Moves the turtle forward by the specified distance.
- `void turtle_position(float *x, float *y)`: Retrieves the current position of the turtle.
- `void turtle_turn(float angle)`: Turns the turtle by the specified angle.
- `void turtle_right(float angle)`: Turns the turtle to the right by the specified angle.
- `void turtle_left(float angle)`: Turns the turtle to the left by the specified angle.
- `void turtle_pen_up(void)`: Lifts the pen, so moving the turtle will not draw.
- `void turtle_pen_down(void)`: Lowers the pen, so moving the turtle will draw.
- `void turtle_set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)`: Sets the color for drawing.
- `void turtle_draw(void)`: Draws the lines and shapes created by the turtle.
- `void turtle_begin_drawing(void)`: Begins the drawing process.
- `void turtle_clear_background(Color color)`: Clears the background with the specified color.
- `void turtle_end_drawing(void)`: Ends the drawing process.
- `void turtle_done(TurtleDrawFunc user_draw)`: Runs the main drawing loop, calling the user-defined drawing function. and  Keep the turtle window open until it is manually closed .
- `void turtle_setposition(float x, float y)`: Moves the turtle to the specified position.
- `void turtle_set_x(float x)`: set the turtles first coordiante to x leave second coordinate unchanged.
- `void turtle_set_y(float y)`: set the turtle's second coordiante to y. leave first coordinate unchanged.
- `void turtle_set_heading(float angle)`: This function sets the orientation of the turtle to the specified angle in degrees, allowing the turtle to face a specific direction. This angle can be any number, with common directions like 0 for east, 90 for north, 180 for west, and 270 for south.
- `void turtle_home(void)`: This function moves the turtle to the origin (coordinates 0,0) and resets its heading to the initial orientation (0 degrees). This allows the turtle to start from the default position and direction.
- `void turtle_circle(float radius, float extent, int steps)`: This function draws a circle or an arc with a specified radius and extent, where the extent is the angle of the arc in degrees. If steps is not specified, it calculates the appropriate number of steps to approximate the circle or arc smoothly.
- `void turtle_dot(float size, unsigned char r, unsigned char g, unsigned char b, unsigned char a)`: This function draws a dot at the turtle's current position with the specified size and color. The color is defined by the RGBA values provided as parameters.

- `void turtle_clear_stamp(int stamp_id)`: This function removes a previously made stamp identified by the provided stamp_id. This allows for selective removal of stamps from the canvas.

- `int turtle_stamp(void)`: The turtle_stamp function creates a stamp at the turtle's current position and returns a unique stamp_id. This stamp_id can later be used to clear the specific stamp.

## Examples

### Example 1: Drawing a Star

```c
#include "turtle/turtle.h"

void draw_star(Turtle *state) {
    static bool initialized = false;
    if (!initialized) {
        turtle_right(state, 75);
        turtle_forward(state, 100);

        for (int i = 0; i < 4; i++) {
            turtle_right(state, 144);
            turtle_forward(state, 100);
        }
        initialized = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "C Turtle Graphics with raylib");
    turtle_set_fps(90);
    turtle_set_speed(state, 5.0f);

    turtle_done(state, draw_star);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}

```
**Result:**
```
A star shape is drawn on the screen.
```

### Example 2: Moving the Turtle

```c
#include "turtle/turtle.h"

void draw_movement(Turtle *state) {
    static bool drawn = false; // static variable to ensure drawing happens once

    if (!drawn) {
        turtle_forward(state, 50);
        turtle_right(state, 90);
        turtle_forward(state, 50);
        turtle_right(state, 90);
        turtle_forward(state, 50);
        turtle_right(state, 90);
        turtle_forward(state, 50);
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Movement Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 3.0f);

    turtle_done(state, draw_movement);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```
**Result:**
```
The turtle moves in a square pattern on the screen.
```

### Example 3: Changing the Drawing Color

```c
#include "turtle/turtle.h"

// draw always
void draw_colored_line(Turtle *state) {
    turtle_set_color(state, 255, 0, 0, 255); // Red color
    turtle_forward(state, 100);
    turtle_right(state, 90);
    turtle_set_color(state, 0, 255, 0, 255); // Green color
    turtle_forward(state, 100);
    turtle_right(state, 90);
    turtle_set_color(state, 0, 0, 255, 255); // Blue color
    turtle_forward(state, 100);
    turtle_right(state, 90);
    turtle_set_color(state, 255, 255, 0, 255); // Yellow color
    turtle_forward(state, 100);
}

void draw_colored_line_once(Turtle *state) {
    static bool drawn = false;

    if (!drawn) {
        turtle_set_color(state, 255, 0, 0, 255); // Red color
        turtle_forward(state, 100);
        turtle_right(state, 90);
        turtle_set_color(state, 0, 255, 0, 255); // Green color
        turtle_forward(state, 100);
        turtle_right(state, 90);
        turtle_set_color(state, 0, 0, 255, 255); // Blue color
        turtle_forward(state, 100);
        turtle_right(state, 90);
        turtle_set_color(state, 255, 255, 0, 255); // Yellow color
        turtle_forward(state, 100);
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Color Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_colored_line_once);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```
**Result:**
```
The turtle draws lines in different colors on the screen.
```

### Example 4: Using Pen Up and Pen Down

```c
#include "turtle/turtle.h"

void draw_with_pen_control(Turtle *state) {
    static bool drawn = false;

    if (!drawn) {
        turtle_forward(state, 50);
        turtle_pen_up(state);
        turtle_forward(state, 50);
        turtle_pen_down(state);
        turtle_forward(state, 50);
        drawn = true; 
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Pen Control Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_with_pen_control);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}

```
**Result:**
```
The turtle draws two separate lines with a gap in between where the pen is lifted.
```

## Example 5 : Setting the turtle position with `turtle_set_position`

```c
#include "turtle/turtle.h"

void draw_set_position(Turtle *state) {
    static bool drawn = false; // Static variable to ensure drawing happens once

    if (!drawn) {
        turtle_set_position(state, 200, 200);
        turtle_forward(state, 100);
        turtle_set_position(state, 400, 400);
        turtle_forward(state, 100);
        drawn = true; // Mark as drawn
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Set Position Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_set_position);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}

```
**Result:**
```
The turtle moves to specified positions and draws lines from those points.
```

## Example 6: turning the turtle left 

```c
#include "turtle/turtle.h"

void draw_left_turn(Turtle *state) {
    turtle_forward(state, 100);
    turtle_left(state, 90);
    turtle_forward(state, 100);
    turtle_left(state, 90);
    turtle_forward(state, 100);
    turtle_left(state, 90);
    turtle_forward(state, 100);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Left Turn Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_left_turn);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

## Example 7 : set x and y with `turtle_set_x | turtle_set_y`

```c
#include "turtle/turtle.h"

void draw_set_position(Turtle *state) {
    static bool drawn = false; 

    if (!drawn) {
        turtle_set_x(state, 200);
        turtle_forward(state, 100);
        turtle_set_y(state, 400);
        turtle_forward(state, 100);
        drawn = true; 
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    
    turtle_init_window(screenWidth, screenHeight, "Turtle Set Position Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_set_position);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}

```

## Example 8 : set heading with `turtle_set_heading`
```c
#include "turtle/turtle.h"

void draw_set_heading(Turtle *state) {
    static bool drawn = false; 

    if (!drawn) {
        turtle_set_heading(state, 90);
        turtle_forward(state, 100);
        turtle_set_heading(state, 180);
        turtle_forward(state, 100);
        drawn = true; 
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Set Heading Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_set_heading);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

## Example 9 : how to send angle to home position with `turtle_home`

```c
#include "turtle/turtle.h"

void draw_home(Turtle *state) {
    static bool drawn = false;

    if (!drawn) {
        turtle_forward(state, 100);
        turtle_set_heading(state, 90);
        turtle_forward(state, 100);
        turtle_home(state);
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Home Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_home);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

## Example 10 : how to draw a circle with `turtle_circle`

```c
#include "turtle/turtle.h"

void draw_circle(Turtle *state) {
    static bool drawn = false;

    if (!drawn) {
        turtle_circle(state, 50, 360, 0);
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
```

## Example 11 : draw dot with `turtle_dot`

```c
#include "turtle/turtle.h"

void draw_dot(Turtle *state) {
    static bool drawn = false;

    if (!drawn) {
        turtle_dot(state, 20, 255, 0, 0, 255);  // draw a red dot with size 20
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Dot Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_dot);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}

```

## Example 12 : clear stamp 

```c
#include "turtle/turtle.h"

void draw_stamps(Turtle *state) {
    static bool drawn = false;
    static int stamp_id;

    if (!drawn) {
        turtle_set_color(state, 0, 0, 255, 255);  
        stamp_id = turtle_stamp(state);          // Stamp the turtle at the current position
        turtle_forward(state, 50);                
        turtle_clear_stamp(state, stamp_id);     // Clear the previously stamped turtle
        drawn = true; 
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Stamp Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_stamps);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

## Example 13 : calculate the distance with `turtle_distance | turtle_distance_turtle`

```c
#include "turtle/turtle.h"
#include "fmt/fmt.h"

void draw_distance(Turtle *state) {
    static bool drawn = false;
    if (!drawn) {
        turtle_home(state);
        float dist = turtle_distance(state, 30, 40);
        fmt_printf("Distance to (30, 40): %f\n", dist);

        Tu other_turtle;
        other_turtle.position = (Vector2){400.0f, 300.0f};
        other_turtle.heading = 0.0f;
        other_turtle.pencolor = BLACK;
        other_turtle.penDown = true;
        other_turtle.speed = 1.0f;
        other_turtle.thickness = 1.0f;

        turtle_forward(state, 77);
        dist = turtle_distance_turtle(state, &other_turtle);
        fmt_printf("Distance to other turtle: %f\n", dist);

        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Distance Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_distance);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

## Example 14 : change size of pen also check pen is down or up `turtle_pen_size | turtle_get_pen_size | turtle_is_down`

```c
#include "turtle/turtle.h"
#include "fmt/fmt.h"

void draw_pen(Turtle *state) {
    static bool drawn = false;
    if (!drawn) {
        fmt_printf("current pen size: %f\n", turtle_get_pen_size(state));
        turtle_pen_size(state, 10);
        fmt_printf("new pen size: %f\n", turtle_get_pen_size(state));

        turtle_pen_down(state);
        fmt_printf("is pen down? %s\n", turtle_is_down(state) ? "Yes" : "No");

        turtle_forward(state, 100);
        turtle_right(state, 90);
        turtle_forward(state, 100);

        turtle_pen_up(state);
        fmt_printf("is pen down? %s\n", turtle_is_down(state) ? "Yes" : "No");

        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Pen Size Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);

    turtle_done(state, draw_pen);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

## Example 15 : star shape 

```c
#include "turtle/turtle.h"
#include "fmt/fmt.h"

void draw_spiral(Turtle *state) {
    static bool drawn = false;

    if (!drawn) {
        float a = 0;
        float b = 0;

        turtle_set_color(state, 255, 0, 0, 255);  
        turtle_pen_up(state);
        turtle_set_position(state, 400, 400);    
        turtle_pen_down(state);

        while (true) {
            turtle_forward(state, a);
            turtle_right(state, b);
        
            a += 3;
            b += 1;
            if (b >= 210) {
                break;
            }
        }

        turtle_pen_up(state);  // Hide the turtle
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Spiral Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 2.0f);
    turtle_set_background_color(state, 0, 0, 0, 255);  // black

    turtle_done(state, draw_spiral);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

## Example 16 : create colorize shape `Circle spiro graph`

```c
#include "turtle/turtle.h"

void draw_circles(Turtle *state) {
    Color colors[] = {
        {255, 0, 0, 255},      // red
        {0, 0, 255, 255},      // blue
        {255, 0, 255, 255},    // magenta
        {0, 255, 0, 255},      // green
        {255, 255, 0, 255},    // yellow
        {255, 255, 255, 255}   // white
    };

    const int num_colors = sizeof(colors) / sizeof(colors[0]);

    static bool drawn = false;
    if (!drawn) {
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < num_colors; j++) {
                turtle_set_color(state, colors[j].r, colors[j].g, colors[j].b, colors[j].a);
                turtle_circle(state, 100, 360, 36);
                turtle_left(state, 10);
            }
        }
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Circles Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 10.f);
    turtle_set_background_color(state, 0, 0, 0, 255); // black

    turtle_done(state, draw_circles);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

## Example 17 : circle 

```c
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
```
## Conclusion

This Turtle Graphics library simplifies the process of drawing graphics in C projects, providing an intuitive interface for moving the turtle and drawing shapes and lines. The provided examples illustrate how to use the library for common graphics tasks.

