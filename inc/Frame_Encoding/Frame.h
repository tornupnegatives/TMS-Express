//
// Created by Joseph Bellahcen on 4/18/22.
//

#ifndef TMS_EXPRESS_FRAME_H
#define TMS_EXPRESS_FRAME_H

class Frame {
public:
    explicit Frame(int order);
    Frame(int order, int pitch, int voicing, float *coefficients, float gain);
    ~Frame();

    void setPitch(int pitch);
    void setVoicing(int voicing);
    void setCoefficients(float *coefficients);
    void setGain(float gain);

    int getQuantizedPitch();
    unsigned char getQuantizedVoicing();
    int *getQuantizedCoefficients();
    int getQuantizedGain();

private:
    int order;
    int pitch;
    int voicing;
    float *reflectorCoefficients;
    float gain;

    int closestValueIndexFinderInt(int value, const int *codingTableEntry, int size);
    int closestValueIndexFinderFloat(float value, const float *codingTableEntry, int size);
};

#endif //TMS_EXPRESS_FRAME_H
