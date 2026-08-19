#include "encoder.h"

#include "tim.h"

typedef struct
{
    TIM_HandleTypeDef *timer;
    uint8_t direction_inverted;
    uint16_t last_count;
    int16_t delta;
    int32_t position;
} Encoder_Channel_t;

static Encoder_Channel_t encoder_channels[ENCODER_MOTOR_COUNT] =
{
    [ENCODER_MOTOR_A] =
    {
        .timer = &htim1,
    },
    [ENCODER_MOTOR_B] =
    {
        .timer = &htim8,
    },
    [ENCODER_MOTOR_C] =
    {
        .timer = &htim3,
    },
    [ENCODER_MOTOR_D] =
    {
        .timer = &htim4,
    },
};

static uint8_t Encoder_IsValidMotor(ENCODER_Motor_t motor)
{
    return ((uint32_t)motor < (uint32_t)ENCODER_MOTOR_COUNT);
}

static uint16_t Encoder_ReadCounter(const Encoder_Channel_t *channel)
{
    return (uint16_t)__HAL_TIM_GET_COUNTER(channel->timer);
}

HAL_StatusTypeDef Encoder_Init(void)
{
    HAL_StatusTypeDef status;

    status = HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    if (status != HAL_OK)
    {
        return status;
    }

    status = HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
    if (status != HAL_OK)
    {
        return status;
    }

    Encoder_ResetAll();

    return HAL_OK;
}

void Encoder_Reset(ENCODER_Motor_t motor)
{
    Encoder_Channel_t *channel;

    if (!Encoder_IsValidMotor(motor))
    {
        return;
    }

    channel = &encoder_channels[motor];
    __HAL_TIM_SET_COUNTER(channel->timer, 0U);
    channel->last_count = 0U;
    channel->delta = 0;
    channel->position = 0;
}

void Encoder_ResetAll(void)
{
    for (ENCODER_Motor_t motor = ENCODER_MOTOR_A; motor < ENCODER_MOTOR_COUNT; motor++)
    {
        Encoder_Reset(motor);
    }
}

int16_t Encoder_Update(ENCODER_Motor_t motor)
{
    Encoder_Channel_t *channel;
    uint16_t current_count;
    int16_t delta;

    if (!Encoder_IsValidMotor(motor))
    {
        return 0;
    }

    channel = &encoder_channels[motor];
    current_count = Encoder_ReadCounter(channel);
    delta = (int16_t)(current_count - channel->last_count);

    if (channel->direction_inverted != 0U)
    {
        delta = (int16_t)(-delta);
    }

    channel->last_count = current_count;
    channel->delta = delta;
    channel->position += delta;

    return delta;
}

void Encoder_UpdateAll(void)
{
    for (ENCODER_Motor_t motor = ENCODER_MOTOR_A; motor < ENCODER_MOTOR_COUNT; motor++)
    {
        (void)Encoder_Update(motor);
    }
}

int16_t Encoder_GetDelta(ENCODER_Motor_t motor)
{
    if (!Encoder_IsValidMotor(motor))
    {
        return 0;
    }

    return encoder_channels[motor].delta;
}

int32_t Encoder_GetPosition(ENCODER_Motor_t motor)
{
    if (!Encoder_IsValidMotor(motor))
    {
        return 0;
    }

    return encoder_channels[motor].position;
}

uint16_t Encoder_GetRawCount(ENCODER_Motor_t motor)
{
    if (!Encoder_IsValidMotor(motor))
    {
        return 0U;
    }

    return Encoder_ReadCounter(&encoder_channels[motor]);
}

void Encoder_SetDirectionInverted(ENCODER_Motor_t motor, uint8_t inverted)
{
    if (!Encoder_IsValidMotor(motor))
    {
        return;
    }

    encoder_channels[motor].direction_inverted = (inverted != 0U) ? 1U : 0U;
}
