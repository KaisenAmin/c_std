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
- `void turtle_run(TurtleDrawFunc user_draw)`: Runs the main drawing loop, calling the user-defined drawing function.
- `void turtle_setposition(float x, float y)`: Moves the turtle to the specified position.

## Examples

### Example 1: Drawing a Star

```c
#include <raylib.h>
#include "turtle/turtle.h"

void draw_star() {
    static bool initialized = false;
    if (!initialized) {
        turtle_right(75);
        turtle_forward(100);
        for (int i = 0; i < 4; i++) {
            turtle_right(144);
            turtle_forward(100);
        }
        initialized = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    turtle_create();
    turtle_init_window(screenWidth, screenHeight, "C Turtle Graphics with raylib");
    turtle_set_fps(90);
    turtle_set_speed(5.0f);

    turtle_run(draw_star);

    return 0;
}
```
**Result:**
```
A star shape is drawn on the screen.
```

### Example 2: Moving the Turtle

```c
#include <raylib.h>
#include "turtle/turtle.h"

void draw_movement() {
    turtle_forward(50);
    turtle_right(90);
    turtle_forward(50);
    turtle_right(90);
    turtle_forward(50);
    turtle_right(90);
    turtle_forward(50);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Movement Example");
    turtle_set_fps(60);
    turtle_set_speed(3.0f);

    turtle_run(draw_movement);

    return 0;
}
```
**Result:**
```
The turtle moves in a square pattern on the screen.
```

### Example 3: Changing the Drawing Color

```c
#include <raylib.h>
#include "turtle/turtle.h"

void draw_colored_line() {
    turtle_set_color(255, 0, 0, 255); // Red color
    turtle_forward(100);
    turtle_right(90);
    turtle_set_color(0, 255, 0, 255); // Green color
    turtle_forward(100);
    turtle_right(90);
    turtle_set_color(0, 0, 255, 255); // Blue color
    turtle_forward(100);
    turtle_right(90);
    turtle_set_color(255, 255, 0, 255); // Yellow color
    turtle_forward(100);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Color Example");
    turtle_set_fps(60);
    turtle_set_speed(2.0f);

    turtle_run(draw_colored_line);

    return 0;
}
```
**Result:**
```
The turtle draws lines in different colors on the screen.
```

### Example 4: Using Pen Up and Pen Down

```c
#include <raylib.h>
#include "turtle/turtle.h"

void draw_with_pen_control() {
    turtle_forward(50);
    turtle_pen_up();
    turtle_forward(50);
    turtle_pen_down();
    turtle_forward(50);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Pen Control Example");
    turtle_set_fps(60);
    turtle_set_speed(2.0f);

    turtle_run(draw_with_pen_control);

    return 0;
}
```
**Result:**
```
The turtle draws two separate lines with a gap in between where the pen is lifted.
```

## Example 5 : Setting the turtle position with `turtle_set_position`

```c
#include <raylib.h>
#include "turtle/turtle.h"

void draw_set_position() {
    turtle_set_position(200, 200);
    turtle_forward(100);
    turtle_set_position(400, 400);
    turtle_forward(100);
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Set Position Example");
    turtle_set_fps(60);
    turtle_set_speed(2.0f);

    turtle_run(draw_set_position);

    return 0;
}
```
**Result:**
```
The turtle moves to specified positions and draws lines from those points.
```

## Example 6: turning the turtle left 

```c
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
```

## Conclusion

This Turtle Graphics library simplifies the process of drawing graphics in C projects, providing an intuitive interface for moving the turtle and drawing shapes and lines. The provided examples illustrate how to use the library for common graphics tasks.

