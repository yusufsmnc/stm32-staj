/*
 * PID.c
 *
 *  Created on: Jul 28, 2026
 *      Author: Yusuf
 */


#include "PID.h"

void PID_Init(PID_t *pid, float Kp, float Ki, float Kd, float cmin, float cmax)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;

    pid->integral = 0.0f;
    pid->onceki_hata = 0.0f;

    pid->cikis_min = cmin;
    pid->cikis_max = cmax;
}

float PID_Compute(PID_t *pid, float hedef, float olcum, float dt)
{
    float hata = hedef - olcum;

    pid->integral += hata * dt;

    float turev = (hata - pid->onceki_hata) / dt;

    float cikis = (pid->Kp * hata)
                + (pid->Ki * pid->integral)
                + (pid->Kd * turev);

    if (cikis > pid->cikis_max) cikis = pid->cikis_max;
    if (cikis < pid->cikis_min) cikis = pid->cikis_min;

    pid->onceki_hata = hata;

    return cikis;
}

void PID_Reset(PID_t *pid)
{
    pid->integral = 0.0f;
    pid->onceki_hata = 0.0f;
}
