cmake_minimum_required(VERSION 3.2)
project(RenderDeviceLib)

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Add external libs
add_subdirectory(externals)

# Build FrameGraphLib
add_subdirectory(source)

# Build examples
add_subdirectory(examples)
