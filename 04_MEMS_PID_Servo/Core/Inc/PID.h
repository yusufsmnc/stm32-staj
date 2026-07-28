/*
 * PID.h
 *
 *  Created on: Jul 28, 2026
 *      Author: Yusuf
 */

#ifndef INC_PID_H_
#define INC_PID_H_

typedef struct {
    float Kp, Ki, Kd;

    float integral;
    float onceki_hata;

    float cikis_min;
    float cikis_max;
} PID_t;

void  PID_Init(PID_t *pid, float Kp, float Ki, float Kd, float cmin, float cmax);
float PID_Compute(PID_t *pid, float hedef, float olcum, float dt);
void  PID_Reset(PID_t *pid);


#endif /* INC_PID_H_ */
