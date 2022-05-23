//
// Created by Joseph Bellahcen on 5/15/22.
//

#ifndef TMS_EXPRESS_PULSETRAINGENERATOR_H
#define TMS_EXPRESS_PULSETRAINGENERATOR_H

#include <vector>
#include "Pitch_Estimation/RundownCircuit.h"

using namespace std;

class PulseTrainGenerator {
public:
    PulseTrainGenerator();

    enum MeasurementType {
        MAX_PEAK = 0,
        MIN_PEAK = 3,
    };

    void reset();
    void update(float peak, int loc, MeasurementType type);
    const vector<vector<int>> getEstimateMatrix();

private:
    vector<vector<int>> locs;
    vector<vector<float>> measurements;
    vector<RundownCircuit> rundowns;
};

#endif //TMS_EXPRESS_PULSETRAINGENERATOR_H
