cmake_minimum_required(VERSION 3.10)

project(mjpeg-avi)

# Set the C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add the directory where the source files are located
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Create a list of source files for the library
set(LIB_SOURCES "${SOURCE_DIR}/avi.c")

# Create a list of source files for the executable
set(EXE_SOURCES "${SOURCE_DIR}/main.c")

# Define the shared library
add_library(avi SHARED ${LIB_SOURCES})

# Create the main program executable
add_executable(avimake ${EXE_SOURCES})

target_compile_options(avi PRIVATE -Wno-multichar)

# Link the executable to the library
target_link_libraries(avimake PRIVATE avi)

# Include header files location
target_include_directories(avi PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>)

# Set install rules for the executable
install(TARGETS avimake
        RUNTIME DESTINATION bin)
install(TARGETS avi
        RUNTIME DESTINATION lib)
install(FILES "${SOURCE_DIR}/avi.h"
        DESTINATION include)
