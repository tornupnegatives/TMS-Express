#include "AudioBuffer.h"
#include "debug.h"
#include "PitchEstimator.h"

int main() {
    AudioBuffer buffer = AudioBuffer("../test/one.wav", 8000, 25);
    PitchEstimator pitchEstimator = PitchEstimator(8000);

    for (int i = 0; i < buffer.numSegments; i++) {
        int size;
        float *segment = buffer.segment(i, &size);
        int pitchPeriod = pitchEstimator.estimatePeriod(segment, size);
        DEBUG("Segment %d period:\t%d\n", i, pitchPeriod);
    }



    return 0;
}
