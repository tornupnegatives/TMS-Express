#ifndef TMS_EXPRESS_LINEARPREDICTOR_H
#define TMS_EXPRESS_LINEARPREDICTOR_H

#include <vector>

class LinearPredictor {
public:
    explicit LinearPredictor(int modelOrder = 10);

    std::vector<float> reflectorCoefficients(const std::vector<float> &acf);
    float gain() const;

private:
    int order;
    float error;
};

#endif //TMS_EXPRESS_LINEARPREDICTOR_H