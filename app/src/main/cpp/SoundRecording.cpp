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

#include "SoundRecording.h"

// At a sample rate of 48000 samples/second 262144 (2^18) represents 5.46 seconds of audio.
// Each sample is stored as a float (4 bytes) so a mono SoundRecording (channelCount = 1)
// will allocate 1MB of memory. A stereo recording will allocate 2MB of memory.
constexpr int kMaxSamples = 262144; // 1 << 18;

SoundRecording::SoundRecording(const int32_t channelCount) :
        mChannelCount(channelCount) {

    mData = new float*[channelCount];
    for (int i = 0; i < channelCount; ++i) {
        mData[i] = new float[kMaxSamples];
    }
}

SoundRecording::~SoundRecording(){
    for (int i = 0; i < mChannelCount; ++i) {
        delete[] mData[i];
    }
    delete mData;
}

void SoundRecording::write(const float *sourceData, int32_t numFrames) {

    // Check that data will fit, if it doesn't fill the data buffer(s) up to capacity.
    if (mWriteIndex + numFrames > kMaxSamples) {
        numFrames = kMaxSamples - mWriteIndex;
    }

    for (int i = 0; i < numFrames; ++i) {
        for (int j = 0; j < mChannelCount; ++j) {
            mData[j][mWriteIndex] = sourceData[(i*mChannelCount)+j];
        }
        mWriteIndex++;
    }
    mTotalLength = mWriteIndex;
}

int32_t SoundRecording::read(float *targetData, int32_t numFrames){

    // If not looping then check that we're not attempting to read too much data
    if (!mIsLooping && mReadIndex + numFrames > mTotalLength) numFrames = mTotalLength - mReadIndex;

    for (int i = 0; i < numFrames; ++i){
        for (int j = 0; j < mChannelCount; ++j) {
            targetData[(i*mChannelCount)+j] = mData[j][mReadIndex];
        }
        mReadIndex++;
        if (mReadIndex >= mTotalLength) mReadIndex = 0;
    }
    return numFrames;
}

void SoundRecording::clear(){
    mTotalLength = 0;
}

bool SoundRecording::isFull() {
    return (mTotalLength == kMaxSamples);
}

void SoundRecording::resetPlayHead() {
    mReadIndex = 0;
}

void SoundRecording::resetWriteHead() {
    mWriteIndex = 0;
}

void SoundRecording::setLooping(bool isLooping) {
    mIsLooping = isLooping;
}

int32_t SoundRecording::getLength() {
    return mTotalLength;
}

int32_t SoundRecording::getChannelCount() {
    return mChannelCount;
}

int32_t SoundRecording::getMaxSamples() {
    return kMaxSamples;
}