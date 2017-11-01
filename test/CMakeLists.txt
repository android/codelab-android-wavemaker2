cmake_minimum_required(VERSION 3.4.1)

if (NOT DEFINED ENV{ANDROID_NDK})
    message (FATAL_ERROR "Please set the ANDROID_NDK environment variable")
endif()

set(GOOGLETEST_ROOT $ENV{ANDROID_NDK}/sources/third_party/googletest/googletest)

if (NOT EXISTS ${GOOGLETEST_ROOT})
    message (FATAL_ERROR "GoogleTest directory does not exist: ${GOOGLETEST_ROOT}")
endif()

# Build GoogleTest as a static library
add_library(gtest STATIC ${GOOGLETEST_ROOT}/src/gtest_main.cc ${GOOGLETEST_ROOT}/src/gtest-all.cc)
target_include_directories(gtest PRIVATE ${GOOGLETEST_ROOT})
target_include_directories(gtest PUBLIC ${GOOGLETEST_ROOT}/include)

# Build our source code as a static library

set(CMAKE_CXX_FLAGS -std=c++11)
include_directories (../app/src/main/cpp/)
add_library (wavemaker2-lib STATIC ../app/src/main/cpp/SoundRecording.cpp)

# Build our test binary
add_executable (soundrecordingtest sound_recording_unittest.cpp)
target_link_libraries(soundrecordingtest wavemaker2-lib gtest)
