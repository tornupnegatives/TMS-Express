// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAME_H
#define TMS_EXPRESS_FRAME_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Frame {
public:
    Frame(int pitchPeriod, bool isVoiced, float gainDB, std::vector<float> coeffs);

    int getPitch() const;
    void setPitch(int pitchPeriod);

    bool getVoicing() const;
    void setVoicing(bool isVoiced);

    void setRepeat(bool isRepeat);

    float getGain() const;
    void setGain(float gainDb);

    std::vector<float> getCoeffs();
    void setCoeffs(std::vector<float> coeffs);

    int getQuantizedPitchIdx() const;
    int getQuantizedVoicingIdx() const;
    int getQuantizedGainIdx() const;
    std::vector<int> getQuantizedCoeffsIdx();

    void setQuantizedGain(int offset);

    bool isVoiced() const;
    bool isSilent() const;
    bool isRepeat() const;

    std::string toBinary();
    nlohmann::json toJSON();

private:
    int pitch;
    bool voicedFrame;
    bool repeatFrame;
    float gain;
    std::vector<float> reflectorCoeffs;

    static int closestIndexFinder(float value, std::vector<float> codingTableEntry);
    static std::string valueToBinary(int value, int bitWidth);
};

#endif //TMS_EXPRESS_FRAME_H
