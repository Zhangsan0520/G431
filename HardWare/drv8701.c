#include "drv8701.h"

#include "main.h"
#include "tim.h"

typedef struct
{
    GPIO_TypeDef *dir_port;
    uint16_t dir_pin;
    GPIO_TypeDef *enable_port;
    uint16_t enable_pin;
    TIM_HandleTypeDef *pwm_timer;
    uint32_t pwm_channel;
    uint8_t direction_inverted;
    int16_t duty;
} DRV8701_Channel_t;

static DRV8701_Channel_t drv8701_channels[DRV8701_MOTOR_COUNT] =
{
    [DRV8701_MOTOR_A] =
    {
        .dir_port = MA_DIR_GPIO_Port,
        .dir_pin = MA_DIR_Pin,
        .enable_port = MA_PWE_EN_GPIO_Port,
        .enable_pin = MA_PWE_EN_Pin,
        .pwm_timer = &htim2,
        .pwm_channel = TIM_CHANNEL_1,
    },
    [DRV8701_MOTOR_B] =
    {
        .dir_port = MB_DIR_GPIO_Port,
        .dir_pin = MB_DIR_Pin,
        .enable_port = MB_PWE_EN_GPIO_Port,
        .enable_pin = MB_PWE_EN_Pin,
        .pwm_timer = &htim2,
        .pwm_channel = TIM_CHANNEL_2,
    },
    [DRV8701_MOTOR_C] =
    {
        .dir_port = MC_DIR_GPIO_Port,
        .dir_pin = MC_DIR_Pin,
        .enable_port = MC_PWE_EN_GPIO_Port,
        .enable_pin = MC_PWE_EN_Pin,
        .pwm_timer = &htim2,
        .pwm_channel = TIM_CHANNEL_3,
    },
    [DRV8701_MOTOR_D] =
    {
        .dir_port = MD_DIR_GPIO_Port,
        .dir_pin = MD_DIR_Pin,
        .enable_port = MD_PWE_EN_GPIO_Port,
        .enable_pin = MD_PWE_EN_Pin,
        .pwm_timer = &htim2,
        .pwm_channel = TIM_CHANNEL_4,
    },
};

static uint8_t DRV8701_IsValidMotor(DRV8701_Motor_t motor)
{
    return ((uint32_t)motor < (uint32_t)DRV8701_MOTOR_COUNT);
}

static int16_t DRV8701_ClampDuty(int16_t duty)
{
    if (duty > DRV8701_DUTY_MAX)
    {
        return DRV8701_DUTY_MAX;
    }

    if (duty < -DRV8701_DUTY_MAX)
    {
        return -DRV8701_DUTY_MAX;
    }

    return duty;
}

static uint16_t DRV8701_AbsDuty(int16_t duty)
{
    return (duty < 0) ? (uint16_t)(-duty) : (uint16_t)duty;
}

static uint32_t DRV8701_DutyToCompare(const DRV8701_Channel_t *channel, uint16_t duty)
{
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(channel->pwm_timer);

    return ((period + 1U) * (uint32_t)duty) / DRV8701_DUTY_MAX;
}

static void DRV8701_WritePwm(DRV8701_Channel_t *channel, uint16_t duty)
{
    __HAL_TIM_SET_COMPARE(channel->pwm_timer,
                          channel->pwm_channel,
                          DRV8701_DutyToCompare(channel, duty));
}

HAL_StatusTypeDef DRV8701_Init(void)
{
    HAL_StatusTypeDef status;

    status = HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    if (status != HAL_OK)
    {
        return status;
    }

    DRV8701_StopAll();
    DRV8701_DisableAll();

    return HAL_OK;
}

void DRV8701_Enable(DRV8701_Motor_t motor)
{
    if (!DRV8701_IsValidMotor(motor))
    {
        return;
    }

    HAL_GPIO_WritePin(drv8701_channels[motor].enable_port,
                      drv8701_channels[motor].enable_pin,
                      GPIO_PIN_SET);
}

void DRV8701_Disable(DRV8701_Motor_t motor)
{
    if (!DRV8701_IsValidMotor(motor))
    {
        return;
    }

    DRV8701_Stop(motor);
    HAL_GPIO_WritePin(drv8701_channels[motor].enable_port,
                      drv8701_channels[motor].enable_pin,
                      GPIO_PIN_RESET);
}

void DRV8701_EnableAll(void)
{
    for (DRV8701_Motor_t motor = DRV8701_MOTOR_A; motor < DRV8701_MOTOR_COUNT; motor++)
    {
        DRV8701_Enable(motor);
    }
}

void DRV8701_DisableAll(void)
{
    for (DRV8701_Motor_t motor = DRV8701_MOTOR_A; motor < DRV8701_MOTOR_COUNT; motor++)
    {
        DRV8701_Disable(motor);
    }
}

void DRV8701_SetDuty(DRV8701_Motor_t motor, int16_t duty)
{
    DRV8701_Channel_t *channel;
    GPIO_PinState dir_state;

    if (!DRV8701_IsValidMotor(motor))
    {
        return;
    }

    channel = &drv8701_channels[motor];
    duty = DRV8701_ClampDuty(duty);
    channel->duty = duty;

    dir_state = (duty >= 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    if (channel->direction_inverted != 0U)
    {
        dir_state = (dir_state == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    }

    DRV8701_WritePwm(channel, 0U);
    HAL_GPIO_WritePin(channel->dir_port, channel->dir_pin, dir_state);
    DRV8701_WritePwm(channel, DRV8701_AbsDuty(duty));
}

void DRV8701_Stop(DRV8701_Motor_t motor)
{
    if (!DRV8701_IsValidMotor(motor))
    {
        return;
    }

    drv8701_channels[motor].duty = 0;
    DRV8701_WritePwm(&drv8701_channels[motor], 0U);
}

void DRV8701_StopAll(void)
{
    for (DRV8701_Motor_t motor = DRV8701_MOTOR_A; motor < DRV8701_MOTOR_COUNT; motor++)
    {
        DRV8701_Stop(motor);
    }
}

void DRV8701_SetDirectionInverted(DRV8701_Motor_t motor, uint8_t inverted)
{
    if (!DRV8701_IsValidMotor(motor))
    {
        return;
    }

    drv8701_channels[motor].direction_inverted = (inverted != 0U) ? 1U : 0U;
}

int16_t DRV8701_GetDuty(DRV8701_Motor_t motor)
{
    if (!DRV8701_IsValidMotor(motor))
    {
        return 0;
    }

    return drv8701_channels[motor].duty;
}
