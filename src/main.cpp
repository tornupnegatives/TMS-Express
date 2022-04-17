#include "AudioBuffer.h"
#include "AudioPreprocessor.h"
#include "Autocorrelator.h"
#include "LowerVocalTractAnalyzer.h"

int main() {
    auto buffer = AudioBuffer("../test/one.wav", 8000, 25);

    // Audio preprocessing
    AudioPreprocessor preprocessor = AudioPreprocessor(&buffer);
    preprocessor.preEmphasize();
    preprocessor.lowpassFilter(3400);
    preprocessor.highpassFilter(300);
    preprocessor.hammingWindow();

    // Analysis structures
    int samplesPerSegment = buffer.getSamplesPerSegment();
    int numSegments = buffer.getNumSegments();

    auto autocorrelator = Autocorrelator(samplesPerSegment);
    auto lowerVocalTractAnalyzer = LowerVocalTractAnalyzer(numSegments, samplesPerSegment);

    // Encode LPC frames for TMS5220
    for (int i = 0; i < numSegments; i++) {
        // Step 1. Segment buffer
        int size;
        float *segment = buffer.getSegment(i, &size);

        // Step 2. Compute normalized autocorrelation
        float *xcorr = autocorrelator.autocorrelation(segment, true);

        // Step 3. Estimate pitch via autocorrelation
        lowerVocalTractAnalyzer.estimatePitch(i, xcorr);

        // Step 4. Compute energy

        // Step 5. Compute LPC coefficients

        // Step 6. Reflect LPC coefficients via TMS5220 lattice filter

        // Step 7. Cleanup
        free(xcorr);
    }

    int *pitches = lowerVocalTractAnalyzer.getPitches();
    for (int i = 0; i < numSegments; i++)
        printf("%d\n",pitches[i]);

    // Convert frames to bitstream

    // Export to file

    // Cleanup

    return 0;
}
