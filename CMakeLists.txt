cmake_minimum_required(VERSION 3.10)
project(c_std)

# Set the C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O3 -march=native -funroll-loops -Wall -Wextra -pedantic -Wno-deprecated-declarations -s")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-auto-import")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")

# Set the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/dependency/include
)

# Link directories
link_directories(
    ${CMAKE_SOURCE_DIR}/dependency/lib
    ${CMAKE_BINARY_DIR}/build
)

# Add subdirectories for modules
add_subdirectory(string)
add_subdirectory(fmt)
add_subdirectory(encoding)
add_subdirectory(random)
add_subdirectory(map)
add_subdirectory(vector)
add_subdirectory(time)
add_subdirectory(statistics)
add_subdirectory(tuple)
add_subdirectory(turtle)
add_subdirectory(sysinfo)
add_subdirectory(stack)
add_subdirectory(span)
add_subdirectory(queue)
add_subdirectory(priority_queue)
add_subdirectory(algorithm)
add_subdirectory(numeric)
add_subdirectory(array)
add_subdirectory(bitset)
add_subdirectory(cli)
add_subdirectory(concurrent)
add_subdirectory(config)
add_subdirectory(file_io)
add_subdirectory(crypto)
add_subdirectory(csv)
add_subdirectory(date)
add_subdirectory(deque)
add_subdirectory(dir)
add_subdirectory(forward_list)
add_subdirectory(list)
add_subdirectory(log)
add_subdirectory(matrix)
add_subdirectory(network)
add_subdirectory(numbers)
add_subdirectory(json)

# Create the main executable
add_executable(main main.c)

# Link the main executable against all the module libraries and other dependencies
target_link_libraries(main
    stdstring
    fmt
    encoding
    # random
    # map
    # vector
    # stdtime
    # statistics
    # tuple
    # turtle
    # sysinfo
    # stack
    # span
    # queue
    # priority_queue
    # algorithm
    # array
    # bitset
    # cli
    # deque
    # json
    # matrix
    # fileio
    # crypto
    # config
    # stdcrypto
    # csv
    # date
    # forwardlist
    # list
    network
    # numbers
    # log
    # dir
    # concurrent
    # numeric
    raylib
    opengl32
    gdi32
    winmm
    ole32
    bthprops
    ssl
    pq
)

# Copy all necessary DLLs to the build directory
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:stdstring> $<TARGET_FILE:fmt> $<TARGET_FILE:encoding> $<TARGET_FILE:random>
    $<TARGET_FILE_DIR:main>
)

# Custom command to run the executable
add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E env "PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY};$ENV{PATH}" ./main.exe
    DEPENDS main
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Optional: Add custom command to clean the build directory
add_custom_target(clean_build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/build
)
