

# C++ Standard Library Implementation in C

This project aims to reimplement the C++ standard library functionality using the C programming language. It is designed to provide C developers with the familiar tools and libraries that are commonly available in C++. This enables better data structure management, algorithm implementation, and feature usage while staying within the C language's ecosystem.

## A Personal Note from the Me

I undertake this project out of a deep affection for the C programming language. It is my belief that C remains an essential tool for any computer engineer or programmer, providing the foundation necessary to build efficient and robust software. My love for C drives this endeavor, aiming to enrich the language with the familiar conveniences found in C++.

## tip 

You're aware that in C, we don't have many features that object-oriented languages offer, like templates, inheritance, abstraction .. and for almost everything, we have to turn to functions. Therefore, the syntax might seem a bit strange, but it's something we need to accept. The important thing is, it's still C :))

## Project Structure

The project is organized into several subdirectories, each representing a different module of the standard library:

- `Array`: Implements a dynamic array similar to `std::array`.
- `ForwardList`: Implements a singly-linked list analogous to `std::forward_list`.
- `List`: Implements a doubly-linked list similar to `std::list`.
- `Queue`: Implements a queue based on `std::queue`.
- `Stack`: Implements a stack akin to `std::stack`.
- `String`: Implements a basic string class that mimics `std::string`.
- `Vector`: Implements a dynamic array similar to `std::vector`.
- `PriorityQueue` : Implements a priority_queue based on `std::priority_queue`
- `Dqueue` : Implements a priority_queue based on `std::deque`
- 'CSVFile, CSVRow': for read, write and etc ... operations on csv file . 

Each module comes with a `.c` source file and a `.h` header file, along with a `README.md` file that provides information on how to use that specific module.

## Compilation

The modules in this project are standalone and do not have any external dependencies other than the GCC compiler. To compile any module, navigate to the desired directory and use the following command:

```
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o module_name module_name.c
```

Replace `module_name` with the appropriate file names for the module you are compiling. For example, to compile the `vector` module, you would use:

```
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o vector vector.c
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
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o your_program your_program.c vector.c
```

## Individual READMEs for Libraries

Each library module comes with its own README.md file located in its directory. These READMEs provide detailed instructions on how to integrate and use the libraries in your projects. They may include sample code, function descriptions, and other relevant usage information.

## Contribution

Contributions to this project are welcome. Whether it's extending the existing libraries, improving performance, or fixing bugs, your help is appreciated. Please feel free to fork the repository, make your changes, and submit a pull request.

## License

This project is open-source and available under [MIT](LICENSE). 


