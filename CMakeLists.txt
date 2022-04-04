cmake_minimum_required(VERSION 3.1)

project(TinyScaler)
 
if(NOT CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE not set, setting it to Release")
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

option(ENABLE_SIMD "Enable SIMD support." On)
option(ENABLE_NEON "Enable Arm Neon support." Off)

############################################################################

# Optimizations
if ("${CMAKE_C_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -ffast-math")
endif()

if(ENABLE_SIMD)
    message(STATUS "SIMD enabled.")

    add_compile_definitions(ENABLE_SIMD)

    if(ENABLE_NEON)
	message(STATUS "Neon support enabled.")

	add_compile_definitions(ENABLE_NEON)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
    endif()
endif()

add_library(TinyScaler "src/scaler.c")

