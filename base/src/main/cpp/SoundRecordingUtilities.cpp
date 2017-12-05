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

#include <cstring>
#include "SoundRecordingUtilities.h"

float convertInt16ToFloat(int16_t intValue){

    // We use asymmetrical conversion (different calculation for positive and negative values)
    // because int16 has a range of -32768 to +32767 and we should preserve the minimum and
    // maximum values. -32768 => -1, 0 => 0, 32767 => 1
    // More info here: http://blog.bjornroche.com/2009/12/linearity-and-dynamic-range-in-int.html
    const float negative_multiplier = -1.0f/INT16_MIN;
    const float positive_multiplier = 1.0f/INT16_MAX;

    float floatValue = 0;

    if (intValue < 0){
        floatValue = intValue * negative_multiplier;
    } else if (intValue > 0){
        floatValue = intValue * positive_multiplier;
    }
    return floatValue;
}

void convertArrayInt16ToFloat(int16_t *source, float *target, int32_t length){

    for (int i = 0; i < length; ++i) {
        target[i] = convertInt16ToFloat(source[i]);
    }
}

void fillArrayWithZeros(float *data, int32_t length) {
    memset(data, 0, length * sizeof(float));
}

void convertArrayMonoToStereo(float *data, int32_t numFrames) {

    for (int i = numFrames - 1; i >= 0; i--) {
        data[i*2] = data[i];
        data[(i*2)+1] = data[i];
    }
}