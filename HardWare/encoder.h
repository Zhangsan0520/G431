#ifndef ENCODER_H
#define ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32g4xx_hal.h"

typedef enum
{
    ENCODER_MOTOR_A = 0,
    ENCODER_MOTOR_B,
    ENCODER_MOTOR_C,
    ENCODER_MOTOR_D,
    ENCODER_MOTOR_COUNT
} ENCODER_Motor_t;

HAL_StatusTypeDef Encoder_Init(void);

void Encoder_Reset(ENCODER_Motor_t motor);
void Encoder_ResetAll(void);

int16_t Encoder_Update(ENCODER_Motor_t motor);
void Encoder_UpdateAll(void);

int16_t Encoder_GetDelta(ENCODER_Motor_t motor);
int32_t Encoder_GetPosition(ENCODER_Motor_t motor);
uint16_t Encoder_GetRawCount(ENCODER_Motor_t motor);

void Encoder_SetDirectionInverted(ENCODER_Motor_t motor, uint8_t inverted);

#ifdef __cplusplus
}
#endif

#endif
