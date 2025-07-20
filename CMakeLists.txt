cmake_minimum_required(VERSION 3.10)
project(c_std)

# Set the C standard
set(CMAKE_C_STANDARD 17)

# Platform-specific compiler flags
if (WIN32 AND MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2 /W3")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O3 -march=native -funroll-loops -Wall -Wextra -pedantic -Wno-deprecated-declarations -s")
endif()


# Platform-specific linker flags
if (WIN32)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-auto-import")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-auto-import")
endif()

# Set the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/dependency/include
)

# Link directories
if (WIN32)
    link_directories(${CMAKE_SOURCE_DIR}/dependency/lib)
else()
    link_directories(/usr/lib /usr/local/lib)
endif()


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
add_subdirectory(secrets)
add_subdirectory(xml)
add_subdirectory(regex)
add_subdirectory(unittest)
add_subdirectory(serial_port)
add_subdirectory(plot)
add_subdirectory(bigint)
add_subdirectory(evalexpr)
add_subdirectory(bigfloat)
# add_subdirectory(audio)
add_subdirectory(hashmap)
add_subdirectory(sort)
# Create the main executable
add_executable(main main.c)

# Link the main executable against all the module libraries and other dependencies
# Link the main executable against all the module libraries and other dependencies
if (WIN32)
    target_link_libraries(main
        stdstring
        fmt
        encoding
        stdcrypto
        # network
        # raylib
        opengl32
        gdi32
        winmm
        ole32
        bthprops
        ssl
        pq
        # Optional modules that are commented out:
        # random
        # map
        # vector
        # stdtime
        # statistics
        hashmap
        sort
        # tuple
        # turtle
        # sysinfo
        # stack
        # secrets
        # span
        # algorithm
        # queue
        # priority_queue
        # array
        # bitset
        # cli
        # deque
        # json
        # matrix
        # fileio
        # crypto
        # config
        # csv
        # date
        # forwardlist
        # list
        # numbers
        # log
        # dir
        # concurrent
        # xml
        # stdregex
        # unittest
        # stdserial
        # plot
        # bigint
        # evalexpr
        # bigfloat
        # audio

    )
else()
    find_package(OpenSSL REQUIRED)
    find_package(PostgreSQL REQUIRED) # Assuming PostgreSQL is needed for libpq
    target_link_libraries(main
        stdstring
        fmt
        encoding
        stdcrypto
        network
        OpenSSL::SSL
        OpenSSL::Crypto
        pthread
        dl
        m
        raylib
        pq
        # Optional modules that are commented out:
        # random
        # map
        vector
        # stdtime
        statistics
        secrets
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
        # csv
        # date
        # forwardlist
        # list
        # numbers
        # log
        # dir
        # concurrent
        # xml
        # stdregex
        # unittest
        # stdserial
        # plot
        # bigint
        evalexpr
        bigfloat
        audio
    )
endif()

# Custom command to run the executable
if (WIN32)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:stdstring> $<TARGET_FILE:fmt> $<TARGET_FILE:encoding> $<TARGET_FILE:random>
        $<TARGET_FILE_DIR:main>
    )
    add_custom_target(run
        COMMAND ${CMAKE_COMMAND} -E env "PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY};$ENV{PATH}" ./main.exe
        DEPENDS main
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

else()
    add_custom_target(run
        COMMAND ./main
        DEPENDS main
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endif()
