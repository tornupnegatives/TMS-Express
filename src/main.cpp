#include "AudioBuffer.h"
#include "Autocorrelator.h"
#include "CoefficientSolver.h"
#include "debug.h"
#include "PitchEstimator.h"

int main() {
    AudioBuffer buffer = AudioBuffer("../test/one.wav", 8000, 25);
    PitchEstimator pitchEstimator = PitchEstimator();
    Autocorrelator autocorrelator = Autocorrelator();

    // Audio preprocessing
    // Step 1. Pre-emphasize buffer

    // Step 2. Filter

    // Encode LPC frames for TMS5220
    for (int i = 0; i < buffer.numSegments; i++) {
        // Step 1. Segment buffer
        int size;
        float *segment = buffer.segment(i, &size);

        // Step 2. Apply Hamming window

        // Step 3. Compute normalized autocorrelation
        autocorrelator.autocorrelation(segment, size);
        float *xcorr = autocorrelator.normalizedResult(&size);

        // Step 4. Estimate pitch via autocorrelation
        int pitchPeriod = pitchEstimator.estimatePeriod(xcorr, size);

        // Step 5. Compute energy

        // Step 5. Compute LPC coefficients

        // Step 6. Reflect LPC coefficients via TMS5220 lattice filter
        if (i == 13)
            float *null = reflectionCoefficients(xcorr, 10);
    }

    // Convert frames to bitstream

    // Export to file

    // Cleanup

    return 0;
}
