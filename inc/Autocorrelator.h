//
// Created by Joseph Bellahcen on 4/11/22.
//

#ifndef TMS_EXPRESS_AUTOCORRELATOR_H
#define TMS_EXPRESS_AUTOCORRELATOR_H

class Autocorrelator {
public:
    Autocorrelator();
    ~Autocorrelator();

    void autocorrelation(float *samples, int size);
    float *result(int *size);
    float *normalizedResult(int *size);

private:
    float *xcorr;
    float *normalizedXcorr;
    int size;
};

#endif //TMS_EXPRESS_AUTOCORRELATOR_H
