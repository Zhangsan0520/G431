#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "pid.h"

typedef enum
{
    MOTOR_CONTROL_A = 0,
    MOTOR_CONTROL_B,
    MOTOR_CONTROL_C,
    MOTOR_CONTROL_D,
    MOTOR_CONTROL_COUNT
} MotorControl_Motor_t;

HAL_StatusTypeDef MotorControl_Init(void);

void MotorControl_Update(void);

void MotorControl_SetTarget(MotorControl_Motor_t motor, int32_t target);
void MotorControl_SetTargetAll(int32_t target_a,
                               int32_t target_b,
                               int32_t target_c,
                               int32_t target_d);
int32_t MotorControl_GetTarget(MotorControl_Motor_t motor);
int32_t MotorControl_GetActual(MotorControl_Motor_t motor);
int32_t MotorControl_GetDuty(MotorControl_Motor_t motor);

void MotorControl_SetPID(MotorControl_Motor_t motor, float kp, float ki, float kd);
void MotorControl_SetDutyLimit(MotorControl_Motor_t motor, int32_t duty_limit);
void MotorControl_SetDutyLimitAll(int32_t duty_limit);
void MotorControl_ResetPID(MotorControl_Motor_t motor);
void MotorControl_ResetAllPID(void);

void MotorControl_Enable(MotorControl_Motor_t motor);
void MotorControl_Disable(MotorControl_Motor_t motor);
void MotorControl_EnableAll(void);
void MotorControl_DisableAll(void);
void MotorControl_StopAll(void);

void MotorControl_SetMotorDirectionInverted(MotorControl_Motor_t motor, uint8_t inverted);
void MotorControl_SetEncoderDirectionInverted(MotorControl_Motor_t motor, uint8_t inverted);

void MotorControl_GetSerialPlotData(int32_t target[4],
                                    int32_t actual[4],
                                    int32_t duty[4]);

#ifdef __cplusplus
}
#endif

#endif
