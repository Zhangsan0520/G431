#include "serial.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "usart.h"

HAL_StatusTypeDef Serial_SendBytes(const uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0U))
    {
        return HAL_OK;
    }

    return HAL_UART_Transmit(&huart2, (uint8_t *)data, length, SERIAL_TX_TIMEOUT_MS);
}

HAL_StatusTypeDef Serial_SendString(const char *str)
{
    if (str == NULL)
    {
        return HAL_OK;
    }

    return Serial_SendBytes((const uint8_t *)str, (uint16_t)strlen(str));
}

HAL_StatusTypeDef Serial_Printf(const char *format, ...)
{
    char buffer[SERIAL_PRINTF_BUFFER_SIZE];
    va_list args;
    int length;

    if (format == NULL)
    {
        return HAL_OK;
    }

    va_start(args, format);
    length = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (length <= 0)
    {
        return HAL_OK;
    }

    if (length >= (int)sizeof(buffer))
    {
        length = (int)sizeof(buffer) - 1;
    }

    return Serial_SendBytes((const uint8_t *)buffer, (uint16_t)length);
}

HAL_StatusTypeDef Vofa_Send3(int32_t target, int32_t actual, int32_t duty)
{
    return Serial_Printf("d:%ld,%ld,%ld\r\n",
                         (long)target,
                         (long)actual,
                         (long)duty);
}

HAL_StatusTypeDef Vofa_Send2Motor(int32_t target_a,
                                  int32_t actual_a,
                                  int32_t duty_a,
                                  int32_t target_d,
                                  int32_t actual_d,
                                  int32_t duty_d)
{
    return Serial_Printf("d:%ld,%ld,%ld,%ld,%ld,%ld\r\n",
                         (long)target_a,
                         (long)actual_a,
                         (long)duty_a,
                         (long)target_d,
                         (long)actual_d,
                         (long)duty_d);
}

HAL_StatusTypeDef Vofa_Send4Motor(const int32_t target[4],
                                  const int32_t actual[4],
                                  const int32_t duty[4])
{
    if ((target == NULL) || (actual == NULL) || (duty == NULL))
    {
        return HAL_OK;
    }

    return Serial_Printf("d:%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\r\n",
                         (long)target[0], (long)actual[0], (long)duty[0],
                         (long)target[1], (long)actual[1], (long)duty[1],
                         (long)target[2], (long)actual[2], (long)duty[2],
                         (long)target[3], (long)actual[3], (long)duty[3]);
}

HAL_StatusTypeDef SerialPlot_Send3(int32_t target, int32_t actual, int32_t duty)
{
    return Vofa_Send3(target, actual, duty);
}

HAL_StatusTypeDef SerialPlot_Send2Motor(int32_t target_a,
                                        int32_t actual_a,
                                        int32_t duty_a,
                                        int32_t target_d,
                                        int32_t actual_d,
                                        int32_t duty_d)
{
    return Vofa_Send2Motor(target_a, actual_a, duty_a, target_d, actual_d, duty_d);
}

HAL_StatusTypeDef SerialPlot_Send4Motor(const int32_t target[4],
                                        const int32_t actual[4],
                                        const int32_t duty[4])
{
    return Vofa_Send4Motor(target, actual, duty);
}
