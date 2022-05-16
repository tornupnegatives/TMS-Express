//
// Created by Joseph Bellahcen on 5/15/22.
//

#include "Pitch_Estimation/RundownCircuit.h"

#include <cmath>

// Initialize a new RundownCircuit with default parameters
RundownCircuit::RundownCircuit(int sampleRateHz, float windowWidthMs) {
    sampleRate = sampleRateHz;
    averagePeriod = sampleRateHz * 22.5 * 1e-3;
    blankingInterval = sampleRateHz * (22.5 / 2) * 1e-4;
    decayConstant = averagePeriod / 0.695;

    lastLoc = 0;
    lastPeak = 0.0f;
}

void RundownCircuit::reset() {
    averagePeriod = sampleRate * 22.5 * 1e-3;
    blankingInterval = sampleRate * (22.5 / 2) * 1e-4;
    decayConstant = averagePeriod / 0.695;

    lastLoc = 0;
    lastPeak = 0.0f;
}

bool RundownCircuit::advance(float peak, int loc) {
    int dt = loc - lastLoc;

    // If the peak is within the blanking interval, it is likely spurious. Similarly, peaks which are outside the
    // blanking interval but whose magnitudes do not exceed the decay threshold are probably not relevant
    if (dt < blankingInterval || peak < decayThreshold(dt)) {
        return false;
    } else {
        updatePeriod(dt, loc);
        lastPeak = peak;
        return true;
    }
}

void RundownCircuit::updatePeriod(int newPeriod, int loc) {
    averagePeriod = (averagePeriod + newPeriod) / 2;
    blankingInterval = 0.4 * averagePeriod;
    decayConstant = averagePeriod / 0.695;

    lastLoc = loc;
}

float RundownCircuit::decayThreshold(int dt) {
    if (lastLoc == 0) {
        return 0;
    }

    return (float) lastPeak * exp(-dt / decayConstant);
}