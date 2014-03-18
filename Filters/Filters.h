#ifndef FILTERS_H
#define FILTERS_H

#include <math.h>

class KalmanFilter
{
private:
    struct KalmanState_t
    {
        float x_bias, x_x;
        float P_00, P_01, P_10, P_11;
        float  y, S;
        float K_0, K_1;
    } state;

public:
    struct KalmanConfiguration_t
    {
        float Q_x, Q_dx_dt, R_x;
    } config;
    KalmanFilter(float Q_x, float Q_dx_dt, float R_x);
    float calculate(float newX, float newDx_dt, float dt);

};

class ComplementaryFilter
{
private:
    struct ComplementaryState_t
    {
        float x1, y1, x_x, x2;
    } state;

public:
    struct ComplementaryConfiguration_t
    {
        float k;
    } config;
    ComplementaryFilter(float k);
    float calculate(float newX, float newDx_dt, float dt);
};

#endif // FILTERS_H
