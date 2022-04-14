//
// Created by Joseph Bellahcen on 4/11/22.
//

#ifndef TMS_EXPRESS_COEFFICIENTSOLVER_H
#define TMS_EXPRESS_COEFFICIENTSOLVER_H


class CoefficientSolver {
public:
    CoefficientSolver(int order);
    ~CoefficientSolver();
    void solve(float *r);
    float *getReflectionCoefficients(int *size);

private:
    int order;
    float *coefficients;
};

#endif //TMS_EXPRESS_COEFFICIENTSOLVER_H
