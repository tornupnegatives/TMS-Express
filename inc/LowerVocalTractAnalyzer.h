//
// Created by Joseph Bellahcen on 4/16/22.
//

#ifndef TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H
#define TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H

class LowerVocalTractAnalyzer {
public:
    enum voicing {VOICED, UNVOICED};
    LowerVocalTractAnalyzer(int numSegments, int samplesPerSegment, float unvoicedThreshold = 0.3);
    ~LowerVocalTractAnalyzer();

    void estimatePitch(int i, float *xcorr);
    void detectVoicing(int i, float *xcorr);

    int *getPitches();
    voicing *getVoicings();

private:
    int numSegments;
    int segmentSize;
    float unvoicedThreshold;
    int *pitches;
    voicing *voicings;
};

#endif //TMS_EXPRESS_LOWERVOCALTRACTANALYZER_H
