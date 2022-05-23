//
// Created by Joseph Bellahcen on 4/16/22.
//

#ifndef TMS_EXPRESS_PITCHESTIMATOR_H
#define TMS_EXPRESS_PITCHESTIMATOR_H

#include "Pitch_Estimation/PulseTrainGenerator.h"
#include "Pitch_Estimation/RundownCircuit.h"

#include <vector>

using namespace std;

#define VOICED true
#define UNVOICED false

typedef bool Voicing;

class PitchEstimator {
public:

    explicit PitchEstimator(float windowWidth = 22.5, int sampleRate = 8000, int minPitchHz = 50, int maxPitchHz = 500, float unvoicedThreshold = 0.3);

    int estimatePitch(vector<float> segment, int *coincidence);
    Voicing detectVoicing(float segment, float energy, float *xcorr, float gain, int pitchPeriod);

private:
    int minPitchPeriod;
    int maxPitchPeriod;
    float unvoicedThreshold;

    PulseTrainGenerator pulseTrainGenerator;

    const vector<int> biases = {1, 2, 5, 7};
    vector<int> windowThresholds;
    vector<vector<int>> coincidenceWindows;

    void detectPeaks(vector<float> segment);
    void updatePulseTrains(vector<float> segment);
};

#endif //TMS_EXPRESS_PITCHESTIMATOR_H
