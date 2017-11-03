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

SoundRecording::SoundRecording(const int32_t channelCount) :
        mChannelCount(channelCount),
        mData(std::make_unique<float[]>((size_t)kMaxFrames * channelCount)){
}

int32_t SoundRecording::write(const float *sourceData, int32_t numFrames) {

    // Check that data will fit, if it doesn't just write as much as we can.
    if (mWriteFrameIndex + numFrames > kMaxFrames) {
        numFrames = kMaxFrames - mWriteFrameIndex;
    }

    for (int i = 0; i < numFrames; ++i) {
        for (int j = 0; j < mChannelCount; ++j) {
            mData[(mWriteFrameIndex*mChannelCount)+j] = sourceData[(i*mChannelCount)+j];
        }
        mWriteFrameIndex++;
    }

    mTotalFrames += numFrames;
    return numFrames;
}

int32_t SoundRecording::read(float *targetData, int32_t numFrames){

    // Check that we're not attempting to read too much data (unless we're looping)
    if (!mIsLooping && mReadFrameIndex + numFrames > mTotalFrames){
        numFrames = mTotalFrames - mReadFrameIndex;
    }

    for (int i = 0; i < numFrames; ++i) {
        for (int j = 0; j < mChannelCount; ++j) {
            targetData[(i*mChannelCount)+j] = mData[(mReadFrameIndex*mChannelCount)+j];
        }
        mReadFrameIndex++;
        if (mReadFrameIndex >= mTotalFrames) mReadFrameIndex = 0;
    }
    return numFrames;
}