// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_FRAME_H
#define TMS_EXPRESS_FRAME_H

#include "json.hpp"
#include <string>
#include <vector>

class Frame {
public:
    Frame(int pitchPeriod, bool isVoiced, float gainDB, std::vector<float> coeffs);

    // Getters & setters
    std::vector<float> getCoeffs();
    void setCoeffs(std::vector<float> coeffs);

    [[nodiscard]] float getGain() const;
    void setGain(float gainDb);
    void setGain(int codingTableIdx);

    [[nodiscard]] int getPitch() const;
    void setPitch(int pitchPeriod);

    bool getRepeat() const;
    void setRepeat(bool isRepeat);

    [[nodiscard]] bool getVoicing() const;
    void setVoicing(bool isVoiced);

    // Const getters
    std::vector<int> quantizedCoeffs();
    [[nodiscard]] int quantizedGain() const;
    [[nodiscard]] int quantizedPitch() const;
    [[nodiscard]] [[maybe_unused]] int quantizedVoicing() const;

    // Boolean properties
    [[nodiscard]] bool isRepeat() const;
    [[nodiscard]] bool isSilent();
    [[nodiscard]] bool isVoiced() const;

    // Serialization
    std::string toBinary();
    nlohmann::json toJSON();

private:
    float gain;
    int pitch;
    std::vector<float> reflectorCoeffs;
    bool isRepeatFrame;
    bool isVoicedFrame;

    static int closestCodingTableIndexForValue(float value, std::vector<float> codingTableRow);
    static std::string valueToBinary(int value, int bitWidth);
};

#endif //TMS_EXPRESS_FRAME_H
