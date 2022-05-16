//
// Created by Joseph Bellahcen on 5/15/22.
//

#include "Pitch_Estimation/PulseTrainGenerator.h"

#include <vector>

PulseTrainGenerator::PulseTrainGenerator() {
    // Pulse train vectors
    //
    // Although the Gold-Rabiner algorithm specifies six independent pulse trains, three each for maxima and minima,
    // this implementation opts to combine the categories into three unified pulse trains. Peak types are tracked using
    // the peakLocs vector
    amplitudes = vector<float>(1, 0);
    maxToMinDistances = vector<float>(1, 0);
    peakToPeakDistances = vector<float>(1, 0);

    // Indices of each extremum
    //
    // Rather than construct a true pulse train, the PulseTrainGenerator stores the indices of the segment at which
    // each peak occurs. This eliminates the need to store many empty vector elements, as well as simplifies the
    // computation of pulse period during later steps of pitch estimation. Indices will be passed to the
    // PulseTrainGenerator as positive for maxima and negative for minima
    peakLocs = vector<int>(1, 0);
}

// Reset the PulseTrainGenerator to its initialization state
void PulseTrainGenerator::reset() {
    amplitudes = vector<float>(1, 0);
    maxToMinDistances = vector<float>(1, 0);
    peakToPeakDistances = vector<float>(1, 0);
    peakLocs = vector<int>(1, 0);
}

// Update the pulse train vectors with information about the given peak
void PulseTrainGenerator::update(float peak, int loc, bool type) {
    int lastMaxLoc = getLastMaxLoc();
    int lastMinLoc = getLastMinLoc();

    float amplitude = abs(peak);
    float maxToMin = abs(peak - ((type == MAXPEAK) ? amplitudes.at(lastMinLoc) : amplitudes.at(lastMaxLoc)));
    float peakToPeak = peak - ((type == MAXPEAK) ? amplitudes.at(lastMaxLoc) : amplitudes.at(lastMinLoc));
    int peakLoc = (type == MAXPEAK) ? loc : -loc;

    if (peakToPeak < 0.0f) {
        peakToPeak = 0.0f;
    }

    amplitudes.push_back(amplitude);
    maxToMinDistances.push_back(maxToMin);
    peakToPeakDistances.push_back(peakToPeak);
    peakLocs.push_back(peakLoc);
}

// Return the index of the last maximum peak
int PulseTrainGenerator::getLastMaxLoc() {
    for (int i = peakLocs.size() - 1; i >= 0; i--) {
        if (peakLocs.at(i) > 0) {
            return i;
        }
    }

    return 0;
}

// Return the index of the last minimum peak
int PulseTrainGenerator::getLastMinLoc() {
    for (int i = peakLocs.size() - 1; i >= 0; i--) {
        if (peakLocs.at(i) < 0) {
            return i;
        }
    }

    return 0;
}
