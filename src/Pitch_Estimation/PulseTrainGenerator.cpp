//
// Created by Joseph Bellahcen on 5/15/22.
//

#include "Pitch_Estimation/PulseTrainGenerator.h"

#include <vector>
#include "Pitch_Estimation/RundownCircuit.h"

PulseTrainGenerator::PulseTrainGenerator() {
    // Each pulse train stores a different measurement as follows:
    // M0: Maxima amplitude
    // M1: Peak-to-peak from previous minimum to current maximum
    // M2: Peak-to-peak from previous maximum to current maximum
    // M3: Minima amplitude
    // M4: Peak-to-peak from previous maximum to current minimum
    // M5: Peak-to-peak from previous minimum to current minimum
    measurements = vector<vector<float>>(6, vector<float>());

    // Rather than constructing a bona-fide pulse train, which would consist
    // mostly of empty space, measurements are stored contiguously, with no
    //gaps between them, and the indices of each measurement are stored in
    // a separate vector
    locs = vector<vector<int>>(6, vector<int>());

    // The RundownCircuit helps prevent the recording of spurious peaks by
    // gating each pulse train based on the amplitude and period of the
    // previous peak. Each pulse train will have its own RundownCircuit
    rundowns = vector<RundownCircuit>(6, RundownCircuit());

    // All measurement vectors begin with a zero element such that there is
    // something for the first pulses in each to compare themselves against
    for (int i = 0; i < 6; i++) {
        measurements[i] = vector<float>(1, 0);
        locs[i] = vector<int>(1, 0);
    }
}

// Reset the PulseTrainGenerator to its initialization state
void PulseTrainGenerator::reset() {
    for (int i = 0; i < 6; i++) {
        measurements[i].erase(measurements[i].begin() + 1, measurements[i].end());
        locs[i].erase(locs[i].begin() + 1, locs[i].end());
        rundowns[i].reset();
    }
}

// Update the pulse trains with the given peak
//
// Only three pulse trains may be updated per iteration, depending on whether
// the peak corresponds to a minimum or a maximum point. Before any
// measurement is stored, a RundownCircuit inspects it to ensure that it is
// not spurious
void PulseTrainGenerator::update(float peak, int loc, MeasurementType type) {
    // Decide which pulse trains should be updated
    int start, stop;
    if (type == MAX_PEAK) {
        start = 0;
        stop = 2;
    } else {
        start = 3;
        stop = 5;
    }

    // Fetch the current peak as well as the values of the previous extrema,
    // the latter two of which will be required for measurements 1,2,4,5
    float amplitude = abs(peak);
    float lastMax = measurements[MAX_PEAK].back();
    float lastMin = measurements[MIN_PEAK].back();

    // Take measurements, advance the RundownCircuits, and store data
    for (int i = start; i <= stop; i++) {
        // Take the appropriate measurement
        float measurement = 0;
        switch (i % 3) {
            case 0:
                // Peak amplitude
                measurement = amplitude;
                break;

            case 1:
                // Max-to-min (peak-to-valley) distance
                measurement = (type == MAX_PEAK) ? abs(amplitude - lastMin) : abs(amplitude - lastMax);
                break;

            case 2:
                // Peak-to-peak distance
                measurement = (type == MAX_PEAK) ? (amplitude - lastMax) : (amplitude - lastMin);
                measurement = (measurement < 0) ? 0 : abs(measurement);
                break;
        }

        // The RundownCircuit will indicate whether the data is likely
        // significant. Spurious peaks will not be stored in the pulse trains
        bool shouldAdvance = rundowns[i].advance(measurement, loc);

        if (shouldAdvance) {
            measurements[i].push_back(measurement);
            locs[i].push_back(loc);
        }
    }
}

// Generate a 6x6 pitch-period estimate matrix from the internal pulse trains
//
// The six columns of the PPE matrix correspond to the six pulse trains, while
// the rows correspond to various measurements of periodicity between the
// four most recent measurements in each pulse train
const vector<vector<int>> PulseTrainGenerator::getEstimateMatrix() {
    // Allocate estimate matrix
    //
    // +-----+-----+-----+-----+-----+-----+-----+
    // |  _  | M0  | M1  | M2  | M3  | M4  | M5  |
    // +-----+-----+-----+-----+-----+-----+-----+
    // | PE0 | P00 | P10 | P20 | P30 | P40 | P50 |
    // | PE1 | P01 | P11 | P21 | P31 | P41 | P51 |
    // | PE2 | P02 | P12 | P22 | P32 | P42 | P52 |
    // | PE3 | P03 | P13 | P23 | P33 | P43 | P53 |
    // | PE4 | P04 | P14 | P24 | P34 | P44 | P54 |
    // | PE5 | P05 | P15 | P25 | P35 | P45 | P55 |
    // +-----+-----+-----+-----+-----+-----+-----+
    vector<vector<int>> estimateMatrix = vector<vector<int>>(6, vector<int>(6));

    // Take column-wise measurements
    for (int i = 0; i < 6; i++) {
        // Get the four most recent elements in the target pulse train
        vector<int> recentLocs = vector<int>(4);
        std::copy(locs.at(i).end() - 4, locs.at(i).end(), recentLocs.begin());

        // Measure the periods between certain pulses
        //
        //           (0)        (1)        (2)        (3)
        //            ^          ^          ^          ^
        //            |          |          |          |
        //            |---------------PN5--------------|
        estimateMatrix[5][i] = recentLocs.at(3) - recentLocs.at(0);
        //            |          |          |          |
        //            |---------PN4---------|          |
        estimateMatrix[4][i] = recentLocs.at(2) - recentLocs.at(0);
        //            |          |          |          |
        //            |          |---------PN3---------|
        estimateMatrix[3][i] = recentLocs.at(3) - recentLocs.at(1);
        //            |          |          |          |
        //            |----PN2---|          |          |
        estimateMatrix[2][i] = recentLocs.at(1) - recentLocs.at(0);
        //            |          |          |          |
        //            |          |----PN1---|          |
        estimateMatrix[1][i] = recentLocs.at(2) - recentLocs.at(1);
        //            |          |          |          |
        //            |          |          |----PN0---|
        estimateMatrix[0][i] = recentLocs.at(3) - recentLocs.at(2);
        // +----------+----------+----------+----------+----------+
    }

    return estimateMatrix;
}
