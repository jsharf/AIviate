#include "Filters.h"

KalmanFilter::KalmanFilter(float Q_x, float Q_dx_dt, float R_x)
{
    config.Q_x = Q_x;
    config.Q_dx_dt = Q_dx_dt;
    config.R_x = R_x;

    state.x_bias =
        state.x_x =
            state.P_00 =
                state.P_01 =
                    state.P_10 =
                        state.P_11 = 0;
}

float KalmanFilter::calculate(float newX, float newDx_dt, float dt)
{
    state.x_x += dt * (newDx_dt - state.x_bias);
    state.P_00 +=  - dt * (state.P_10 + state.P_01) + config.Q_x * dt;
    state.P_01 +=  - dt * state.P_11;
    state.P_10 +=  - dt * state.P_11;
    state.P_11 +=  + config.Q_dx_dt * dt;

    state.y = newX - state.x_x;
    state.S = state.P_00 + config.R_x;
    state.K_0 = state.P_00 / state.S;
    state.K_1 = state.P_10 / state.S;

    state.x_x +=  state.K_0 * state.y;
    state.x_bias  +=  state.K_1 * state.y;
    state.P_00 -= state.K_0 * state.P_00;
    state.P_01 -= state.K_0 * state.P_01;
    state.P_10 -= state.K_1 * state.P_00;
    state.P_11 -= state.K_1 * state.P_01;

    return state.x_x;
}

ComplementaryFilter::ComplementaryFilter(float k)
{
    config.k = k;

    state.x1 =
        state.y1 =
            state.x_x = 0;
}

float ComplementaryFilter::calculate(float newX, float newDx_dt, float dt)
{
    state.x1 = (newX - state.x_x)*config.k*config.k;
    state.y1 = dt*state.x1 + state.y1;
    state.x2 = state.y1 + (newX - state.x_x)*2*config.k + newDx_dt;
    state.x_x = dt*state.x2 + state.x_x;

    return state.x_x;
}
