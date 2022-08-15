///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class: Autocorrelator
//
// Description: At the heart of many speech analysis applications is the autocorrelation function (AFC), which
//              represents the FFT of the energy spectrum of the signal. The Autocorrelator provides a single static
//              method for computing the biased ACF, which is scaled by the number of samples in the segment
//
// Author: Joseph Bellahcen <joeclb@icloud.com>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LPC_Analysis/Autocorrelator.h"
#include <vector>

// Compute the biased autocorrelation of the segment
std::vector<float> Autocorrelator::process(const std::vector<float> &segment) {
    auto size = segment.size();
    auto acf = std::vector<float>(size);

    for (int i = 0; i < size; i++) {
        float sum = 0.0f;

        for (int j = 0; j < size - i; j++) {
            sum += segment[j] * segment[j + i];
        }

        acf[i] = (sum / float(size));
    }

    return acf;
}
