/*
 * Copyright 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef WAVEMAKER2_SAMPLE_H
#define WAVEMAKER2_SAMPLE_H

#include <cstdint>
#include <array>
#include "Definitions.h"
#include <android/log.h>
#include <memory>


// At a sample rate of 48000 samples/second 262144 (2^18) represents 5.46 seconds of audio.
// Each sample is stored as a float (4 bytes) so a mono SoundRecording (channelCount = 1)
// will allocate 1MB of memory. A stereo recording will allocate 2MB of memory.
constexpr int kMaxFrames = 262144; // 1 << 18;


class SoundRecording {

public:
    SoundRecording(const int32_t channelCount = kChannelCountMono);
    int32_t write(const float *sourceData, int32_t numFrames);
    int32_t read(float *targetData, int32_t numFrames);
    void clear() { mTotalFrames = 0; };
    bool isFull() const { return (mTotalFrames == kMaxFrames); };
    void resetPlayHead() { mReadFrameIndex = 0; };
    void resetWriteHead() { mWriteFrameIndex = 0; };
    void setLooping(bool isLooping) { mIsLooping = isLooping; };
    int32_t getLength() const { return mTotalFrames; };
    int32_t getChannelCount() const { return mChannelCount; };
    static int32_t getMaxFrames() { return kMaxFrames; };

private:
    int32_t mChannelCount;
    int32_t mWriteFrameIndex = 0;
    int32_t mReadFrameIndex = 0;
    std::unique_ptr<float[]> mData;
    int32_t mTotalFrames = 0;
    bool mIsLooping = false;
};

#endif //WAVEMAKER2_SAMPLE_H
