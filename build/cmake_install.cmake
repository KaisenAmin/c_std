# Install script for directory: C:/Users/asus/OneDrive/Desktop/project/c_std

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/c_std")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/mingw64/bin/objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/string/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/fmt/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/encoding/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/random/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/map/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/vector/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/time/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/statistics/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/tuple/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/turtle/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/sysinfo/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/stack/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/span/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/queue/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/priority_queue/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/algorithm/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/numeric/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/array/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/bitset/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/cli/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/concurrent/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/config/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/file_io/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/crypto/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/csv/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/date/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/deque/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/dir/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/forward_list/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/list/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/log/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/matrix/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/network/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/numbers/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/json/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/secrets/cmake_install.cmake")
  include("C:/Users/asus/OneDrive/Desktop/project/c_std/build/xml/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
  file(WRITE "C:/Users/asus/OneDrive/Desktop/project/c_std/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
