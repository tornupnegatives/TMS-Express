#ifndef TMS_EXPRESS_AUTOCORRELATOR_H
#define TMS_EXPRESS_AUTOCORRELATOR_H

#include <vector>

class Autocorrelator {
public:
    static std::vector<float> process(const std::vector<float>& segment);
};

#endif //TMS_EXPRESS_AUTOCORRELATOR_H
