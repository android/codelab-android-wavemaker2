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

#include <android/log.h>
#include "SoundRecording.h"

int32_t SoundRecording::write(const float *sourceData, int32_t numSamples) {

    // Check that data will fit, if it doesn't just write as much as we can.
    if (mWriteIndex + numSamples > kMaxSamples) {
        numSamples = kMaxSamples - mWriteIndex;
    }

    for (int i = 0; i < numSamples; ++i) {
        mData[mWriteIndex++] = sourceData[i];
    }
    return numSamples;
}

int32_t SoundRecording::read(float *targetData, int32_t numSamples){

    int32_t framesRead = 0;
    while (framesRead < numSamples && mReadIndex < mWriteIndex){
        targetData[framesRead++] = mData[mReadIndex++];
        if (mIsLooping && mReadIndex == mWriteIndex) mReadIndex = 0;
    }
    return framesRead;
}