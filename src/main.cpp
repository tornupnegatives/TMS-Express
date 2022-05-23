#include "Audio/AudioBuffer.h"
#include "Audio/AudioPreprocessor.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "LPC_Analysis/LowerVocalTractAnalyzer.h"
#include "LPC_Analysis/UpperVocalTractAnalyzer.h"
#include "Pitch_Estimation/PitchEstimator.h"
#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FrameEncoder.h"
#include "Frame_Encoding/FramePostProcessor.h"

#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <cmath>
#include <vector>

void printUsage() {
    printf("usage: tms_express filename\n");
}

void printFrame(Frame *frame, float *samples, int samplesSize, int lpcOrder, int frameID) {
    printf("Frame %d Data:\n", frameID);

    printf("Samples: [");
    for (int i = 0; i < samplesSize; i++)
        printf("%f ", samples[i]);
    printf("\b]\n\n");

    printf("Pitch: %d\n", frame->getQuantizedPitch());
    printf("Voicing: %d\n", frame->getQuantizedVoicing());
    printf("Gain: %d\n", frame->getQuantizedGain());

    printf("Coeff: [");
    int *coeff = frame->getQuantizedCoefficients();
    for (int i = 0; i < lpcOrder - 1; i++)
        printf("%d ", coeff[i]);
    printf("\b]\n\n");

    free(coeff);
}

void packFrames(Frame **frames, AudioBuffer *buffer, int order) {
    // Preprocess audio
    auto preprocessor = AudioPreprocessor(buffer);
    //preprocessor.preEmphasize();
    //preprocessor.lowpassFilter(48000);
    //preprocessor.highpassFilter(300);
    //preprocessor.hammingWindow();

    // Prepare segmentation and framing
    int samplesPerSegment = buffer->getSamplesPerSegment();
    int numSegments = buffer->getNumSegments();

    // Prepare LPC analysis structures
    auto autocorrelator = Autocorrelator(samplesPerSegment);
    auto lowerTract = LowerVocalTractAnalyzer(samplesPerSegment);
    auto upperTract = UpperVocalTractAnalyzer(samplesPerSegment, order);

    auto pitchEstimator = PitchEstimator();

    // Pack frames
    for (int i = 0; i < numSegments; i++) {
        // Step 1. Get buffer segment
        float *segment = buffer->getSegment(i);

        // Step N. Apply lowpass filter for pitch estimation
        float *lowpassedSegment = (float *) malloc(samplesPerSegment * sizeof(float));
        memcpy(lowpassedSegment, segment, samplesPerSegment);
        preprocessor.lowpassFilter(lowpassedSegment, 300);

        // After pitch estimation, apply preemphasis and hamming window
        preprocessor.preEmphasize(segment);
        preprocessor.lowpassFilter(segment, 48000);
        preprocessor.highpassFilter(segment, 300);
        preprocessor.hammingWindow(segment);

        // Step 2. Compute normalized autocorrelation
        float *xcorr = autocorrelator.autocorrelation(segment);
        float energy = autocorrelator.energy(segment);




        // Step 3. Estimate pitch period
        auto segmentVector = vector<float>(lowpassedSegment, lowpassedSegment + samplesPerSegment);
        int pitch = lowerTract.estimatePitch(xcorr);
        //pitch = pitchEstimator.estimatePitch(segmentVector);
        //pitch = lowerTract.estimatePitch(xcorr);


        // Step 5. Compute LPC coefficients
        float error;
        float *coeff = upperTract.lpcCoefficients(xcorr, &error);

        // Step 6. Compute energy
        float gain = upperTract.gain(error);

        // Step 4. Determine voicing
        Voicing voicing = lowerTract.detectVoicing(segment, energy, xcorr, gain, pitch);
        int coincidence;
        pitch = pitchEstimator.estimatePitch(segmentVector, &coincidence);

        float differncedEnegy = 0.0f;
        for (int i = 1; i < samplesPerSegment; i++)
            differncedEnegy += pow(segment[i] - segment[i - 1], 2);

        differncedEnegy /= energy;

        //voicing = ((10 * coincidence) - differncedEnegy - 109) > 0;

           //voicing = UNVOICED;

        // Step 7. Store parameters in frame
        frames[i] = new Frame(order, pitch, (int) voicing, coeff, gain);

        printFrame(frames[i], segment, samplesPerSegment, 10, i);

        // Step 8. Cleanup
        free(xcorr);
        free(coeff);
    }

    auto framePostprocessor = FramePostProcessor(frames, numSegments);
    framePostprocessor.normalizeGain();
    //framePostprocessor.shiftGain(2);
}

int main(int argc, char **argv) {
    // Argument parsing
    if (argc == 1) {
        printUsage();
        exit(EXIT_FAILURE);
    }

    // Ensure file exists
    char *filepath = argv[1];
    struct stat file;
    if (stat(filepath, &file) != 0) {
        printf("Specified file does not exist: %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    // Import audio into segmented buffer
    auto buffer = AudioBuffer(filepath, 8000, 25);
    int numFrames = buffer.getNumSegments();

    // Perform LPC analysis and framing
    Frame **frames = (Frame **) malloc(sizeof(Frame *) * numFrames);
    packFrames(frames, &buffer, 11);

    // Write to file
    auto frameEncoder = FrameEncoder(frames, numFrames);
    frameEncoder.serialize(argv[2]);
}
