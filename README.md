

# C Standard Library Implementation in C

## Project Overview

This project aims to reimplement the C++ standard library functionality using the C programming language. It is designed to provide C developers with the familiar tools and libraries that are commonly available in C++. This enables better data structure management, algorithm implementation, and feature usage while staying within the C language's ecosystem.

## Project Structure

The project is organized into several subdirectories, each representing a different module of the standard library:

- `array`: Implements a dynamic array similar to `std::array`.
- `forward_list`: Implements a singly-linked list analogous to `std::forward_list`.
- `list`: Implements a doubly-linked list similar to `std::list`.
- `queue`: Implements a queue based on `std::queue`.
- `stack`: Implements a stack akin to `std::stack`.
- `string`: Implements a basic string class that mimics `std::string`.
- `vector`: Implements a dynamic array similar to `std::vector`.
- `priority_queue` : Implements a priority_queue based on `std::priority_queue`

Each module comes with a `.c` source file and a `.h` header file, along with a `README.md` file that provides information on how to use that specific module.

## Compilation

The modules in this project are standalone and do not have any external dependencies other than the GCC compiler. To compile any module, navigate to the desired directory and use the following command:

```
gcc -o module_name module_name.c
```

Replace `module_name` with the appropriate file names for the module you are compiling. For example, to compile the `vector` module, you would use:

```
gcc -o vector vector.c
```

## Usage

To use a module in your project, include the corresponding header file in your C source files and compile them with the module's implementation. For instance:

```c
#include "vector.h"

int main() {
    // Your code here
}
```

And compile with:

```
gcc -o your_program your_program.c vector.c
```

## Individual READMEs for Libraries

Each library module comes with its own README.md file located in its directory. These READMEs provide detailed instructions on how to integrate and use the libraries in your projects. They may include sample code, function descriptions, and other relevant usage information.

## Contribution

Contributions to this project are welcome. Whether it's extending the existing libraries, improving performance, or fixing bugs, your help is appreciated. Please feel free to fork the repository, make your changes, and submit a pull request.

## License

This project is open-source and available under [LICENSE](LICENSE). Feel free to use, modify, and distribute the code as you see fit.


