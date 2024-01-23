# C++ Standard Library and Qt, Python etc Implementation in C

This project aims to reimplement the C++ standard library functionality using the C programming language. It provides C developers with tools and libraries commonly available in C++, enabling better data structure management, algorithm implementation, and feature usage while staying within the C language's ecosystem.

## A Personal Note from Me

I undertake this project out of a deep affection for the C programming language. It is my belief that C remains an essential tool for any computer engineer or programmer, providing the foundation necessary to build efficient and robust software. My love for C drives this endeavor, aiming to enrich the language with the familiar conveniences found in C++.

### Project Structure

The project is organized into several subdirectories, each representing a different module of the standard library:

- `Array`: Implements a dynamic array similar to `std::array` in C++.
- `ForwardList`: Implements a singly-linked list analogous to `std::forward_list` in C++.
- `List`: Implements a doubly-linked list similar to `std::list` in C++.
- `Queue`: Implements a queue based on `std::queue` in C++.
- `Stack`: Implements a stack akin to `std::stack` in C++.
- `String`: Implements a basic string class that mimics `std::string` in C++.
- `Vector`: Implements a dynamic array similar to `std::vector` in C++.
- `PriorityQueue`: Implements a priority queue based on `std::priority_queue` in C++.
- `Deque`: Implements a deque based on `std::deque` in C++.
- `CsvFile`, `CsvRow`: For read, write, and other operations on CSV files.
- `ConfigFile`: Provides a flexible solution for handling configuration files in C.
- `Map`: A generic implementation of a map, or associative array.
- `Span`: Implements a span container for working with contiguous memory.
- `Algorithm`: Provides a wide range of generic functionalities similar to `std::algorithm` in C++.
- `Encoding`: A versatile tool for encoding and decoding operations.
- `Numbers`: Provides constants for common mathematical values, similar to C++20's `<numbers>`.
- `Numeric`: Implements various numerical operations and algorithms, including those for basic arithmetic, interpolation, and mathematical calculations. This module aims to bring the functionality of the C++ `<numeric>` header to C, offering features like GCD, LCM, and midpoint calculations, among others.
- `Crypto`: Provides tools for cryptographic operations. This module might include functions for encryption and decryption, hashing, and secure random number generation. It's intended to offer C developers basic cryptographic utilities, similar to what might be found in a more comprehensive C++ cryptographic library.
- `Time`: The Time library in C is a comprehensive tool designed for handling time-related operations. Inspired by the functionality of more advanced languages, this library provides a robust set of functions for manipulating and comparing time values in C programs.
- `Date`: The Date library in C offers a robust solution for handling and manipulating dates in both Gregorian and Persian calendars. This library is particularly useful for applications requiring operations on dates, such as calculations, conversions, and comparisons. Its comprehensive set of functions simplifies complex date-related tasks in C programming.
- `Dir`: Dir  is used to manipulate path names, access information regarding paths and files, and manipulate the underlying file system.
- `Tuple`: implement tuple similar to std::tuple in c++.
- `FileWriter`: The FileWriter library is a versatile tool designed for file writing operations in C. It provides functionalities similar to higher-level languages, offering various modes for writing text and binary files, including support for Unicode (UTF-8 and UTF-16).
- `FileReader`: The FileReader library is a versatile tool designed for file writing operations in C.
- `fmt`: The fmt library is a comprehensive formatting and I/O library for C, inspired by fmt package in `golang`. It offers a wide range of formatting options and is designed to work seamlessly with multilingual and Unicode text.

Each module in the project comes with a `.c` source file, a `.h` header file, and a `README.md` file. These README files offer detailed explanations of each module's functionality, usage examples, and any other relevant information, ensuring that developers can easily understand and utilize the components of the library.

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
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o your_program your_program.c vector.c
```

## Individual READMEs for Libraries

Each library module comes with its own README.md file, providing detailed instructions, sample code, function descriptions, and other relevant usage information.

## Contribution

Contributions are welcome. Whether it's extending existing libraries, improving performance, or fixing bugs, your help is appreciated. Fork the repository, make your changes, and submit a pull request.

## License

This project is open-source and available under [ISC License](LICENSE).

---

