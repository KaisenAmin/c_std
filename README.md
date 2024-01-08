# C++ Standard Library Implementation in C

This project aims to reimplement the C++ standard library functionality using the C programming language. It provides C developers with tools and libraries commonly available in C++, enabling better data structure management, algorithm implementation, and feature usage while staying within the C language's ecosystem.

## A Personal Note from Me

I undertake this project out of a deep affection for the C programming language. It is my belief that C remains an essential tool for any computer engineer or programmer, providing the foundation necessary to build efficient and robust software. My love for C drives this endeavor, aiming to enrich the language with the familiar conveniences found in C++.

## Project Structure

The project is organized into several subdirectories, each representing a different module of the standard library:

- `Array`: Implements a dynamic array similar to `std::array`.
- `ForwardList`: Implements a singly-linked list analogous to `std::forward_list`.
- `List`: Implements a doubly-linked list similar to `std::list`.
- `Queue`: Implements a queue based on `std::queue`.
- `Stack`: Implements a stack akin to `std::stack`.
- `String`: Implements a basic string class that mimics `std::string`.
- `Vector`: Implements a dynamic array similar to `std::vector`.
- `PriorityQueue`: Implements a priority queue based on `std::priority_queue`.
- `Deque`: Implements a deque based on `std::deque`.
- `CsvFile`, `CsvRow`: For read, write, and other operations on CSV files.
- `ConfigFile`: Provides a flexible solution for handling configuration files in C.
- `Map`: A generic implementation of a map, or associative array.
- `Span`: Implements a span container for working with contiguous memory.
- `Algorithm`: Provides a wide range of generic functionalities similar to `std::algorithm` in C++.
- `Encoding`: A versatile tool for encoding and decoding operations.
- `Numbers`: Provides constants for common mathematical values, similar to C++20's `<numbers>`.

Each module comes with a `.c` source file, a `.h` header file, and a `README.md` file.

## Compilation and Execution

To compile and execute modules, a Python script `compile.py` is provided. It offers a cross-platform way to build and run the project.

To compile the entire project, use:
```bash
python compile.py b
```

To compile and run the project, use:
```bash
python compile.py r
```

This script compiles the project into the `build` directory.

## Usage

To use a module in your project, include the corresponding header file in your C source files. Example usage:

```c
#include "vector.h"

int main() {
    // Your code here
}
```

And compile with the Python script or manually:

```
python compile.py b
```

or

```
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o your_program your_program.c vector.c
```

## Individual READMEs for Libraries

Each library module comes with its own README.md file, providing detailed instructions, sample code, function descriptions, and other relevant usage information.

## Contribution

Contributions are welcome. Whether it's extending existing libraries, improving performance, or fixing bugs, your help is appreciated. Fork the repository, make your changes, and submit a pull request.

## License

This project is open-source and available under [GNU/GPL3](LICENSE).

---
