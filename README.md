# C++ Standard Library and Qt, Python etc Implementation in C

This project aims to reimplement the C++ standard library functionality using the C programming language. It provides C developers with tools and libraries commonly available in C++, enabling better data structure management, algorithm implementation, and feature usage while staying within the C language's ecosystem.

## A Personal Note from Me

I undertake this project out of a deep affection for the C programming language. It is my belief that C remains an essential tool for any computer engineer or programmer, providing the foundation necessary to build efficient and robust software. My love for C drives this endeavor, aiming to enrich the language with the familiar conveniences found in C++.

### Project Structure

The project is organized into several subdirectories, each representing a different module of the standard library:

- `Array`: The Array library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::array` in C++.

- `ForwardList`: The ForwardList library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size ForwardList, offering similar functionality to `std::forward_list` in C++. its single linked list.

- `List`: The List library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size List, offering similar functionality to `std::list` in C++. its double linked list

- `Queue`: The Queue library is part of a project aimed at reimplementing C++ standard library features in C. It provides a generic container that encapsulates dynamic-sized Queues, offering functionality similar to `std::queue` in C++.

- `Stack`: The Stack library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::stack` in C++.

- `String`: The String library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::string` in C++.

- `Vector`: This Vector library, is an implementation that mimics the functionality of the C++ `std::vector`. It aims to provide dynamic array capabilities to C programs. The Vector library allows for creating dynamic arrays that can resize automatically when elements are added or removed. It supports various operations such as inserting, deleting, and accessing elements, as well as more advanced functionalities like memory pooling for efficient allocation and deallocation of elements.

- `PriorityQueue`: The Priority Queue library is part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic-sized priority queues, offering similar functionality to `std::priority_queue` in C++.

- `Deque`: The Deque library provides a generic container that mimics the functionality of `std::deque` in C++. It offers a dynamic-size double-ended queue (deque) that allows fast insertion and removal of elements from both ends. 

- `CsvFile`, `CsvRow`: This library provides a simple and efficient way to read, manipulate, and write CSV (Comma-Separated Values) files . It offers functions for creating and destroying CSV rows and files, reading and writing CSV files, and performing various operations like appending rows, inserting columns, and exporting data in JSON format.

- `ConfigFile`: The ConfigFile library is a versatile and easy-to-use solution for handling configuration files. It allows developers to read, modify, and save configurations in the standard INI file format. The library supports sections, key-value pairs, and comments, making it ideal for managing configuration data in various C applications.

- `Map`: The Map library provides a generic implementation of a map (also known as an associative array). This map is implemented as a Red-Black Tree, which is a type of self-balancing binary search tree. This ensures efficient operations like insertion, deletion, and lookup, all with a time complexity of O(log n). similar to `std::map`

- `Span`: The Span library is a custom C implementation inspired by the C++ `std::span` container. It provides a view over a contiguous sequence of elements, allowing for efficient and safe access without owning the underlying data.

- `Algorithm`: The Algorithm library is a versatile collection of functions designed to bring the power and flexibility of C++ STL `<algorithm>` functionality to C programming. It provides a suite of generic algorithms for a variety of operations on arrays and other data structures, ranging from searching and sorting to transforming and accumulating elements.

- `Encoding`: The Encoding library is a versatile tool designed for efficient and user-friendly encoding and decoding operations. While it currently supports Base64 encoding and decoding, the library is structured to accommodate additional encoding methods in the future. The library's extensible architecture ensures it can be adapted to include other encoding types such as Base16, Base32, and URL encoding, catering to a wide range of applications and data formats.

- `Numbers`: The Numbers library in C provides constants for common mathematical values, analogous to the `<numbers>` header introduced in C++20. It offers a simple way to access important mathematical constants in C programming, enhancing the precision and readability of mathematical computations.

- `Crypto`: Provides tools for cryptographic operations. This module might include functions for encryption and decryption, hashing, and secure random number generation. It's intended to offer C developers basic cryptographic utilities.

- `Time`: The Time library in C is a comprehensive tool designed for handling time-related operations. Inspired by the functionality of more advanced languages, this library provides a robust set of functions for manipulating and comparing time values in C programs.

- `Date`: The Date library in C offers a robust solution for handling and manipulating dates in both Gregorian and Persian calendars. This library is particularly useful for applications requiring operations on dates, such as calculations, conversions, and comparisons. Its comprehensive set of functions simplifies complex date-related tasks in C programming.

- `Dir`: This library is dedicated to directory and file operations, offering comprehensive functionality to manage and manipulate files and directories on the filesystem. It is designed to provide a straightforward and efficient way to interact with the filesystem in C programming.

- `Tuple`: This library offers a versatile implementation of tuples, structures that can hold a fixed number of elements of potentially different types. It's designed for efficiency and ease of use in a variety of applications, from data structures to systems programming. its similar to `std::tuple`

- `FileWriter`: The FileWriter library is a versatile tool designed for file writing operations in C. It provides functionalities similar to higher-level languages, offering various modes for writing text and binary files, including support for Unicode (UTF-8 and UTF-16).

- `FileReader`: The FileReader library is a versatile tool designed for file reading operations in C.

- `fmt`: The fmt library is a comprehensive formatting and I/O library, inspired by the `fmt` package in Go. It offers a wide range of formatting options and is designed to work seamlessly with multilingual and Unicode text.

- `Json`: The Json library is a comprehensive and efficient tool designed for parsing, generating, and manipulating JSON data. It aims to provide a robust and user-friendly interface for handling JSON objects and arrays, enabling seamless integration of JSON functionality into C projects.

- `Log`: The Log library offers a flexible and powerful logging system for C projects, enabling detailed message logging across various levels (DEBUG, INFO, WARN, ERROR, FATAL). It supports multiple output destinations (console, file, or both), timestamp inclusion, log level configuration, and keyword filtering to refine log output. Integrated with file I/O operations and customizable settings, it enhances diagnostic and monitoring capabilities in development and production environments.

- `Cli`: The CLI library provides a comprehensive solution for parsing command-line arguments in C projects, supporting options, commands, and subcommands with both short and long names. It features advanced functionalities like custom error handling, option grouping, interactive mode, and pipelining support, making it versatile for both simple and complex CLI applications. This library simplifies command-line parsing, validation, and execution, offering a robust foundation for building user-friendly command-line interfaces.

- `Network`: The Network library provides support for TCP and UDP protocols, enabling the development of networked applications. It includes functions for creating sockets, binding, listening, connecting, sending, and receiving data over both TCP and UDP connections.

- `Database`: The Database library offers tools for interacting with PostgreSQL databases. It includes functions for connecting to databases, executing queries, retrieving results, and handling transactions. This library aims to simplify database operations in C by providing a high-level API.

- `Matrix`: The Matrix library provides tools for matrix operations, including creation, manipulation, and mathematical computations on matrices. It is designed to support a wide range of matrix operations needed in scientific computing and data analysis.

- `Random`: This random library provides functionality like python random module for working with probablity and randomly numbers and elements. 

- `Statistics`: This Statistics library in C provides a set of functions for calculating mathematical statistics of numeric data. exactly like python statistics module .

- `SysInfo`: The SysInfo library provides a set of functions for retrieving detailed information about the operating system and hardware of a machine. It is designed to work on both Windows and Linux systems, though it is not tested on macOS. The library is lightweight and easy to integrate into C projects, offering developers essential system details such as OS version, kernel type, CPU architecture, and more.

- `Turtle`: The Turtle Graphics library in C provides functions for drawing shapes and lines using a "turtle" that moves around the screen. Inspired by the Python `turtle` module, this library allows C developers to easily create graphics and animations.

- `HttpRequest and HttpResponse` :The Http library provides functionality for creating and handling HTTP servers and clients. It includes support for routing, request parsing, response generation, and handling various HTTP methods (GET, POST, etc.). This library simplifies the development of web-based applications in C, enabling easy integration of HTTP functionality into C projects.

- `Secrets`: This Secrets library provides a simple interface for generating secure random numbers, tokens, and performing cryptographic operations like constant-time comparison. This makes it a suitable choice for any C project requiring secure random number generation and cryptographic security.

- `Xml`: This XML library in C provides a comprehensive and easy-to-use API for parsing, creating, modifying, and traversing XML documents. It is designed to be lightweight, efficient, and fully cross-platform, making it suitable for embedded systems, desktop applications, and server environments

- `Regex`: The Regex library in C provides functionality to compile, match, and search regular expressions. It offers flexibility with flags for case-insensitive matching, multiline support, and dotall mode. The library is cross-platform, supporting PCRE on Windows and POSIX regex on Linux, simplifying regex-based string operations in C projects.

- `UnitTest`: The UnitTest library is a lightweight and powerful framework designed to facilitate unit testing in C. It supports a wide range of features, making it a versatile and indispensable tool for ensuring code reliability and quality in complex projects.

- `BigInt`: The BigInt library in C provides a robust interface for performing arbitrary-precision integer arithmetic. It leverages the power of the GMP (GNU Multiple Precision Arithmetic) library under the hood, offering C developers a familiar API for creating, manipulating, and performing arithmetic on large integers that exceed the limitations of standard C integer types

- `HashMap`: The HashMap library provides a dynamic, hash-table-based associative array implementation similar to `std::unordered_map` in C++. It offers O(1) average time complexity for insertions, deletions, and lookups. The library supports custom hash functions, comparison functions, and memory deallocation callbacks, making it flexible for various data types. Features include automatic rehashing, iterator support, bucket interface, and built-in hash functions for common types like strings and integers.

- `Sort`: The Sort library provides a comprehensive, generic sorting library for C that offers multiple sorting algorithms with performance statistics, benchmarking, and search capabilities. It includes 8+ sorting algorithms (Insertion, QuickSort, MergeSort, HeapSort, BubbleSort, Selection, Shell, Introspective), specialized algorithms (Counting Sort, Radix Sort, Bucket Sort), optimized variants, adaptive sorting, search functions (binary search, lower/upper bounds), utility functions (array validation, reverse, shuffle), and benchmarking capabilities. The library features generic design using function pointers, performance tracking, and comprehensive documentation with complete working examples.
---

## Note 
Each module in the project comes with a `.c` source file, a `.h` header file, and a `README.md` file. These README files offer detailed explanations of each module's functionality, usage examples, and any other relevant information, ensuring that developers can easily understand and utilize the components of the library.

## Compilation and Execution


### CMake Support

This project now supports building with CMake, a powerful cross-platform build system that simplifies the process of compiling and linking code.

### Building with CMake

To build the project using CMake, follow these steps:

1. **Install CMake**:
   - Ensure that CMake is installed on your system. You can download it from the [official website](https://cmake.org/download/) or use a package manager like `apt`, `brew`, or `choco` depending on your operating system.

2. **Create a Build Directory**:
   - Navigate to the root directory of the project and create a build directory:

     ```bash
     mkdir build
     cd build
     ```

3. **Generate Build Files**:
   - Run CMake to generate the build files for your system:

     ```bash
     # Using Unix Makefiles (Linux/macOS)
     cmake -G "Unix Makefiles" ..
     
     # Using Ninja (faster, cross-platform)
     cmake -G "Ninja" ..
     ```

4. **Compile the Project**:
   - Once the build files are generated, compile the project:

     ```bash
     # If using Unix Makefiles
     make
     
     # If using Ninja
     cmake --build .
     ```

5. **Run the Compiled Program**:
   - After compilation, the executable and shared libraries will be located in the `build` directory. You can run the executable directly:

     ```bash
     ./main
     ```

   - On Linux, ensure to set the `LD_LIBRARY_PATH` if you're using shared libraries:

     ```bash
     export LD_LIBRARY_PATH=./build:$LD_LIBRARY_PATH
     ./build/main
     ```

### Adding Modules with CMake

If you add new modules, you need to update the `CMakeLists.txt` file to include them. This file is used by CMake to configure the build process. Here's how to add a new module:

1. **Edit `CMakeLists.txt`**:
   - Add the new module's source files to the `add_library` or `add_executable` commands in the `CMakeLists.txt` file.

2. **Re-run CMake**:
   - After editing `CMakeLists.txt`, re-run CMake in the `build` directory:

     ```bash
     # If using Unix Makefiles
     cmake -G "Unix Makefiles" ..
     
     # If using Ninja
     cmake -G "Ninja" ..
     ```

3. **Compile Again**:
   - Use the appropriate build tool to compile the updated project:

     ```bash
     # If using Unix Makefiles
     make
     
     # If using Ninja
     cmake --build .
     ```


This project utilizes a Python script (`compile.py`) for easy compilation of modules, making the build process straightforward and efficient.

### Requirements
- Python 3.10 or higher
- GCC compiler (ensure it's added to your system's PATH)
- **Linux Users**: Make sure to install the necessary development libraries:
  ```bash
  sudo apt-get update
  sudo apt install build-essential git cmake libasound2-dev libx11-dev libxcursor-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev wayland-protocols libwayland-dev pkg-config libxinerama-dev libxkbcommon-dev
  sudo apt-get install libssl-dev libpq-dev libraylib-dev
  ```

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

On Linux, make sure to set the `LD_LIBRARY_PATH` before running the program:

```bash
export LD_LIBRARY_PATH=./build:$LD_LIBRARY_PATH
./build/main
```

### Compiling to Shared Libraries Only

To compile only the shared libraries (DLLs or `.so` files) for each module, use the `l` argument:

```bash
python compile.py l
```

This command compiles all source files into shared libraries in the `./build` directory without producing an executable.

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

You can modify the GCC command to suit your specific requirements, such as including additional modules, linking libraries, or adjusting optimization levels. This approach offers full

 control over the compilation process, allowing you to tailor it to your project's needs.

---

## Individual READMEs for Libraries

Each library module comes with its own README.md file, providing detailed instructions, sample code, function descriptions, and other relevant usage information.

## Contribution

Contributions are welcome. Whether it's extending existing libraries, improving performance, or fixing bugs, your help is appreciated. Fork the repository, make your changes, and submit a pull request.

## License

This project is open-source and available under [ISC License](LICENSE).

