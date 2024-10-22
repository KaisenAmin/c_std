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

1. **Clone the raylib repository and build it from source:**

    ```bash
    git clone https://github.com/raysan5/raylib.git
    cd raylib/src
    make PLATFORM=PLATFORM_DESKTOP
    sudo make install
    ```

2. **Install additional required packages:**

    ```bash
    sudo apt-get install libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libgl1-mesa-dev
    ```

3. **Rebuild raylib with `-fPIC`:**

   To ensure `raylib` is compiled with position-independent code, follow these steps:

   ```bash
   mkdir build
   cd ~/Desktop/raylib/build
   make clean
   cmake .. -DCMAKE_C_FLAGS="-fPIC" -DCMAKE_BUILD_TYPE=Release
   make
   sudo make install
   ```

#### Windows

For Windows, it is recommended to use MSYS2 to install `raylib`. First, download and install MSYS2 from [msys2.org](https://www.msys2.org/). Then, open the MSYS2 terminal and run the following commands:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-raylib
```

### Compiling the Turtle Graphics Library

To use this library, include `turtle.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./turtle/turtle.c -lraylib -lm
```

### Rebuilding the Project

After `raylib` has been successfully recompiled and installed, go back to your `c_std` project directory and clean the build files:

```bash
cd ~/Desktop/c_std/build
rm -rf CMakeCache.txt CMakeFiles/ build/
cmake -G "Unix Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ ..
cmake --build .
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with usage examples. It covers basic operations like moving the turtle and drawing shapes.

### Function Descriptions


### `Turtle *turtle_create(void)`

**Purpose**:  
This function creates and initializes a new `Turtle` instance. It sets up various turtle attributes like position, heading, pen color, and line drawing structures. This function also allocates memory for storing lines, stamps, and fill points, which are necessary for turtle drawing.

**Parameters**:  
None.

**Return Value**:  
- A pointer to the newly created `Turtle` structure.
- If memory allocation fails, the program exits with an error.

**Usage Case**:  
Use this function when starting a new turtle drawing session to initialize the turtle's state and prepare it for drawing.

---

### `void turtle_deallocate(Turtle *state)`

**Purpose**:  
Deallocates and cleans up all memory associated with the `Turtle` instance. This includes freeing the memory for the turtle itself, the lines it has drawn, stamps, and any points used for filling shapes.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure that needs to be deallocated.

**Return Value**:  
This function does not return any value.

**Usage Case**:  
Call this function when you are done with the turtle to free up memory and avoid memory leaks.

---

### `void turtle_close_window(void)`

**Purpose**:  
Closes the window used for rendering turtle graphics. This function should be called when the drawing session is over and the window is no longer needed.

**Parameters**:  
None.

**Return Value**:  
This function does not return any value.

**Usage Case**:  
Use this function to close the window after completing the turtle drawing, typically at the end of a program or session.

---

### `void turtle_init_window(int width, int height, const char *title)`

**Purpose**:  
Initializes a window with the specified width, height, and title for turtle graphics rendering. This window is where all turtle-related drawings will appear.

**Parameters**:  
- `width`: The width of the window in pixels.
- `height`: The height of the window in pixels.
- `title`: The title of the window that will appear in the title bar.

**Return Value**:  
This function does not return any value.

**Usage Case**:  
Call this function at the start of your turtle graphics session to create a window where drawings can be rendered.

---

### `void turtle_set_fps(int fps)`

**Purpose**:  
Sets the target frames per second (FPS) for the turtle window, controlling how often the window refreshes and how smoothly graphics are rendered.

**Parameters**:  
- `fps`: The desired frames per second value.

**Return Value**:  
This function does not return any value.

**Usage Case**:  
Use this function to adjust the speed of rendering in your turtle window. A higher FPS will result in smoother animations, while a lower FPS will slow down the rendering process.

--- 

### `void turtle_set_speed(Turtle *state, float speed)`

**Purpose**:  
This function adjusts the movement speed of the turtle. It also recalculates the step size that determines how far the turtle moves per frame based on the given speed.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure whose speed is being set.
- `speed`: The desired speed for the turtle as a floating-point value.

**Return Value**:  
None.

**Usage Case**:  
Use this function to increase or decrease the turtle's movement speed, which directly affects how fast it draws on the screen.

---

### `void turtle_pen_size(Turtle *state, float width)`

**Purpose**:  
This function sets the thickness of the pen that the turtle uses to draw lines. The thicker the pen, the wider the lines it draws.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure whose pen size is being set.
- `width`: The desired thickness of the pen as a floating-point value.

**Return Value**:  
None.

**Usage Case**:  
Use this function to change the thickness of the lines drawn by the turtle. For example, setting a thicker pen size is useful for bold or emphasized drawings.

---

### `float turtle_get_pen_size(Turtle *state)`

**Purpose**:  
This function retrieves the current thickness (width) of the turtle's pen.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure from which to retrieve the pen size.

**Return Value**:  
The current pen thickness as a floating-point value.

**Usage Case**:  
Use this function when you need to know the current pen size, for example, to display it or adjust the drawing style based on it.

---

### `bool turtle_is_down(Turtle *state)`

**Purpose**:  
This function checks if the turtle's pen is currently down (meaning it is in drawing mode) or up (meaning it is moving without drawing).

**Parameters**:  
- `state`: A pointer to the `Turtle` structure whose pen state is being checked.

**Return Value**:  
- `true`: If the pen is down.
- `false`: If the pen is up.

**Usage Case**:  
Use this function to determine if the turtle is actively drawing on the screen, which can help in making decisions about when to draw lines.

---

### `void turtle_forward(Turtle *state, float distance)`

**Purpose**:  
This function moves the turtle forward by a specified distance. If the pen is down, it draws a line along the turtle's path. The movement is broken into smaller steps for smooth transitions, and if the turtle is in fill mode, points are added to the fill buffer.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's state.
- `distance`: The distance (in units) to move the turtle forward.

**Return Value**:  
None.

**Usage Case**:  
Use this function to move the turtle forward by a specified amount, either drawing a line or moving without drawing, depending on the pen state.

---

### `void turtle_position(Turtle *state, float *x, float *y)`

**Purpose**:  
This function retrieves the current x and y coordinates of the turtle's position.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `x`: A pointer to store the x-coordinate of the turtle's position.
- `y`: A pointer to store the y-coordinate of the turtle's position.

**Return Value**:  
None.

**Usage Case**:  
Use this function to get the current position of the turtle in the graphical window.

---

### `void turtle_turn(Turtle *state, float angle)`

**Purpose**:  
This function adjusts the turtle's heading by a specified angle in degrees.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `angle`: The angle (in degrees) to turn the turtle.

**Return Value**:  
None.

**Usage Case**:  
Use this function to turn the turtle by a specified number of degrees, changing its heading without affecting its position.

---

### `void turtle_right(Turtle *state, float angle)`

**Purpose**:  
This function turns the turtle to the right (clockwise) by decreasing the turtle's heading by a specified angle.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `angle`: The angle (in degrees) to turn the turtle to the right.

**Return Value**:  
None.

**Usage Case**:  
Use this function to turn the turtle right, making the turtle face a new direction without moving its position.

---

### `void turtle_left(Turtle *state, float angle)`

**Purpose**:  
This function turns the turtle to the left (counterclockwise) by increasing the turtle's heading by a specified angle.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `angle`: The angle (in degrees) to turn the turtle to the left.

**Return Value**:  
None.

**Usage Case**:  
Use this function to turn the turtle left, changing its direction without affecting its current position.

---

### `void turtle_pen_up(Turtle *state)`

**Purpose**:  
This function lifts the turtle's pen, preventing it from drawing lines while it moves.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.

**Return Value**:  
None.

**Usage Case**:  
Use this function when you want to move the turtle without drawing a line.

---

### `void turtle_pen_down(Turtle *state)`

**Purpose**:  
This function lowers the turtle's pen, allowing it to draw lines as it moves. When the pen is down, any movement by the turtle will leave a trail (line).

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.

**Return Value**:  
None.

**Usage Case**:  
Use this function when you want the turtle to start drawing as it moves.

---

### `void turtle_set_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a)`

**Purpose**:  
This function sets the turtle's pen color using specified RGBA values. The color will be used to draw lines as the turtle moves with the pen down.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `r`: Red component of the color (0-255).
- `g`: Green component of the color (0-255).
- `b`: Blue component of the color (0-255).
- `a`: Alpha (transparency) component of the color (0-255).

**Return Value**:  
None.

**Usage Case**:  
Use this function to set the pen color of the turtle with specific RGBA values, allowing control over the transparency of the drawing.

---

### `void turtle_color(Turtle *state, const char *pencolor_str, const char *fillcolor_str)`

**Purpose**:  
This function sets both the pen and fill colors for the turtle using hexadecimal color strings. The pen color is used for drawing lines, while the fill color is used during fill operations.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `pencolor_str`: Hexadecimal string representing the pen color (e.g., "#FF0000" for red).
- `fillcolor_str`: Hexadecimal string representing the fill color.

**Return Value**:  
None.

**Usage Case**:  
Use this function to set the pen and fill colors using hex codes, which are commonly used in web and graphics programming.

---

### `void turtle_color_rgb(Turtle *state, unsigned char pr, unsigned char pg, unsigned char pb, unsigned char fr, unsigned char fg, unsigned char fb)`

**Purpose**:  
This function sets the pen color and fill color for the turtle using RGB values. The pen color is used for drawing lines, and the fill color is used for filling shapes.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `pr`: Red component for the pen color (0-255).
- `pg`: Green component for the pen color (0-255).
- `pb`: Blue component for the pen color (0-255).
- `fr`: Red component for the fill color (0-255).
- `fg`: Green component for the fill color (0-255).
- `fb`: Blue component for the fill color (0-255).

**Return Value**:  
None.

**Usage Case**:  
Use this function to set both pen and fill colors for the turtle using RGB values, providing full control over the drawing and filling colors.

---

### `bool turtle_filling(Turtle *state)`

**Purpose**:  
This function checks if the turtle is currently in fill mode. When in fill mode, the turtle collects points for creating a filled shape.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.

**Return Value**:  
- `true` if the turtle is filling, `false` otherwise.

**Usage Case**:  
Use this function to check whether the turtle is currently in fill mode, indicating whether the turtle will fill a shape.

---

### `void turtle_begin_fill(Turtle *state)`

**Purpose**:  
This function starts the fill operation for the turtle. It enables the turtle's filling mode and begins tracking points for the filled shape.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.

**Return Value**:  
None.

**Usage Case**:  
Use this function when you want the turtle to begin collecting points for creating a filled shape.

---

### `void turtle_end_fill(Turtle *state)`

**Purpose**:  
This function ends the fill operation and draws the filled shape using the points collected during the fill process. The shape is drawn using the turtle's current fill color.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.

**Return Value**:  
None.

**Usage Case**:  
Use this function to complete a fill operation and render the filled shape.

---

### `void turtle_draw(Turtle *state)`

**Purpose**:  
This function renders the current state of the turtle, including the lines it has drawn, any filled shapes, stamps, and the turtle itself. It clears the background before drawing the updated scene.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.

**Return Value**:  
None.

**Usage Case**:  
Use this function to render everything the turtle has drawn in the window, ensuring that all visual elements (lines, stamps, turtle, etc.) are updated.

---

### `void turtle_begin_drawing(void)`

**Purpose**:  
This function begins a drawing operation, preparing the canvas for rendering shapes, lines, or any other drawing instructions.

**Parameters**:  
None.

**Return Value**:  
None.

**Usage Case**:  
Use this function at the beginning of any drawing process, before rendering anything on the screen.

---

### `void turtle_clear_background(Color color)`

**Purpose**:  
This function clears the screen by filling the background with the specified color.

**Parameters**:  
- `color`: A `Color` structure specifying the RGBA values to fill the background.

**Return Value**:  
None.

**Usage Case**:  
Use this function to clear the screen before drawing anything new, ensuring that previous frames are erased.

---

### `void turtle_end_drawing(void)`

**Purpose**:  
This function concludes the current drawing operation, finalizing the rendering process.

**Parameters**:  
None.

**Return Value**:  
None.

**Usage Case**:  
Use this function to finalize the drawing operation, after all rendering instructions have been issued.

---

### `void turtle_done(Turtle *state, TurtleDrawFunc user_draw)`

**Purpose**:  
This function runs the main drawing loop for the turtle graphics, rendering the turtle and its drawings repeatedly until the window is closed. It also calls a user-provided drawing function (`user_draw`), allowing custom drawings to be added.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.
- `user_draw`: A function pointer to a user-defined drawing function that takes a `Turtle` pointer as a parameter.

**Return Value**:  
None.

**Usage Case**:  
Use this function to handle the continuous drawing loop for turtle graphics. You can also pass a custom function to render additional shapes or drawings as needed.

---

### `void turtle_set_position(Turtle *state, float x, float y)`

**Purpose**:  
This function moves the turtle to the specified `(x, y)` coordinates. If the pen is down, a line is drawn from the current position to the new position. If the turtle is in fill mode, the new position is added to the fill points for future shape filling.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.
- `x`: The new X-coordinate for the turtle's position.
- `y`: The new Y-coordinate for the turtle's position.

**Return Value**:  
None.

**Usage Case**:  
Use this function when you need to move the turtle to a specific position on the screen. The turtle will draw a line if its pen is down.

---

### `void turtle_set_x(Turtle *state, float x)`

**Purpose**:  
This function moves the turtle to the specified X-coordinate, keeping the Y-coordinate unchanged. If the pen is down, a line is drawn from the current position to the new X-coordinate. If the turtle is in fill mode, the new position is added to the fill points.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.
- `x`: The new X-coordinate for the turtle's position.

**Return Value**:  
None.

**Usage Case**:  
Use this function when you only want to adjust the X-coordinate of the turtle without changing its Y-coordinate.

---

### `void turtle_set_y(Turtle *state, float y)`

**Purpose**:  
This function moves the turtle to the specified Y-coordinate, keeping the X-coordinate unchanged. If the pen is down, a line is drawn from the current position to the new Y-coordinate. If the turtle is in fill mode, the new position is added to the fill points.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.
- `y`: The new Y-coordinate for the turtle's position.

**Return Value**:  
None.

**Usage Case**:  
Use this function when you want to adjust the Y-coordinate of the turtle while keeping its X-coordinate unchanged.

---

### `void turtle_set_heading(Turtle *state, float to_angle)`

**Purpose**:  
This function sets the turtle's heading (direction) to a specific angle, in degrees.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.
- `to_angle`: The desired heading angle in degrees.

**Return Value**:  
None.

**Usage Case**:  
Use this function to change the turtle's direction to a specific angle.

---

### `void turtle_home(Turtle *state)`

**Purpose**:  
This function moves the turtle back to the home position `(0, 0)` and resets its heading to 0 degrees. If the pen is down, a line is drawn from the turtle's current position to the home position.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.

**Return Value**:  
None.

**Usage Case**:  
Use this function to bring the turtle back to its starting position at the center of the screen and reset its heading direction.

---

### `void turtle_circle(Turtle *state, float radius, float extent, int steps)`

**Purpose**:  
This function draws a circle or an arc with the specified radius, extent (arc angle), and number of steps (resolution). If the number of steps is not provided, it is automatically calculated based on the radius.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.
- `radius`: The radius of the circle.
- `extent`: The angular extent of the circle or arc in degrees (default is 360 for a full circle).
- `steps`: The number of steps to divide the circle into. If set to 0, it is automatically calculated.

**Return Value**:  
None.

**Usage Case**:  
Use this function to draw a full circle or an arc. The number of steps controls how smooth the circle or arc looks. You can also specify how much of the circle to draw using the `extent` parameter.

---

### `void turtle_dot(Turtle *state, float size, unsigned char r, unsigned char g, unsigned char b, unsigned char a)`

**Purpose**:  
This function draws a dot of the specified size and color at the turtle's current position.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.
- `size`: The size of the dot to be drawn.
- `r`: The red component of the dot's color (0-255).
- `g`: The green component of the dot's color (0-255).
- `b`: The blue component of the dot's color (0-255).
- `a`: The alpha (transparency) component of the dot's color (0-255).

**Return Value**:  
None.

**Usage Case**:  
Use this function when you want the turtle to draw a circular dot at its current location.

---

### `int turtle_stamp(Turtle *state)`

**Purpose**:  
This function stamps the turtle's current position and pencolor onto the screen. Each stamp is assigned a unique ID, allowing it to be cleared later.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.

**Return Value**:  
Returns the unique ID of the created stamp.

**Usage Case**:  
Use this function when you want to mark the turtle's current location and pencolor without moving the turtle.

---

### `void turtle_clear_stamp(Turtle *state, int stamp_id)`

**Purpose**:  
This function clears a previously created stamp by its ID, removing it from the screen.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `stamp_id`: The unique ID of the stamp to be cleared.

**Return Value**:  
None.

**Usage Case**:  
Use this function to remove a specific stamp from the screen based on its unique ID.

---

### `float turtle_distance(Turtle *state, float x, float y)`

**Purpose**:  
This function calculates and returns the Euclidean distance between the turtle's current position and a specified point.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.
- `x`: The X-coordinate of the point to calculate the distance to.
- `y`: The Y-coordinate of the point to calculate the distance to.

**Return Value**:  
Returns the distance between the turtle's current position and the specified point.

**Usage Case**:  
Use this function when you need to determine how far the turtle is from a particular point on the screen.

---

### `float turtle_distance_turtle(Turtle *state, Tu *other_turtle)`

**Purpose**:  
This function calculates and returns the Euclidean distance between two turtles.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the current turtle.
- `other_turtle`: A pointer to another turtle whose distance from the current turtle is to be calculated.

**Return Value**:  
Returns the distance between the two turtles.

**Usage Case**:  
Use this function to calculate how far apart two turtles are from each other.

---

### `void turtle_degrees(Turtle *state, float fullcircle)`

**Purpose**:  
This function sets the turtle's movement mode to use degrees, with the specified number of degrees representing a full circle (typically 360 degrees).

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `fullcircle`: The number of degrees that make up a full circle (e.g., 360 for degrees, or other custom values).

**Return Value**:  
None.

**Usage Case**:  
Use this function to configure the turtle to work with degrees for its angular movements.

---

### `void turtle_radians(Turtle *state)`

**Purpose**:  
This function switches the turtle's angular movement to use radians, with `2 * PI` radians representing a full circle.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.

**Return Value**:  
None.

**Usage Case**:  
Use this function to switch the turtle to use radians for angular movements, instead of degrees.

--- 

### `float turtle_heading(Turtle *state)`

**Purpose**:  
This function retrieves the turtle's current heading in degrees, normalized to the full circle degrees (default is 360).

**Parameters**:  
- `state`: A pointer to the `Turtle` structure representing the turtle's current state.

**Return Value**:  
Returns the turtle's current heading in degrees, adjusted for the full circle setting.

**Usage Case**:  
Use this function when you need to know the direction the turtle is currently facing in degrees, particularly when using degrees for movement.

---

### `void turtle_set_background_color(Turtle *state, unsigned char r, unsigned char g, unsigned char b, unsigned char a)`

**Purpose**:  
This function sets the background color of the drawing window to the specified RGBA values, allowing you to customize the background appearance.

**Parameters**:  
- `state`: A pointer to the `Turtle` structure.
- `r`: The red component of the background color (0-255).
- `g`: The green component of the background color (0-255).
- `b`: The blue component of the background color (0-255).
- `a`: The alpha (transparency) component of the background color (0-255).

**Return Value**:  
None.

**Usage Case**:  
Use this function to change the background color of the turtle's drawing area, making it suitable for different visual designs.

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

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

---

## Example 18 : Drawing a Spiral Pattern 
```c
#include <math.h>
#include "turtle/turtle.h"



void draw_spiral(Turtle *state) {
    static bool drawn = false;

    if (!drawn) {
        for (int i = 0; i < 360; i++) {
            // Set the color based on the angle to create a gradient effect
            int red = (int)(127 * (1 + cos(DEG2RAD * i)));
            int green = (int)(127 * (1 + sin(DEG2RAD * i)));
            int blue = 255 - red;

            turtle_set_color(state, red, green, blue, 255);
            turtle_forward(state, i * 0.5);
            turtle_right(state, 59); 
        }
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Spiral Pattern Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 5.0f);

    turtle_done(state, draw_spiral);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

---

## Example 19 : Drawing a flower Pattern 
```c
#include <math.h>
#include "turtle/turtle.h"


void draw_petal(Turtle *state, float radius, float angle) {
    for (int i = 0; i < 2; i++) {
        turtle_circle(state, radius, angle, 50);
        turtle_right(state, 180 - angle);
    }
}

void draw_flower(Turtle *state) {
    static bool drawn = false;
    if (!drawn) {
        for (int i = 0; i < 36; i++) {
            // Set the color based on the angle to create a gradient effect
            int red = (int)(127 * (1 + cos(DEG2RAD * i * 10)));
            int green = (int)(127 * (1 + sin(DEG2RAD * i * 10)));
            int blue = 255 - red;

            turtle_set_color(state, red, green, blue, 255);
            draw_petal(state, 50, 60);
            turtle_right(state, 10);
        }
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Flower Pattern Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 5.0f);

    turtle_done(state, draw_flower);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

---

## Example 20 : Drawing a hexagonal Spiral 

```c
#include <math.h>
#include "turtle/turtle.h"


void draw_hexagonal_spiral(Turtle *state) {
    static bool drawn = false;
    if (!drawn) {
        float length = 10.0f;
        for (int i = 0; i < 100; i++) {
            
            int red = (int)(127 * (1 + cos(DEG2RAD * i * 3.6)));
            int green = (int)(127 * (1 + sin(DEG2RAD * i * 3.6)));
            int blue = 255 - red;

            turtle_set_color(state, red, green, blue, 255);
            turtle_forward(state, length);
            turtle_right(state, 60); 
            length += 2.0f; 
        }
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Hexagonal Spiral Example");
    turtle_set_fps(60);
    turtle_set_speed(state, 5.0f);

    turtle_done(state, draw_hexagonal_spiral);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

---

## Example 21 : draw a heart 

```c
#include <math.h>
#include "turtle/turtle.h"


void curve(Turtle *state) {
    for (int i = 0; i < 200; i++) {
        turtle_left(state, 1);
        turtle_forward(state, 1);
    }
}

void draw_heart(Turtle *state) {
    static bool drawn = false;
    if (!drawn) {
        turtle_set_speed(state, 3.0f);
        
        turtle_set_color(state, 255, 0, 0, 255);
        turtle_pen_size(state, 5);
        
        turtle_color(state, "#FF0000", "#FFFFFF");
        turtle_begin_fill(state);

        turtle_right(state, 140); 
        turtle_forward(state, 111.65);
        curve(state);
        turtle_right(state, 120);
        curve(state);
        turtle_forward(state, 111.65);
        
        turtle_end_fill(state);
        
        drawn = true;
    }
}

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    Turtle *state = turtle_create();
    turtle_init_window(screenWidth, screenHeight, "Turtle Heart Shape Example");
    turtle_set_fps(60);

    turtle_set_background_color(state, 0, 0, 0, 255);
    turtle_done(state, draw_heart);

    turtle_deallocate(state);
    turtle_close_window();

    return 0;
}
```

---

## Conclusion

This Turtle Graphics library simplifies the process of drawing graphics in C projects, providing an intuitive interface for moving the turtle and drawing shapes and lines. The provided examples illustrate how to use the library for common graphics tasks.

