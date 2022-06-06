#include "Audio/AudioBuffer.h"
#include "Audio/AudioPreprocessor.h"
#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FrameEncoder.h"
#include "Frame_Encoding/FramePostprocessor.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "LPC_Analysis/PitchEstimator.h"
#include "LPC_Analysis/LinearPredictor.h"
#include "UserParameters.h"

#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
    // Parse arguments
    auto params = UserParameters(argc, argv);

    // Import audio file and mix to 8kHz mono
    const std::string &inPath = params.getInputPath();
    AudioBuffer buffer = AudioBuffer(inPath, 8000, params.getWindowWidthMs());

    // Prepare separate analysis buffer for pitch estimation
    auto lpcBuffer = buffer;
    auto pitchBuffer = AudioBuffer(buffer);

    // Perform preprocessing on each buffer
    //
    // Because pitch is a low-frequency component of the signal, the pitch buffer will undergo lowpass filtering.
    // Conversely, a preemphasis filter will be applied to the LPC buffer to bring out the high-frequency components
    AudioPreprocessor preprocessor = AudioPreprocessor();
    preprocessor.applyBiquad(pitchBuffer, params.getLowpassFilterCutoffHz(), AudioPreprocessor::FILTER_LOWPASS);
    preprocessor.applyBiquad(lpcBuffer, params.getHighpassFilterCutoffHz(), AudioPreprocessor::FILTER_HIGHPASS);
    preprocessor.applyPreemphasis(lpcBuffer, params.getPreemphasisAlpha());

    // Extract buffer metadata
    unsigned int nSegments = lpcBuffer.getNSegments();
    unsigned int sampleRate = lpcBuffer.getSampleRate();

    // Analysis structures
    PitchEstimator pitchEstimator = PitchEstimator(sampleRate, params.getMinFrqHz(), params.getMaxFrqHz());
    LinearPredictor linearPredictor = LinearPredictor();

    std::vector<Frame> frames = std::vector<Frame>();

    for (int i = 0; i < nSegments; i++) {
        // Get segment for frame
        auto pitchSegment = pitchBuffer.getSegment(i);
        auto lpcSegment = lpcBuffer.getSegment(i);

        // Apply a window to the segment to smoothen boundaries, since information about the transition between
        // segments is lost during the slicing process. This windowed signal will be used to compute the biased
        // autocorrelation function, which is the basis of LPC analysis
        preprocessor.applyHammingWindow(lpcSegment);
        auto lpcAcf = Autocorrelator::process(lpcSegment);
        auto pitchAcf = Autocorrelator::process(pitchSegment);

        // Estimate pitch
        unsigned int pitchPeriod = pitchEstimator.estimatePeriod(pitchAcf);

        // Extract LPC coefficients and prediction gain
        auto coeffs = linearPredictor.reflectorCoefficients(lpcAcf);
        auto gain = linearPredictor.gain();

        // Detect voicing
        auto isVoiced = coeffs[0] < 0;

        // Store parameters in frame
        Frame frame = Frame(pitchPeriod, isVoiced, gain, coeffs);
        frames.push_back(frame);
        frame.print(i + 1);
    }

    // Apply post-processing and serialize Frame data
    auto postProcessor = FramePostprocessor(&frames, params.getMaxVoicedGainDb(), params.getMaxUnvoicedGainDb());
    postProcessor.normalizeGain();
    postProcessor.shiftGain(1);

    auto encoder = FrameEncoder(frames, params.getIncludeHexPrefix(), params.getHexStreamSeparator());
    auto frameBin = encoder.toHex(params.getShouldAppendStopFrame());

    // Save output to file
    std::ofstream lpcOut;
    lpcOut.open(params.getOutputLpcPath());
    lpcOut << frameBin;
    lpcOut.close();
}