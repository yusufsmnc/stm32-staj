/*
 * PID.h
 *
 *  Created on: Jul 28, 2026
 *      Author: Yusuf
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#include <stdint.h>

typedef struct {
    float Kp, Ki, Kd;

    float integral;
    float integral_min;
    float integral_max;

    float onceki_hata;
    uint32_t onceki_zaman;

    float cikis_min;
    float cikis_max;
} PID_t;

void  PID_Init(PID_t *pid, float Kp, float Ki, float Kd,
               float cikis_min, float cikis_max,
               float integral_min, float integral_max);
float PID_Compute(PID_t *pid, float hedef, float olcum);
void  PID_Reset(PID_t *pid);

#endif /* INC_PID_H_ */
