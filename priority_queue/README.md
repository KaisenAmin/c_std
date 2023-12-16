
# Queue Library

The Priority_Queue library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size Priority_Queues, offering similar functionality to `std::priority_queue` in C++.

## Compilation

To compile the Priority_Queue library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -o main ./main.c priority_queue/priority_queue.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Priority_Queue library in your project, include the `priority_queue.h` header file in your source code.

```c
#include "priority_queue/priority_queue.h"
```