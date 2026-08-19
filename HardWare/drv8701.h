#ifndef DRV8701_H
#define DRV8701_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32g4xx_hal.h"

#define DRV8701_DUTY_MAX 1000

typedef enum
{
    DRV8701_MOTOR_A = 0,
    DRV8701_MOTOR_B,
    DRV8701_MOTOR_C,
    DRV8701_MOTOR_D,
    DRV8701_MOTOR_COUNT
} DRV8701_Motor_t;

HAL_StatusTypeDef DRV8701_Init(void);

void DRV8701_Enable(DRV8701_Motor_t motor);
void DRV8701_Disable(DRV8701_Motor_t motor);
void DRV8701_EnableAll(void);
void DRV8701_DisableAll(void);

void DRV8701_SetDuty(DRV8701_Motor_t motor, int16_t duty);
void DRV8701_Stop(DRV8701_Motor_t motor);
void DRV8701_StopAll(void);

void DRV8701_SetDirectionInverted(DRV8701_Motor_t motor, uint8_t inverted);
int16_t DRV8701_GetDuty(DRV8701_Motor_t motor);

#ifdef __cplusplus
}
#endif

#endif
