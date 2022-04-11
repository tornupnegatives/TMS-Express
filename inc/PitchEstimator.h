//
// Created by Joseph Bellahcen on 4/6/22.
//

#ifndef TMS_EXPRESS_PITCHESTIMATOR_H
#define TMS_EXPRESS_PITCHESTIMATOR_H

class PitchEstimator {
public:
    PitchEstimator(int sampleRate);
    int estimatePeriod(float *segment, int size);

private:
    int sampleRate;
    float *autocorrelation(float *samples, int size, int lag);
};

#endif //TMS_EXPRESS_PITCHESTIMATOR_H
