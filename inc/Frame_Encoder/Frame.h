//
// Created by Joseph Bellahcen on 4/18/22.
//

#ifndef TMS_EXPRESS_FRAME_H
#define TMS_EXPRESS_FRAME_H

#include "Tms5220CodingTable.h"

class Frame {
public:
    explicit Frame(int order);
    Frame(int order, int pitch, int voicing, float *coefficients, float energy);
    ~Frame();

    void setPitch(int pitch);
    void setVoicing(int voicing);
    void setCoefficients(float *coefficients);
    void setEnergy(float energy);

    int getQuantizedPitch();
    unsigned char getQuantizedVoicing();
    int *getQuantizedCoefficients();
    int getQuantizedEnergy();

private:
    int order;
    int pitch;
    int voicing;
    float *reflectorCoefficients;
    float energy;

    int closestValueIndexFinderInt(int value, int *codingTableEntry, int size);
    int closestValueIndexFinderFloat(float value, float *codingTableEntry, int size);
};

#endif //TMS_EXPRESS_FRAME_H
