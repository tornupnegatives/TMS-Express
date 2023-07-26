

#include "LPC_Analysis/LinearPredictor.hpp"
#include <cmath>
#include <vector>

namespace tms_express {

///////////////////////////////////////////////////////////////////////////////
// Initializers ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LinearPredictor::LinearPredictor(int modelOrder) {
    order_ = modelOrder;
    error_ = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Linear Prediction //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

std::vector<float> LinearPredictor::computeCoeffs(
    const std::vector<float>& acf) {
    // Reference: "Levinson–Durbin Algorithm" (Castiglioni)

    // Autocorrelation alias
    auto r = acf;

    // Reflector coefficients
    auto k = std::vector<float>(order_ + 1);

    // Errors
    auto e = std::vector<float>(order_ + 1);

    // Coefficient matrix
    auto b = std::vector<std::vector<float>>(order_ + 1,
        std::vector<float>(order_ + 1));

    e[0] = r[0];
    k[0] = 0.0f;

    for (int m = 1; m <= order_; m++) {
        float sum = r[m];
        for (int i = 1; i < m; i++) {
            sum += b[m - 1][i] * r[m - i];
        }

        b[m][m] = k[m] = -sum / e[m - 1];
        e[m] = e[m - 1] * (1 - b[m][m] * b[m][m]);

        for (int i = 1; i < m; i++) {
            b[m][i] = b[m - 1][i] + b[m][m] * b[m - 1][m - i];
        }
    }

    // The error member is the squared gain factor of the prediction
    error_ = e[order_ - 1];
    auto reflectors = std::vector<float>(k.begin() + 1, k.end());
    return reflectors;
}


float LinearPredictor::gain() const {
    // TODO(Joseph Bellahcen): Handle case where called first

    // The gain of the signal may be expressed as the ratio of the original
    //  signal energy and the residual error, which is the final error
    // coefficient. This error is scaled by a reference intensity and then
    // expressed on the decibel scale
    // Reference: http://www.sengpielaudio.com/calculator-soundlevel.htm
    // 10 * log10(x) == 20 * log10(sqrt(x))
    float gain = 10.0f * log10f(error_ / 1e-12f);
    return abs(gain);
}

};  // namespace tms_express
