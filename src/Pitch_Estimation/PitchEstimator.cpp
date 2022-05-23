//
// Created by Joseph Bellahcen on 4/16/22.
//
#include "Pitch_Estimation/PitchEstimator.h"
#include "Pitch_Estimation/PulseTrainGenerator.h"

#include <cmath>
#include <vector>

PitchEstimator::PitchEstimator(float windowWidth, int sampleRate, int minPitchHz, int maxPitchHz, float unvoicedThreshold) {
    // User-configurable parameters
    PitchEstimator::minPitchPeriod = sampleRate / maxPitchHz;
    PitchEstimator::maxPitchPeriod = sampleRate / minPitchHz;
    PitchEstimator::unvoicedThreshold = unvoicedThreshold;

    // The Gold-Rabiner pitch estimation algorithm uses information about the local extrema (peaks) of a signal.
    // The PitchEstimator object will detect the signal peaks and pass their indices and amplitudes to a
    // PulseTrainGenerator which will take a number of measurements around the peak
    PitchEstimator::pulseTrainGenerator = PulseTrainGenerator();

    // The coincidence window is used to analyze pulse trains and select the most likely estimate of pitch. It is
    // composed of the below-tabulated values, which must be scaled to correspond with vector indices of a signal with
    // a certain sample rate
    //
    // +----------------+--------+--------+--------+--------+
    // | Threshold/Bias |   1    |   2    |   5    |   7    |
    // +----------------+--------+--------+--------+--------+
    // | 1.6-3.1 ms     | 0.1 ms | 0.2 us | 0.3 ms | 0.4 ms |
    // | 3.1-6.3 ms     | 0.2 ms | 0.4 ms | 0.6 ms | 0.8 ms |
    // | 6.3-12.7 ms    | 0.4 ms | 0.8 ms | 1.2 ms | 1.6 ms |
    // | 12.7-25.5 ms   | 0.8 ms | 1.6 ms | 2.4 ms | 3.2 ms |
    // +----------------+--------+--------+--------+--------+
    windowThresholds = {int(ceil(sampleRate * 3.10e-3)), int(ceil(sampleRate * 6.30e-3)),
                        int(ceil(sampleRate * 12.7e-3)), int(ceil(sampleRate * 25.5e-3))};
    coincidenceWindows = vector<vector<int>>(4, vector<int>(4));
    for (int i = 1; i <= 4; i++) {
        for (int j = 1; j <= 4; j++) {
            int coincidence = int(ceil(pow(2, i - 1) * j * sampleRate * 1e-4));
            coincidenceWindows.at(i - 1).at(j - 1) = coincidence;
        }
    }
}

int PitchEstimator::estimatePitch(vector<float> segment, int *coincidence) {
    // Clear data from computations of previous segments
    pulseTrainGenerator.reset();

    // Detect local extrema and build pulse trains based on measurements
    // thereof. The result of analysis is a 6x6 estimate matrix
    detectPeaks(segment);
    auto estimateMatrix = pulseTrainGenerator.getEstimateMatrix();

    // The most-likely pitch period is one of the elements in the first row of the estimate matrix
    auto candidates = estimateMatrix.at(0);
    auto coincidences = vector<int>(6, 0);

    for (int i = 0; i < 6; i++) {
        int candidate = candidates.at(i);

        // Determine which window is appropriate for inspecting the target candidate
        auto firstStrictlyLargerValue = std::upper_bound(windowThresholds.begin(),
                                                         windowThresholds.end(), candidate);
        auto windowIdx = std::distance(windowThresholds.begin(), firstStrictlyLargerValue);
        auto window = coincidenceWindows.at(windowIdx);

        // Each candidate will have four potential coincidence scores, one for each pane of the window
        auto potentialCoincidences = vector<int>(4, 0);
        for (int j = 0; j < 4; j++) {
            // Get the threshold and bias of the pane
            int paneThreshold = window.at(j);
            int paneBias = biases.at(j);

            // Compare candidate against all other pitch period estimates using the pane threshold
            for (int row = 0; row < 6; row++) {
                auto estimateRow = estimateMatrix.at(row);

                for (int col = 0; col < 6; col++) {
                    // Do not compare a candidate against itself
                    /*
                    if (row == 0 && col == i) {
                        continue;
                    }
                     */

                    // Two estimates are coincident within a pane if the magnitude of their difference is below the
                    // threshold of that pane
                    int estimate = estimateMatrix.at(row).at(col);
                    int difference = abs(candidate - estimate);
                    if (difference <= paneThreshold) {
                        potentialCoincidences.at(j)++;
                    }
                }
            }

            // After counting all the coincidences within a given pane, subtract the bias
            potentialCoincidences.at(j) -= paneBias;
        }

        // After looking at the estimate through each pane, select the largest coincidence to represent it
        coincidences.at(i) = *std::max_element(potentialCoincidences.begin(), potentialCoincidences.end());
    }

    // Once all the candidates have been inspected, select the one with the largest coincidence to represent the pitch
    // period of the segment. Furthermore, store the coincidence value for voicing determination
    auto maxCoincidence = std::max_element(coincidences.begin(), coincidences.end());
    auto maxIdx = std::distance(coincidences.begin(), maxCoincidence);
    int pitchPeriodEstimate = candidates.at(maxIdx);

    *coincidence = *maxCoincidence;
    if (pitchPeriodEstimate >= minPitchPeriod && pitchPeriodEstimate <= maxPitchPeriod) {
        return pitchPeriodEstimate;
    } else {
        return 0;
    }


    return pitchPeriodEstimate;
}

// Detect the peaks (local maxima and minima) of the segment
void PitchEstimator::detectPeaks(vector<float> segment) {
    bool increasing = segment.at(0) < segment.at(1);
    float previous = segment.at(0);

    for (int i = 1; i < segment.size() - 1; i++) {
        float current = segment.at(i);

        if ((increasing && current > previous) || (!increasing && current < previous)) {
            // If the slope has not changed directions, the point is not a peak
            previous = current;
            continue;

        } else {
            auto type = increasing ? PulseTrainGenerator::MAX_PEAK : PulseTrainGenerator::MIN_PEAK;
            pulseTrainGenerator.update(current, i - 1, type);

            increasing = !increasing;
            previous = current;
        }
    }
}
