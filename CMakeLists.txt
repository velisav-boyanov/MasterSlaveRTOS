cmake_minimum_required(VERSION 3.15)

project(freertos_linux C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Use the FreeRTOS POSIX simulator port.
set(FREERTOS_PORT GCC_POSIX CACHE STRING "" FORCE)

# Use heap_4 for dynamic allocation.
set(FREERTOS_HEAP 4 CACHE STRING "" FORCE)

# FreeRTOS application configuration
add_library(freertos_config INTERFACE)

target_include_directories(freertos_config
    SYSTEM
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(freertos_config
    INTERFACE
        projCOVERAGE_TEST=0
)

# FreeRTOS Kernel
add_subdirectory(freertos-kernel)

# Application
add_executable(device_sim
    src/main.c
    src/Os_TaskMaster.c
    src/Os_TaskSlave.c
)

# POSIX threads
find_package(Threads REQUIRED)

target_link_libraries(device_sim
    PRIVATE
        freertos_kernel
        Threads::Threads
)
