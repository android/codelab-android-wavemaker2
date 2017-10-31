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

#include "AudioEngine.h"
#include "SoundRecordingUtilities.h"
#include <aaudio/AAudio.h>
#include <android/log.h>

aaudio_data_callback_result_t recordingCallback(
        AAudioStream __unused *stream,
        void *userData,
        void *audioData,
        int32_t numFrames) {

    return ((AudioEngine *) userData)->recordingCallback(audioData, numFrames);
}

aaudio_data_callback_result_t playbackCallback(
        AAudioStream __unused *stream,
        void *userData,
        void *audioData,
        int32_t numFrames) {

    return ((AudioEngine *) userData)->playbackCallback(audioData, numFrames);
}

AudioEngine::AudioEngine() {
    mSoundRecording.setLooping(true);
}

void AudioEngine::start() {

    AAudioStreamBuilder *playbackBuilder;

    // Create the playback stream.
    AAudio_createStreamBuilder(&playbackBuilder);
    AAudioStreamBuilder_setPerformanceMode(playbackBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setSharingMode(playbackBuilder, AAUDIO_SHARING_MODE_EXCLUSIVE);
    AAudioStreamBuilder_setFormat(playbackBuilder, AAUDIO_FORMAT_PCM_FLOAT);
    AAudioStreamBuilder_setChannelCount(playbackBuilder, kChannelCountStereo);
    AAudioStreamBuilder_setDataCallback(playbackBuilder, ::playbackCallback, this);

    aaudio_result_t result = AAudioStreamBuilder_openStream(playbackBuilder, &mPlaybackStream);
    AAudioStreamBuilder_delete(playbackBuilder);

    if (result != AAUDIO_OK){
        __android_log_print(ANDROID_LOG_DEBUG, "AudioEngine::startEngine()",
                            "Error opening playback stream %s",
                            AAudio_convertResultToText(result));
        return;
    }

    // Obtain the sample rate from the playback stream so we can request the same sample rate from
    // the recording stream.
    int32_t sampleRate = AAudioStream_getSampleRate(mPlaybackStream);

    result = AAudioStream_requestStart(mPlaybackStream);
    if (result != AAUDIO_OK){
        __android_log_print(ANDROID_LOG_DEBUG, "AudioEngine::startEngine()",
                            "Error starting playback stream %s",
                            AAudio_convertResultToText(result));
        return;
    }

    // Create the recording stream.
    AAudioStreamBuilder *recordingBuilder;
    AAudio_createStreamBuilder(&recordingBuilder);
    AAudioStreamBuilder_setDirection(recordingBuilder, AAUDIO_DIRECTION_INPUT);
    AAudioStreamBuilder_setPerformanceMode(recordingBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setFormat(recordingBuilder, AAUDIO_FORMAT_PCM_I16);
    AAudioStreamBuilder_setSampleRate(recordingBuilder, sampleRate);
    AAudioStreamBuilder_setChannelCount(recordingBuilder, kChannelCountMono);
    AAudioStreamBuilder_setDataCallback(recordingBuilder, ::recordingCallback, this);

    result = AAudioStreamBuilder_openStream(recordingBuilder, &mRecordingStream);
    AAudioStreamBuilder_delete(recordingBuilder);

    if (result != AAUDIO_OK){
        __android_log_print(ANDROID_LOG_DEBUG, __func__,
                            "Error opening recording stream %s",
                            AAudio_convertResultToText(result));
        return;
    }

    // Allocate memory for the conversion buffer now we know what our maximum buffer size is.
    mConversionBuffer = std::make_unique<float[]>(
            (size_t) AAudioStream_getBufferCapacityInFrames(mRecordingStream)
            * kChannelCountStereo);

    result = AAudioStream_requestStart(mRecordingStream);
    if (result != AAUDIO_OK){
        __android_log_print(ANDROID_LOG_DEBUG, __func__,
                            "Error starting recording stream %s",
                            AAudio_convertResultToText(result));
        return;
    }


}

void AudioEngine::stop() {

    stopStream(mPlaybackStream);
    closeStream(mPlaybackStream);
    stopStream(mRecordingStream);
    closeStream(mRecordingStream);
}

aaudio_data_callback_result_t AudioEngine::recordingCallback(void *audioData,
                                                             int32_t numFrames) {

    if (mIsRecording.load()) {

        convertArrayInt16ToFloat(static_cast<int16_t *>(audioData),
                                 mConversionBuffer.get(), numFrames * kChannelCountMono);
        convertArrayMonoToStereo(mConversionBuffer.get(), numFrames);
        mSoundRecording.write(mConversionBuffer.get(), numFrames);
    }

    if (mSoundRecording.isFull()) {
        mIsRecording.store(false);
    }

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

aaudio_data_callback_result_t AudioEngine::playbackCallback(void *audioData, int32_t numFrames) {

    float *audioDataFloat = static_cast<float *>(audioData);

    if (mIsPlaying.load()) {
        int32_t framesRead = mSoundRecording.read(audioDataFloat, numFrames);

        if (framesRead != numFrames) {

            // Pad the remaining samples with zeros (silence).
            float *firstEmptySample = audioDataFloat + (framesRead * kChannelCountStereo);
            int32_t length = (numFrames - framesRead) * kChannelCountStereo;
            fillArrayWithZeros(firstEmptySample, length);

            mIsPlaying.store(false);
            mSoundRecording.resetPlayHead();
        }
    } else {
        // Fill whole buffer with zeros.
        fillArrayWithZeros(audioDataFloat, numFrames * kChannelCountStereo);
    }

    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void AudioEngine::setRecording(bool isRecording) {
    if (mIsRecording) mSoundRecording.resetWriteHead();
    mIsRecording.store(isRecording);
}

void AudioEngine::setPlaying(bool isPlaying) {
    if (isPlaying) {
        mSoundRecording.resetPlayHead();
        __android_log_print(ANDROID_LOG_DEBUG, "AudioEngine", "Playing sample, length %d",
                            mSoundRecording.getLength());
    }
    mIsPlaying.store(isPlaying);
}

void AudioEngine::stopStream(AAudioStream *stream) const {

    if (stream != nullptr){
        aaudio_result_t result = AAudioStream_requestStop(stream);
        if (result != AAUDIO_OK){
            __android_log_print(ANDROID_LOG_DEBUG, __func__, "Error stopping stream %s",
                                AAudio_convertResultToText(result));
        }
    }
}

void AudioEngine::closeStream(AAudioStream *stream) const {

    if (stream != nullptr){
        aaudio_result_t result = AAudioStream_close(stream);
        if (result != AAUDIO_OK){
            __android_log_print(ANDROID_LOG_DEBUG, __func__, "Error closing stream %s",
                                AAudio_convertResultToText(result));
        }
    }
}



