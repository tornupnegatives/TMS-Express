//
// Created by Joseph Bellahcen on 4/18/22.
//

#ifndef TMS_EXPRESS_FRAME_H
#define TMS_EXPRESS_FRAME_H

#define VOICED true
#define UNVOICED false

class Frame {
public:
    explicit Frame(int order);
    Frame(int order, int pitch, int voicing, float *coefficients, float gain);
    ~Frame();

    void setPitch(int pitch);
    void setVoicing(int voicing);
    void setCoefficients(float *coefficients);

    float getGain();
    void setGain(float gain);

    int getQuantizedPitch();
    unsigned char getQuantizedVoicing();
    int *getQuantizedCoefficients();

    int getQuantizedGain();
    void setQuantizedGain(int gain);

private:
    int order;
    int pitch;
    int voicing;
    float *reflectorCoefficients;
    float gain;

    int closestValueIndex(float value, const float *codingTableEntry, int size);
};

#endif //TMS_EXPRESS_FRAME_H
