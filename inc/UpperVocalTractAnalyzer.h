//
// Created by Joseph Bellahcen on 4/16/22.
//

#ifndef TMS_EXPRESS_UPPERVOCALTRACTANALYZER_H
#define TMS_EXPRESS_UPPERVOCALTRACTANALYZER_H

class UpperVocalTractAnalyzer {
public:
    UpperVocalTractAnalyzer(int numSegments, int samplesPerSegment, int order);
    ~UpperVocalTractAnalyzer();

    void lpcAnalysis(int i, float *xcorr);
    void estimateEnergy(int i, float *segment);

    float *getReflectorCoefficients(int i);
    float getEnergy(int i);

private:
    int numSegments;
    int segmentSize;
    int order;

    float **reflectorCoefficients;
    float *energies;
};

#endif //TMS_EXPRESS_UPPERVOCALTRACTANALYZER_H
