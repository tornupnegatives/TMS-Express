///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: BitstreamGenerator
//
// Description: The BitstreamGenerator encapsulates the core functionality of TMS Express in a user-accessible
//              interface. Given an audio file or directory thereof, it will perform LPC analysis and create a bitstream
//              file in the desired format
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Audio/AudioBuffer.h"
#include "Audio/AudioFilter.h"
#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FrameEncoder.h"
#include "Frame_Encoding/FramePostprocessor.h"
#include "Interfaces/BitstreamGenerator.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "LPC_Analysis/LinearPredictor.h"
#include "LPC_Analysis/PitchEstimator.h"
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

BitstreamGenerator::BitstreamGenerator(float windowMs, int highpassHz, int lowpassHz, float preemphasis, EncoderStyle style,
                                       bool includeStopFrame, int gainShift, float maxVoicedDb, float maxUnvoicedDb, bool detectRepeats,
                                       int maxHz, int minHz) : windowMs(windowMs), highpassHz(highpassHz), lowpassHz(lowpassHz),
                                         preemphasisAlpha(preemphasis), style(style),
                                         includeStopFrame(includeStopFrame), gainShift(gainShift),
                                         maxVoicedDB(maxVoicedDb), maxUnvoicedDB(maxUnvoicedDb),
                                         detectRepeats(detectRepeats), maxHz(maxHz), minHz(minHz) {}

// Encode a single audio file
void BitstreamGenerator::encode(const std::string &inputPath, const std::string &inputFilename,
                                const std::string &outputPath) {
    // Perform LPC analysis and convert audio data to a bitstream
    auto frames = generateFrames(inputPath);
    auto bitstream = formatBitstream(frames, inputFilename);

    // Write bitstream to disk
    std::ofstream lpcOut;
    lpcOut.open(outputPath);
    lpcOut << bitstream;
    lpcOut.close();
}

// Encode a batch job into either a single file or a collection of files
void BitstreamGenerator::encodeBatch(const std::vector<std::string> &inputPaths,
                                     const std::vector<std::string> &inputFilenames, const std::string &outputPath) {
    if (style == ENCODERSTYLE_ASCII) {
        // Create directory to populate with encoded files
        std::experimental::filesystem::create_directory(outputPath);

        for (int i = 0; i < inputPaths.size(); i++) {
            const auto& inPath = inputPaths[i];
            const auto& filename = inputFilenames[i];

            std::experimental::filesystem::path outPath = outputPath;
            outPath /= (filename + ".lpc");

            encode(inPath, filename, outPath.string());
        }
    } else {
        std::ofstream lpcOut;
        lpcOut.open(outputPath);

        for (int i = 0; i < inputPaths.size(); i++) {
            const auto& inPath = inputPaths[i];
            const auto& filename = inputFilenames[i];

            auto frames = generateFrames(inPath);
            auto bitstream = formatBitstream(frames, filename);

            lpcOut << bitstream << std::endl;
        }

        lpcOut.close();
    }
}

// Generate a bitstream from a single audio file
std::vector<Frame> BitstreamGenerator::generateFrames(const std::string &inputPath) const {
    // Import 8kHz mono audio
    auto buffer = AudioBuffer(inputPath, windowMs);
    buffer.resample(8000);

    // Apply a lowpass filter to establish the effective bandwidth of the signal
    // A LPF of around 4kHz-5kHz will result in brighter-sounding speech
    auto preprocessor = AudioFilter(8000, highpassHz, lowpassHz, preemphasisAlpha);
    buffer.setSamples(preprocessor.applyLowpass(buffer.getSamples()));

    // Initialize analysis objects and data structures
    auto linearPredictor = LinearPredictor();
    auto pitchEstimator = PitchEstimator(8000, minHz, maxHz);
    auto frames = std::vector<Frame>();
    auto nSegments = buffer.nSegments();

    auto test = preprocessor.applyPreemphasis(buffer.getSamples());
    test = preprocessor.applyHighpass(buffer.getSamples());
    auto newAudio = AudioBuffer(buffer);
    newAudio.setSamples(test);
    newAudio.exportAudio("processed.wav");

    for (int i = 0; i < nSegments; i++) {
        // Get segment for frame
        auto segment = buffer.getSegment(i);

        // Compute autocorrelation of unmodified buffer for pitch estimation
        auto pitchAcf = Autocorrelator::process(segment);

        // Apply pre-emphasis filter, highpass filter, and Hamming window to buffer for LPC analysis
        segment = preprocessor.applyPreemphasis(segment);
        segment = preprocessor.applyHighpass(segment);
        segment = preprocessor.applyHammingWindow(segment);

        // Extract LPC reflector coefficients and predictor gain from the autocorrelation of the preprocessed segment
        auto lpcAcf = Autocorrelator::process(segment);
        auto coeffs = linearPredictor.reflectorCoefficients(lpcAcf);
        auto gain = linearPredictor.gain();

        // Estimate pitch and voicing
        auto pitchPeriod = pitchEstimator.estimatePeriod(pitchAcf);
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

    return frames;
}

// Format raw bitstream for use in various contexts such as C headers
std::string BitstreamGenerator::formatBitstream(const std::vector<Frame>& frames, const std::string &filename) {
    // Encode frames to hex bitstreams
    auto encoder = FrameEncoder(frames, style != ENCODERSTYLE_ASCII, ',');
    std::string bitstream;

    // Either export the bitstream as a string for testing or as a C array for embedded development
    switch(style) {
        case ENCODERSTYLE_ASCII:
            bitstream = encoder.toHex(includeStopFrame);
            break;
        case ENCODERSTYLE_C:
            // C-style bitstreams are headers which contain an integer array of bitstream values
            // Format: const int bitstream_name [] = {<values>};
            bitstream = encoder.toHex(includeStopFrame);
            bitstream = "const int " + filename + "[] = {" + bitstream + "};\n";
            break;
        case ENCODERSTYLE_ARDUINO:
            // Arduino-style bitstreams are C-style bitstreams which include the Arduino header and PROGMEM keyword
            // Format: extern const uint8_t bitstream_name [] PROGMEM = {<values>};
            bitstream = encoder.toHex(includeStopFrame);
            bitstream = "extern const uint8_t " + filename + "[] PROGMEM = {" + bitstream + "};\n";
            break;
        case ENCODERSTYLE_JSON:
            bitstream = encoder.toJSON();
            break;
    }

    return bitstream;
}
