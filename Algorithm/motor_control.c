#include "motor_control.h"

#include "drv8701.h"
#include "encoder.h"

#define MOTOR_CONTROL_OUTPUT_MIN ((float)(-DRV8701_DUTY_MAX))
#define MOTOR_CONTROL_OUTPUT_MAX ((float)DRV8701_DUTY_MAX)
#define MOTOR_CONTROL_INTEGRAL_MIN (-2000.0f)
#define MOTOR_CONTROL_INTEGRAL_MAX (2000.0f)

typedef struct
{
    DRV8701_Motor_t driver_motor;
    ENCODER_Motor_t encoder_motor;
    PID_t pid;
    int32_t target;
    int32_t actual;
    int32_t duty;
    uint8_t enabled;
} MotorControl_Channel_t;

static MotorControl_Channel_t motor_channels[MOTOR_CONTROL_COUNT] =
{
    [MOTOR_CONTROL_A] =
    {
        .driver_motor = DRV8701_MOTOR_A,
        .encoder_motor = ENCODER_MOTOR_A,
    },
    [MOTOR_CONTROL_B] =
    {
        .driver_motor = DRV8701_MOTOR_B,
        .encoder_motor = ENCODER_MOTOR_B,
    },
    [MOTOR_CONTROL_C] =
    {
        .driver_motor = DRV8701_MOTOR_C,
        .encoder_motor = ENCODER_MOTOR_C,
    },
    [MOTOR_CONTROL_D] =
    {
        .driver_motor = DRV8701_MOTOR_D,
        .encoder_motor = ENCODER_MOTOR_D,
    },
};

static uint8_t MotorControl_IsValidMotor(MotorControl_Motor_t motor)
{
    return ((uint32_t)motor < (uint32_t)MOTOR_CONTROL_COUNT);
}

static int32_t MotorControl_OutputToDuty(float output)
{
    if (output >= 0.0f)
    {
        return (int32_t)(output + 0.5f);
    }

    return (int32_t)(output - 0.5f);
}

HAL_StatusTypeDef MotorControl_Init(void)
{
    HAL_StatusTypeDef status;

    status = DRV8701_Init();
    if (status != HAL_OK)
    {
        return status;
    }

    status = Encoder_Init();
    if (status != HAL_OK)
    {
        return status;
    }

    for (MotorControl_Motor_t motor = MOTOR_CONTROL_A; motor < MOTOR_CONTROL_COUNT; motor++)
    {
        PID_Init(&motor_channels[motor].pid,
                 0.0f,
                 0.0f,
                 0.0f,
                 MOTOR_CONTROL_OUTPUT_MIN,
                 MOTOR_CONTROL_OUTPUT_MAX,
                 MOTOR_CONTROL_INTEGRAL_MIN,
                 MOTOR_CONTROL_INTEGRAL_MAX);
        motor_channels[motor].target = 0;
        motor_channels[motor].actual = 0;
        motor_channels[motor].duty = 0;
        motor_channels[motor].enabled = 0U;
    }

    return HAL_OK;
}

void MotorControl_Update(void)
{
    MotorControl_Channel_t *channel;
    float output;

    for (MotorControl_Motor_t motor = MOTOR_CONTROL_A; motor < MOTOR_CONTROL_COUNT; motor++)
    {
        channel = &motor_channels[motor];
        channel->actual = Encoder_Update(channel->encoder_motor);

        if (channel->enabled == 0U)
        {
            channel->duty = 0;
            PID_Reset(&channel->pid);
            DRV8701_SetDuty(channel->driver_motor, 0);
            continue;
        }

        output = PID_Update(&channel->pid, (float)channel->target, (float)channel->actual);
        channel->duty = MotorControl_OutputToDuty(output);
        DRV8701_SetDuty(channel->driver_motor, (int16_t)channel->duty);
    }
}

void MotorControl_SetTarget(MotorControl_Motor_t motor, int32_t target)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return;
    }

    motor_channels[motor].target = target;
}

void MotorControl_SetTargetAll(int32_t target_a,
                               int32_t target_b,
                               int32_t target_c,
                               int32_t target_d)
{
    MotorControl_SetTarget(MOTOR_CONTROL_A, target_a);
    MotorControl_SetTarget(MOTOR_CONTROL_B, target_b);
    MotorControl_SetTarget(MOTOR_CONTROL_C, target_c);
    MotorControl_SetTarget(MOTOR_CONTROL_D, target_d);
}

int32_t MotorControl_GetTarget(MotorControl_Motor_t motor)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return 0;
    }

    return motor_channels[motor].target;
}

int32_t MotorControl_GetActual(MotorControl_Motor_t motor)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return 0;
    }

    return motor_channels[motor].actual;
}

int32_t MotorControl_GetDuty(MotorControl_Motor_t motor)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return 0;
    }

    return motor_channels[motor].duty;
}

void MotorControl_SetPID(MotorControl_Motor_t motor, float kp, float ki, float kd)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return;
    }

    PID_SetTunings(&motor_channels[motor].pid, kp, ki, kd);
}

void MotorControl_SetDutyLimit(MotorControl_Motor_t motor, int32_t duty_limit)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return;
    }

    if (duty_limit < 0)
    {
        duty_limit = -duty_limit;
    }

    if (duty_limit > DRV8701_DUTY_MAX)
    {
        duty_limit = DRV8701_DUTY_MAX;
    }

    PID_SetOutputLimits(&motor_channels[motor].pid,
                        (float)(-duty_limit),
                        (float)duty_limit);
}

void MotorControl_SetDutyLimitAll(int32_t duty_limit)
{
    for (MotorControl_Motor_t motor = MOTOR_CONTROL_A; motor < MOTOR_CONTROL_COUNT; motor++)
    {
        MotorControl_SetDutyLimit(motor, duty_limit);
    }
}

void MotorControl_ResetPID(MotorControl_Motor_t motor)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return;
    }

    PID_Reset(&motor_channels[motor].pid);
}

void MotorControl_ResetAllPID(void)
{
    for (MotorControl_Motor_t motor = MOTOR_CONTROL_A; motor < MOTOR_CONTROL_COUNT; motor++)
    {
        MotorControl_ResetPID(motor);
    }
}

void MotorControl_Enable(MotorControl_Motor_t motor)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return;
    }

    motor_channels[motor].enabled = 1U;
    DRV8701_Enable(motor_channels[motor].driver_motor);
}

void MotorControl_Disable(MotorControl_Motor_t motor)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return;
    }

    motor_channels[motor].enabled = 0U;
    motor_channels[motor].duty = 0;
    PID_Reset(&motor_channels[motor].pid);
    DRV8701_Disable(motor_channels[motor].driver_motor);
}

void MotorControl_EnableAll(void)
{
    for (MotorControl_Motor_t motor = MOTOR_CONTROL_A; motor < MOTOR_CONTROL_COUNT; motor++)
    {
        MotorControl_Enable(motor);
    }
}

void MotorControl_DisableAll(void)
{
    for (MotorControl_Motor_t motor = MOTOR_CONTROL_A; motor < MOTOR_CONTROL_COUNT; motor++)
    {
        MotorControl_Disable(motor);
    }
}

void MotorControl_StopAll(void)
{
    MotorControl_SetTargetAll(0, 0, 0, 0);
    MotorControl_ResetAllPID();
    DRV8701_StopAll();

    for (MotorControl_Motor_t motor = MOTOR_CONTROL_A; motor < MOTOR_CONTROL_COUNT; motor++)
    {
        motor_channels[motor].duty = 0;
    }
}

void MotorControl_SetMotorDirectionInverted(MotorControl_Motor_t motor, uint8_t inverted)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return;
    }

    DRV8701_SetDirectionInverted(motor_channels[motor].driver_motor, inverted);
}

void MotorControl_SetEncoderDirectionInverted(MotorControl_Motor_t motor, uint8_t inverted)
{
    if (!MotorControl_IsValidMotor(motor))
    {
        return;
    }

    Encoder_SetDirectionInverted(motor_channels[motor].encoder_motor, inverted);
}

void MotorControl_GetSerialPlotData(int32_t target[4],
                                    int32_t actual[4],
                                    int32_t duty[4])
{
    if ((target == 0) || (actual == 0) || (duty == 0))
    {
        return;
    }

    for (MotorControl_Motor_t motor = MOTOR_CONTROL_A; motor < MOTOR_CONTROL_COUNT; motor++)
    {
        target[motor] = motor_channels[motor].target;
        actual[motor] = motor_channels[motor].actual;
        duty[motor] = motor_channels[motor].duty;
    }
}
