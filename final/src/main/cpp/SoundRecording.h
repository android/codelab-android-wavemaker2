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
#include <atomic>

#include "Definitions.h"

constexpr int kMaxSamples = 480000; // 10s of audio data @ 48kHz

class SoundRecording {

public:
    int32_t write(const float *sourceData, int32_t numSamples);
    int32_t read(float *targetData, int32_t numSamples);
    bool isFull() const { return (mWriteIndex == kMaxSamples); };
    void setReadPositionToStart() { mReadIndex = 0; };
    void clear() { mWriteIndex = 0; };
    void setLooping(bool isLooping) { mIsLooping = isLooping; };
    int32_t getLength() const { return mWriteIndex; };
    static const int32_t getMaxSamples() { return kMaxSamples; };

private:
    std::atomic<int32_t> mWriteIndex { 0 };
    std::atomic<int32_t> mReadIndex { 0 };
    std::atomic<bool> mIsLooping { false };
    std::array<float,kMaxSamples> mData { 0 };
};

#endif //WAVEMAKER2_SAMPLE_H
