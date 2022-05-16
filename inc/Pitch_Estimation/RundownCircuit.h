//
// Created by Joseph Bellahcen on 5/15/22.
//

#ifndef TMS_EXPRESS_RUNDOWNCIRCUIT_H
#define TMS_EXPRESS_RUNDOWNCIRCUIT_H

class RundownCircuit {
public:
    explicit RundownCircuit(int sampleRateHz = 8000, float windowWidthMs = 22.5);

    void reset();
    bool advance(float peak, int loc);

private:
    int sampleRate;

    int averagePeriod;
    int blankingInterval;
    float decayConstant;

    int lastLoc;
    float lastPeak;

    void updatePeriod(int newPeriod, int loc);
    float decayThreshold(int dt);
};

#endif //TMS_EXPRESS_RUNDOWNCIRCUIT_H
