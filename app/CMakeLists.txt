cmake_minimum_required(VERSION 3.4.1)

add_library( native-lib SHARED
             src/main/cpp/native-lib.cpp
             src/main/cpp/AudioEngine.cpp
             src/main/cpp/SoundRecording.cpp
             src/main/cpp/SoundRecordingUtilities.cpp)

target_link_libraries( native-lib
                       log
                       aaudio)

set(GOOGLETEST_ROOT ${ANDROID_NDK}/sources/third_party/googletest/googletest)
add_library(gtest STATIC ${GOOGLETEST_ROOT}/src/gtest_main.cc ${GOOGLETEST_ROOT}/src/gtest-all.cc)
target_include_directories(gtest PRIVATE ${GOOGLETEST_ROOT})
target_include_directories(gtest PUBLIC ${GOOGLETEST_ROOT}/include)

add_executable(soundrecordingtest ../test/sound_recording_unittest.cpp)
target_link_libraries(soundrecordingtest gtest native-lib)

find_program(ADB adb)
add_custom_command(TARGET soundrecordingtest POST_BUILD
    COMMAND ${ADB} shell mkdir -p /data/local/tmp/${ANDROID_ABI}
    COMMAND ${ADB} push $<TARGET_FILE:native-lib> /data/local/tmp/${ANDROID_ABI}/
    COMMAND ${ADB} push $<TARGET_FILE:soundrecordingtest> /data/local/tmp/${ANDROID_ABI}/
    COMMAND ${ADB} shell \"export LD_LIBRARY_PATH=/data/local/tmp/${ANDROID_ABI}\; /data/local/tmp/${ANDROID_ABI}/soundrecordingtest\")
