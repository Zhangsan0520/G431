#ifndef PID_H
#define PID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    float kp;
    float ki;
    float kd;
    float integral;
    float previous_error;
    float output_min;
    float output_max;
    float integral_min;
    float integral_max;
} PID_t;

void PID_Init(PID_t *pid,
              float kp,
              float ki,
              float kd,
              float output_min,
              float output_max,
              float integral_min,
              float integral_max);

void PID_Reset(PID_t *pid);
void PID_SetTunings(PID_t *pid, float kp, float ki, float kd);
void PID_SetOutputLimits(PID_t *pid, float output_min, float output_max);
void PID_SetIntegralLimits(PID_t *pid, float integral_min, float integral_max);

float PID_Update(PID_t *pid, float target, float actual);

#ifdef __cplusplus
}
#endif

#endif
