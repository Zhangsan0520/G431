#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32g4xx_hal.h"

#define SERIAL_TX_TIMEOUT_MS 20U
#define SERIAL_PRINTF_BUFFER_SIZE 384U

HAL_StatusTypeDef Serial_SendBytes(const uint8_t *data, uint16_t length);
HAL_StatusTypeDef Serial_SendString(const char *str);
HAL_StatusTypeDef Serial_Printf(const char *format, ...);

HAL_StatusTypeDef Vofa_Send3(int32_t target, int32_t actual, int32_t duty);
HAL_StatusTypeDef Vofa_Send2Motor(int32_t target_a,
                                  int32_t actual_a,
                                  int32_t duty_a,
                                  int32_t target_d,
                                  int32_t actual_d,
                                  int32_t duty_d);
HAL_StatusTypeDef Vofa_Send4Motor(const int32_t target[4],
                                  const int32_t actual[4],
                                  const int32_t duty[4]);

HAL_StatusTypeDef SerialPlot_Send3(int32_t target, int32_t actual, int32_t duty);
HAL_StatusTypeDef SerialPlot_Send2Motor(int32_t target_a,
                                        int32_t actual_a,
                                        int32_t duty_a,
                                        int32_t target_d,
                                        int32_t actual_d,
                                        int32_t duty_d);
HAL_StatusTypeDef SerialPlot_Send4Motor(const int32_t target[4],
                                        const int32_t actual[4],
                                        const int32_t duty[4]);

#ifdef __cplusplus
}
#endif

#endif
