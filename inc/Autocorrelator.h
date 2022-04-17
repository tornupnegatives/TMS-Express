//
// Created by Joseph Bellahcen on 4/11/22.
//

#ifndef TMS_EXPRESS_AUTOCORRELATOR_H
#define TMS_EXPRESS_AUTOCORRELATOR_H

class Autocorrelator {
public:
    Autocorrelator(int samplesPerSegment);
    float *autocorrelation(float *segment, bool normalize = true);

private:
    int size;
};

#endif //TMS_EXPRESS_AUTOCORRELATOR_H
