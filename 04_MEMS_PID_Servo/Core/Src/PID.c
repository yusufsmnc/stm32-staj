/*
 * pid.c
 *
 *  Created on: Jul 28, 2026
 *      Author: Yusuf
 */

#include "PID.h"
#include "main.h"

void PID_Init(PID_t *pid, float Kp, float Ki, float Kd,
              float cikis_min, float cikis_max,
              float integral_min, float integral_max)
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;

    pid->integral = 0.0f;
    pid->integral_min = integral_min;
    pid->integral_max = integral_max;

    pid->onceki_hata = 0.0f;
    pid->onceki_zaman = HAL_GetTick();

    pid->cikis_min = cikis_min;
    pid->cikis_max = cikis_max;
}

float PID_Compute(PID_t *pid, float hedef, float olcum)
{
    uint32_t simdiki_zaman = HAL_GetTick();
    float dt = (simdiki_zaman - pid->onceki_zaman) / 1000.0f;
    pid->onceki_zaman = simdiki_zaman;

    if (dt <= 0.0f) dt = 0.001f;

    float hata = hedef - olcum;

    // Hata isaret degistirdiyse VEYA cok kucukse integral'i sifirla
    if (((hata > 0 && pid->onceki_hata < 0) || (hata < 0 && pid->onceki_hata > 0))
        || fabsf(hata) < 1.5f)
    {
        pid->integral = 0.0f;
    }

    pid->integral += hata * dt;
    if (pid->integral > pid->integral_max) pid->integral = pid->integral_max;
    if (pid->integral < pid->integral_min) pid->integral = pid->integral_min;

    float turev = (hata - pid->onceki_hata) / dt;

    float cikis = (pid->Kp * hata) + (pid->Ki * pid->integral) + (pid->Kd * turev);

    if (cikis > pid->cikis_max) cikis = pid->cikis_max;
    if (cikis < pid->cikis_min) cikis = pid->cikis_min;

    pid->onceki_hata = hata;

    return cikis;
}

void PID_Reset(PID_t *pid)
{
    pid->integral = 0.0f;
    pid->onceki_hata = 0.0f;
    pid->onceki_zaman = HAL_GetTick();
}
