cmake_minimum_required(VERSION 3.1)

include(CheckSymbolExists)

project(TinyScaler)
 
if(NOT CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE not set, setting it to Release")
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

option(ENABLE_SIMD "Enable SIMD support." On)

############################################################################

# Optimizations
if ("${CMAKE_C_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -ffast-math")
endif()

if(ENABLE_SIMD)
    message(STATUS "SIMD enabled.")

    add_compile_definitions(ENABLE_SIMD)

    # Check if is ARM
    check_symbol_exists(__arm__ "stdio.h" ARM_PLATFORM)

    if(ARM_PLATFORM)
	message(STATUS "ARM platform detected.")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
    endif()

endif()

add_library(TinyScaler "src/scaler.c")

