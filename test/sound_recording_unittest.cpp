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

#include <gtest/gtest.h>
#include "../app/src/main/cpp/SoundRecording.h"

class SoundRecordingTest : public ::testing::Test {
protected:

    void SetUp(){
        sr = new SoundRecording();
        sr2 = new SoundRecording(2);
    }

    void TearDown(){
        delete sr;
        delete sr2;
    }

    SoundRecording *sr;
    SoundRecording *sr2;

    float sourceData[10] = {0.1, 0.2, 0.3, 0.4, 0.5, -0.1F, -0.2F, -0.3F, -0.4F, -0.5F};
    float targetData[20];

    int32_t dataCapacity = SoundRecording::getMaxFrames();
};

TEST_F (SoundRecordingTest, WriteOne){

    sr->write(sourceData, 1);
    sr->read(targetData, 1);
    ASSERT_EQ(sourceData[0], targetData[0]);
}

TEST_F (SoundRecordingTest, WriteMax){

    // Create the source data
    float sourceData[dataCapacity];
    float targetData[dataCapacity];

    for (int i = 0; i < dataCapacity; ++i) {
        sourceData[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    }

    sr->write(sourceData, dataCapacity);
    EXPECT_EQ(dataCapacity, sr->getLength());
    sr->read(targetData, dataCapacity);

    for (int i = 0; i < dataCapacity; ++i) {
        ASSERT_EQ(sourceData[i], targetData[i]);
    }
}

TEST_F (SoundRecordingTest, WriteMoreThanMaxCapacity){

    const int numFrames = dataCapacity+1;
    float sourceData[numFrames];
    int framesWritten = sr->write(sourceData, numFrames);
    ASSERT_EQ(framesWritten, dataCapacity);
}

TEST_F (SoundRecordingTest, ReportsFullWhenFull){

    float sourceData[dataCapacity];
    sr->write(sourceData, dataCapacity);

    ASSERT_EQ(sr->isFull(), true);
}

TEST_F (SoundRecordingTest, ReportsNotFullWhenNotFull){

    ASSERT_EQ(sr->isFull(), false);
}

TEST_F (SoundRecordingTest, NotFullAfterClear){

    float sourceData[dataCapacity];
    sr->write(sourceData, dataCapacity);
    sr->clear();
    ASSERT_EQ(sr->isFull(), false);
}

TEST_F (SoundRecordingTest, ReadReturnsZeroAfterInit){
    int framesRead = sr->read(targetData, 1);
    ASSERT_EQ(0, framesRead);
}

TEST_F (SoundRecordingTest, ReadReturnsValidNumber){
    sr->write(sourceData, 10);
    int framesRead = sr->read(targetData, 10);
    ASSERT_EQ(framesRead, 10);
}

TEST_F (SoundRecordingTest, ReadReturnsLessWhenNotEnoughData){
    sr->write(sourceData, 5);
    int framesRead = sr->read(targetData, 10);
    ASSERT_EQ(framesRead, 5);
}

TEST_F (SoundRecordingTest, ReadAfterReset){
    sr->write(sourceData, 5);
    sr->read(targetData, 5);
    ASSERT_EQ(targetData[0], sourceData[0]);
    sr->resetPlayHead();
    sr->read(targetData, 5);
    ASSERT_EQ(targetData[0], sourceData[0]);
}

TEST_F (SoundRecordingTest, CanReadMoreDataAfterEnablingLoopMode){

    sr->write(sourceData, 5);
    sr->setLooping(true);
    int framesRead = sr->read(targetData, 10);
    ASSERT_EQ(framesRead, 10);
}

TEST_F (SoundRecordingTest, ReadingDataWrapsInLoopMode){

    constexpr int numFramesToWrite = 5;
    constexpr int numFramesToRead = 10;

    sr->write(sourceData, numFramesToWrite);
    sr->setLooping(true);
    sr->read(targetData, numFramesToRead);

    for (int i = 0; i < numFramesToRead; ++i) {
        ASSERT_EQ(targetData[i], sourceData[i%numFramesToWrite]) << "Failed at i=" << i;
    }
}

TEST_F (SoundRecordingTest, StereoRecordingReportsCorrectChannelCount){

    ASSERT_EQ(2, sr2->getChannelCount());
}

TEST_F (SoundRecordingTest, StereoRecordingLength){

    sr2->write(sourceData, 5); // 10 samples = 5 stereo frames
    ASSERT_EQ(5, sr2->getLength());
}

TEST_F (SoundRecordingTest, StereoRecordingRead){

    sr2->write(sourceData, 5);
    int framesRead = sr2->read(targetData, 5);

    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(sourceData[i], targetData[i]) << "Failed at i=" << i;
    }

    ASSERT_EQ(framesRead, 5);
}

//TEST_F (SoundRecordingTest,)