#include "Audio/AudioBuffer.h"
#include "Audio/AudioPreprocessor.h"
#include "LPC_Analysis/Autocorrelator.h"
#include "LPC_Analysis/LowerVocalTractAnalyzer.h"
#include "LPC_Analysis/UpperVocalTractAnalyzer.h"
#include "Frame_Encoder/Frame.h"

#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>

void printUsage() {
    printf("usage: tms_express filename\n");
}

void packFrames(Frame **frames, AudioBuffer *buffer, int order) {
    // Preprocess audio
    auto preprocessor = AudioPreprocessor(buffer);
    preprocessor.preEmphasize();
    preprocessor.lowpassFilter(3400);
    preprocessor.highpassFilter(300);
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
        LowerVocalTractAnalyzer::voicing voicing = lowerTract.detectVoicing(pitch, xcorr[0]);

        // Step 5. Compute LPC coefficients
        float *coeff = upperTract.lpcCoefficients(xcorr);

        // Step 6. Compute energy
        float energy = upperTract.energy(segment);

        // Step 7. Store parameters in frame
        frames[i] = new Frame(order, pitch, (int) voicing, coeff, energy);

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
    if (stat (filepath, &file) != 0) {
        printf("Specified file does not exist: %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    // Pack frames
    auto buffer = AudioBuffer(filepath, 8000, 25);
    Frame **frames = (Frame **) malloc(sizeof(Frame *) * buffer.getNumSegments());
    packFrames(frames, &buffer, 11);

    // Cleanup
    for (int i = 0; i < buffer.getNumSegments(); i++)
        delete frames[i];

    free(frames);
}

/*
void debugFrame(int i) {
    if (i == 13) {
        printf("\nSegment 13\n");

        printf("Samples: [");
        for (int j = 0; j < samplesPerSegment; j++)
            printf("%f, ", segment[j]);
        printf("%f]\n", segment[samplesPerSegment - 1]);

        printf("Xcorr: [");
        for (int j = 0; j < samplesPerSegment; j++)
            printf("%f, ", xcorr[j]);
        printf("%f]\n", xcorr[samplesPerSegment - 1]);

        printf("Pitch: %d\n", pitch);
        printf("Voicing: %d\n", voicing);
        printf("Energy: %f\n", energy);

        printf("Coeff: [");
        for (int j = 1; j < order; j++)
            printf("%f, ", coeff[j]);
        printf("%f]\n", coeff[order]);
    }
}
 */
