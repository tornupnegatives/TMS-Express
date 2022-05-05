#include "Audio/AudioBuffer.h"
#include "Audio/AudioPreprocessor.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "LPC_Analysis/LowerVocalTractAnalyzer.h"
#include "LPC_Analysis/UpperVocalTractAnalyzer.h"
#include "Frame_Encoding/Frame.h"
#include "Frame_Encoding/FrameEncoder.h"

#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>

void printUsage() {
    printf("usage: tms_express filename\n");
}

void printFrame(Frame *frame, float *samples, int samplesSize, int lpcOrder, int frameID) {
    printf("Frame %d Data:\n", frameID);

    printf("Pitch: %d\n", frame->getQuantizedPitch());
    printf("Voicing: %d\n", frame->getQuantizedVoicing());
    printf("Energy: %d\n", frame->getQuantizedEnergy());

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
    preprocessor.preEmphasize();
    preprocessor.lowpassFilter(3800);
    preprocessor.highpassFilter(3800);
    preprocessor.hammingWindow();

    // Prepare segmentation and framing
    int samplesPerSegment = buffer->getSamplesPerSegment();
    int numSegments = buffer->getNumSegments();

    // Prepare LPC analysis structures
    auto autocorrelator = Autocorrelator(samplesPerSegment);
    auto lowerTract = LowerVocalTractAnalyzer(numSegments, samplesPerSegment);
    auto upperTract = UpperVocalTractAnalyzer(samplesPerSegment, order);

    // Pack frames
    for (int i = 0; i < numSegments; i++) {
        // Step 1. Get buffer segment
        float *segment = buffer->getSegment(i);

        // Step 2. Compute normalized autocorrelation
        float *xcorr = autocorrelator.autocorrelation(segment);

        // Step 3. Estimate pitch period
        int pitch = lowerTract.estimatePitch(xcorr);

        // Step 4. Determine voicing
        LowerVocalTractAnalyzer::voicing voicing = lowerTract.detectVoicing(pitch, xcorr);

        // Step 5. Compute LPC coefficients
        float loss;
        float *coeff = upperTract.lpcCoefficients(xcorr, &loss);

        // Step 6. Compute energy
        float energy = upperTract.gain(segment, loss);

        // Step 7. Store parameters in frame
        frames[i] = new Frame(order, pitch, (int) voicing, coeff, energy);

        printFrame(frames[i], segment, samplesPerSegment, 11, i);

        // Step 8. Cleanup
        free(xcorr);
        free(coeff);
    }
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

    // Pack frames
    auto buffer = AudioBuffer(filepath, 8000, 25);
    int numFrames = buffer.getNumSegments();

    Frame **frames = (Frame **) malloc(sizeof(Frame *) * numFrames);
    packFrames(frames, &buffer, 11);

    // Write to file
    auto frameEncoder = FrameEncoder(frames, numFrames);
    frameEncoder.serialize(argv[2]);
}
