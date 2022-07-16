//
// Created by Joseph Bellahcen on 7/16/22.
// TODO: Documentation
//

#include "Audio/AudioBuffer.h"
#include "Audio/AudioPreprocessor.h"
#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FrameEncoder.h"
#include "Frame_Encoding/FramePostprocessor.h"
#include "Interfaces/Encoder.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "LPC_Analysis/LinearPredictor.h"
#include "LPC_Analysis/PitchEstimator.h"
#include <fstream>
#include <iostream>
#include <vector>


Encoder::Encoder(float windowMs, int highpassHz, int lowpassHz, float preemphasis, bool cStyle, char separator,
                 bool includeStopFrame, int gainShift, float maxVoicedDb, float maxUnvoicedDb, bool detectRepeats,
                 int maxHz, int minHz) : windowMs(windowMs), highpassHz(highpassHz), lowpassHz(lowpassHz),
                                         preemphasisAlpha(preemphasis), cStyle(cStyle), separator(separator),
                                         includeStopFrame(includeStopFrame), gainShift(gainShift),
                                         maxVoicedDB(maxVoicedDb), maxUnvoicedDB(maxUnvoicedDb),
                                         detectRepeats(detectRepeats), maxHz(maxHz), minHz(minHz) {}

void Encoder::encode(const std::string &inputPath, const std::string &outputPath) {
    // Perform LPC analysis and convert audio data to a bitstream
    auto bitstream = analyzeAudio(inputPath);

    // Either export the bitstream as a string for testing or as a C array for embedded development
    if (cStyle) {
        ;
    }

    // Write bitstream to disk
    std::ofstream lpcOut;
    lpcOut.open(outputPath);
    lpcOut << bitstream;
    lpcOut.close();
}

bool Encoder::inputFileExists(const std::string &inputPath) {
    FILE *file = fopen(inputPath.c_str(), "r");
    bool fileExists = (file != nullptr);

    fclose(file);
    return fileExists;
}

std::string Encoder::analyzeAudio(const std::string &inputPath) const {
    // Check if file exists
    if (!inputFileExists(inputPath)) {
        std::cerr << "Error: could not open audio file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Mix audio to 8kHz mono and store in a segmented buffer
    auto lpcBuffer = AudioBuffer(inputPath, 8000, windowMs);

    // Copy the buffer so that upper and lower vocal tract analysis may occur separately
    auto pitchBuffer = AudioBuffer(lpcBuffer);

    // Preprocess the buffers
    //
    // The pitch buffer will ONLY be lowpass-filtered, as pitch is a low-frequency component of the signal. Neither
    // highpass filtering nor preemphasis, which exaggerate high-frequency components, will improve pitch estimation
    auto preprocessor = AudioPreprocessor();
    preprocessor.applyBiquad(pitchBuffer, lowpassHz, AudioPreprocessor::FILTER_LOWPASS);
    preprocessor.applyBiquad(lpcBuffer, highpassHz, AudioPreprocessor::FILTER_HIGHPASS);
    preprocessor.applyPreemphasis(lpcBuffer, preemphasisAlpha);

    // Extract buffer metadata
    //
    // Only the LPC buffer is queried for metadata, since it will have the same number of samples as the pitch buffer.
    // The sample rate of the buffer is extracted despite being known, as future iterations of TMS Express may support
    // encoding 10kHz/variable sample rate audio for the TMS5200C
    auto nSegments = lpcBuffer.getNSegments();
    auto sampleRate = lpcBuffer.getSampleRate();

    // Initialize analysis objects and data structures
    auto linearPredictor = LinearPredictor();
    auto pitchEstimator = PitchEstimator(int(sampleRate), minHz, maxHz);
    auto frames = std::vector<Frame>();

    for (int i = 0; i < nSegments; i++) {
        // Get segment for frame
        auto pitchSegment = pitchBuffer.getSegment(i);
        auto lpcSegment = lpcBuffer.getSegment(i);

        // Apply a window function to the segment to smoothen its boundaries
        //
        // Because information about the transition between adjacent frames is lost during segmentation, a window will
        // help produce smoother results
        preprocessor.applyHammingWindow(lpcSegment);

        // Compute the autocorrelation of each segment, which serves as the basis of all analysis
        auto lpcAcf = Autocorrelator::process(lpcSegment);
        auto pitchAcf = Autocorrelator::process(pitchSegment);

        // Extract LPC reflector coefficients and compute the predictor gain
        auto coeffs = linearPredictor.reflectorCoefficients(lpcAcf);
        auto gain = linearPredictor.gain();

        // Estimate pitch
        auto pitchPeriod = pitchEstimator.estimatePeriod(pitchAcf);

        // Decide whether the segment is voiced or unvoiced
        auto segmentIsVoiced = coeffs[0] < 0;

        // Store parameters in a Frame object
        auto frame = Frame(pitchPeriod, segmentIsVoiced, gain, coeffs);
        frames.push_back(frame);
    }

    // Apply preprocessing
    auto postProcessor = FramePostprocessor(&frames, maxVoicedDB, maxUnvoicedDB);
    postProcessor.normalizeGain();
    postProcessor.shiftGain(gainShift);

    if (detectRepeats) {
        postProcessor.detectRepeatFrames();
    }

    // Encode frames to hex bitstreams
    auto encoder = FrameEncoder(frames, cStyle, separator);
    auto bitstream = encoder.toHex(includeStopFrame);

    return bitstream;
}
