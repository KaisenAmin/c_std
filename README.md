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
- `Json` : The Json library is a comprehensive and efficient tool designed for parsing, generating, and manipulating JSON data in C. It aims to provide a robust and user-friendly interface for handling JSON objects and arrays, enabling seamless integration of JSON functionality into C projects.
- `Log`: The Log library offers a flexible and powerful logging system for C projects, enabling detailed message logging across various levels (DEBUG, INFO, WARN, ERROR, FATAL). It supports multiple output destinations (console, file, or both), timestamp inclusion, log level configuration, and keyword filtering to refine log output. Integrated with file I/O operations and customizable settings, it enhances diagnostic and monitoring capabilities in development and production environments.
- `Cli`: The CLI library provides a comprehensive solution for parsing command-line arguments in C projects, supporting options, commands, and subcommands with both short and long names. It features advanced functionalities like custom error handling, option grouping, interactive mode, and pipelining support, making it versatile for both simple and complex CLI applications. This library simplifies command-line parsing, validation, and execution, offering a robust foundation for building user-friendly command-line interfaces.


Each module in the project comes with a `.c` source file, a `.h` header file, and a `README.md` file. These README files offer detailed explanations of each module's functionality, usage examples, and any other relevant information, ensuring that developers can easily understand and utilize the components of the library.


## Compilation and Execution

This project utilizes a Python script (`compile.py`) for easy compilation of modules, making the build process straightforward and efficient.

### Requirements
- Python 3.10 or higher
- GCC compiler (ensure it's added to your system's PATH)

### Using the compile.py Script

To compile the entire project, simply run the `compile.py` script with the `b` argument:

```bash
python compile.py b
```

This command compiles all source files and produces an executable in the `./build` directory.

### Running the Compiled Program

To compile and immediately run the compiled program, use the `r` argument:

```bash
python compile.py r
```

### Adding New Modules

If you add new modules or directories containing `.c` files, simply include their paths in the `source_directories` list within the `compile.py` script. The script automatically finds and compiles all `.c` files in the specified directories.

### Streamlined Build Process

The use of `compile.py` eliminates the need for traditional makefiles or manual compilation commands, providing a simple and unified build process. The script handles dependencies, includes, and linking, ensuring a hassle-free compilation experience.


## Manual Compilation Using GCC

For developers who prefer manual compilation or need to integrate the project into other build systems, the source files can be compiled using the GCC command line. While the `compile.py` script is recommended for its convenience and automated handling of file dependencies, manual compilation offers flexibility for advanced use cases.

### Requirements for Manual Compilation
- GCC compiler (ensure it's added to your system's PATH)
- C17 standard support in GCC

### Compiling with GCC

To manually compile a specific module or your entire project, you can use the GCC command with the `-std=c17` flag to ensure compliance with the C17 standard. Here's an example command to compile a program with the `vector` module:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o your_program your_program.c vector.c
```

In this command:
- `-std=c17` specifies the use of the C17 standard.
- `-O3`, `-march=native`, `-flto`, and `-funroll-loops` are optimization flags.
- `-Wall`, `-Wextra`, and `-pedantic` enable additional warnings for better code quality.
- `-s` strips the binary for a smaller executable size.
- `your_program.c` is your main C source file.
- `vector.c` is the source file for the `vector` module (include other `.c` files as needed).
- `your_program` is the output executable file.

### Customizing the Compilation

You can modify the GCC command to suit your specific requirements, such as including additional modules, linking libraries, or adjusting optimization levels. This approach offers full control over the compilation process, allowing you to tailor it to your project's needs.

---

## Individual READMEs for Libraries

Each library module comes with its own README.md file, providing detailed instructions, sample code, function descriptions, and other relevant usage information.

## Contribution

Contributions are welcome. Whether it's extending existing libraries, improving performance, or fixing bugs, your help is appreciated. Fork the repository, make your changes, and submit a pull request.

## License

This project is open-source and available under [ISC License](LICENSE).

---

