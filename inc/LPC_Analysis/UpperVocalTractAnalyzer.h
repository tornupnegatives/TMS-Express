//
// Created by Joseph Bellahcen on 4/16/22.
//

#ifndef TMS_EXPRESS_UPPERVOCALTRACTANALYZER_H
#define TMS_EXPRESS_UPPERVOCALTRACTANALYZER_H

#include "Frame_Encoding/Frame.h"

class UpperVocalTractAnalyzer {
public:
    UpperVocalTractAnalyzer(int samplesPerSegment, int order);

    float *lpcCoefficients(float *xcorr, float *error);
    float gain(float predictionError);

private:
    int samplesPerSegment;
    int order;
};

#endif //TMS_EXPRESS_UPPERVOCALTRACTANALYZER_H
