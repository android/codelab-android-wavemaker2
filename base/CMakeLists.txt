cmake_minimum_required(VERSION 3.4.1)

add_library( native-lib SHARED
             src/main/cpp/jni-bridge.cpp
             src/main/cpp/AudioEngine.cpp
             src/main/cpp/SoundRecording.cpp
             src/main/cpp/SoundRecordingUtilities.cpp)

target_link_libraries( native-lib
                       log
                       aaudio)