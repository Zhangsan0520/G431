#include "pid.h"

static float PID_Clamp(float value, float min_value, float max_value)
{
    if (value > max_value)
    {
        return max_value;
    }

    if (value < min_value)
    {
        return min_value;
    }

    return value;
}

void PID_Init(PID_t *pid,
              float kp,
              float ki,
              float kd,
              float output_min,
              float output_max,
              float integral_min,
              float integral_max)
{
    if (pid == 0)
    {
        return;
    }

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->integral_min = integral_min;
    pid->integral_max = integral_max;
    PID_Reset(pid);
}

void PID_Reset(PID_t *pid)
{
    if (pid == 0)
    {
        return;
    }

    pid->integral = 0.0f;
    pid->previous_error = 0.0f;
}

void PID_SetTunings(PID_t *pid, float kp, float ki, float kd)
{
    if (pid == 0)
    {
        return;
    }

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

void PID_SetOutputLimits(PID_t *pid, float output_min, float output_max)
{
    if ((pid == 0) || (output_min > output_max))
    {
        return;
    }

    pid->output_min = output_min;
    pid->output_max = output_max;
}

void PID_SetIntegralLimits(PID_t *pid, float integral_min, float integral_max)
{
    if ((pid == 0) || (integral_min > integral_max))
    {
        return;
    }

    pid->integral_min = integral_min;
    pid->integral_max = integral_max;
}

float PID_Update(PID_t *pid, float target, float actual)
{
    float error;
    float derivative;
    float output;

    if (pid == 0)
    {
        return 0.0f;
    }

    error = target - actual;
    pid->integral += error;
    pid->integral = PID_Clamp(pid->integral, pid->integral_min, pid->integral_max);

    derivative = error - pid->previous_error;
    pid->previous_error = error;

    output = (pid->kp * error) + (pid->ki * pid->integral) + (pid->kd * derivative);
    return PID_Clamp(output, pid->output_min, pid->output_max);
}
