//
// Created by Joseph Bellahcen on 5/15/22.
//

#ifndef TMS_EXPRESS_PULSETRAINGENERATOR_H
#define TMS_EXPRESS_PULSETRAINGENERATOR_H

#define MAXPEAK true
#define MINPEAK false

#include <vector>

using namespace std;


class PulseTrainGenerator {
public:
    enum PulseTrainModes {
        PULSETRAIN_MODE_AMPLITUDE,
        PULSETRAIN_MODE_MAXTOMIN,
        PULSETRAIN_MODE_PEAKTOPEAK
    };

    PulseTrainGenerator();

    void reset();
    void update(float peak, int loc, bool type);

private:
    vector<float> amplitudes;
    vector<float> maxToMinDistances;
    vector<float> peakToPeakDistances;
    vector<int> peakLocs;

    int getLastMaxLoc();
    int getLastMinLoc();
};

#endif //TMS_EXPRESS_PULSETRAINGENERATOR_H
