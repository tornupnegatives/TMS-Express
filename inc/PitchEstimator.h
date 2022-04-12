//
// Created by Joseph Bellahcen on 4/6/22.
//

#ifndef TMS_EXPRESS_PITCHESTIMATOR_H
#define TMS_EXPRESS_PITCHESTIMATOR_H

class PitchEstimator {
public:
    int estimatePeriod(float *xcorr, int size);
};

#endif //TMS_EXPRESS_PITCHESTIMATOR_H
