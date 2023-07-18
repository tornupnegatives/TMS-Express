// Author: Joseph Bellahcen <joeclb@icloud.com>

#ifndef TMS_EXPRESS_LINEARPREDICTOR_H
#define TMS_EXPRESS_LINEARPREDICTOR_H

#include <vector>

namespace tms_express {

class LinearPredictor {
public:
    explicit LinearPredictor(int modelOrder = 10);

    std::vector<float> reflectorCoefficients(const std::vector<float> &acf);
    [[nodiscard]] float gain() const;

private:
    int order;
    float error;
};

};  // namespace tms_express

#endif //TMS_EXPRESS_LINEARPREDICTOR_H