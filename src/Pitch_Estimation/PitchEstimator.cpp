//
// Created by Joseph Bellahcen on 4/16/22.
//
#include "Pitch_Estimation/PitchEstimator.h"
#include "Pitch_Estimation/PulseTrainGenerator.h"
#include "Pitch_Estimation/RundownCircuit.h"

#include <cstdlib>
#include <vector>

PitchEstimator::PitchEstimator(float windowWidth, int sampleRate, int minPitchHz, int maxPitchHz, float unvoicedThreshold) {
    // User-configurable parameters
    PitchEstimator::minPitchPeriod = sampleRate / maxPitchHz;
    PitchEstimator::maxPitchPeriod = sampleRate / minPitchHz;
    PitchEstimator::unvoicedThreshold = unvoicedThreshold;

    // Peak detector and pulse train generator structures
    //
    // The Gold-Rabiner pitch estimation algorithm uses information about the local extrema (peaks) of a signal.
    // The PitchEstimator object will detect the signal peaks and pass their indices and amplitudes to a
    // PulseTrainGenerator which will take a number of measurements around the peak
    PitchEstimator::peakIndices = vector<int>();
    PitchEstimator::pulseTrainGenerator = PulseTrainGenerator();

    // Peak Detecting Exponential Window Circuit structures
    //
    // The PDEWC will help filter out peaks which do not contribute to pitch estimation. Essentially it is a gating
    // mechanism which, upon detecting a relevant peak, ignores subsequent peaks for a decaying interval
    //PitchEstimator::averagePitchPeriod = sampleRate * windowWidth * 1e-3f;
    //PitchEstimator::blankingInterval = 0.4f * averagePitchPeriod;
    PitchEstimator::maxRundownCircuit = RundownCircuit();
    PitchEstimator::minRundownCircuit = RundownCircuit();



}

int PitchEstimator::estimatePitch(vector<float> *segment) {
    // Clear data from computations of previous segments
    pulseTrainGenerator.reset();
    maxRundownCircuit.reset();
    minRundownCircuit.reset();

    // Find the local extrema of the segment
    detectPeaks(segment);
    //updatePulseTrains(segment);

}

// Detect the peaks (local maxima and minima) of the segment
//
// Because the peakIndices vector represents array indices, which must always be signed, signedness is used to indicate
// whether the index corresponds to a maximum (positive) or minimum (negative)
void PitchEstimator::detectPeaks(vector<float> *segment) {
    bool increasing = segment->at(0) < segment->at(1);
    float previous = segment->at(0);

    for (int i = 1; i < segment->size() - 1; i++) {
        float current = segment->at(i);

        if ((increasing && current > previous) || (!increasing && current < previous)) {
            // If the slope has not changed directions, the point is not a peak
            previous = current;
            continue;

        } else {
            // When the slope changes directions, update the pulse trains
            // TODO: move rundown into PulseTrainEstimator and have one rundown per train
            // TODO: also separate min/max trains to this end
            //if (rundownCircuit.advance(current, i - 1)) {
                pulseTrainGenerator.update(current, i - 1, increasing);

            increasing = !increasing;
            previous = current;
        }
    }
}
