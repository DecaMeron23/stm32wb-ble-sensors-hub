#ifndef __STTS22H_H
#define __STTS22H_H

#include <main.h>

#define STTS22H_SCALING_FATTOR 100          // LSB/°C
#define STTS22H_TEMPERATURE_LIMIT(temp) (uint8_t ((temp / 0.64F) + 63))

#define STTS22H_IC2 hi2c3;

typedef enum{
    STTS22H_MODE_ONE_SHOT,
    STTS22H_MODE_FREE_RUN,
    STTS22H_MODE_LOW_ODR
}STTS22H_OperatingModes;

typedef enum{
    STTS22H_ODR_25Hz,
    STTS22H_ODR_50Hz,
    STTS22H_ODR_100Hz,
    STTS22H_ODR_200Hz,
    STTS22H_ODR_NO_ODR,
}STTS22H_ODR;

typedef struct{
    STTS22H_OperatingModes opt_mode;
    STTS22H_ODR odr;
}STTS22H_HandleTypeDef;

void STTS22H_Init(STTS22H_HandleTypeDef *hstts);

void STTS22H_ChangeMode(STTS22H_HandleTypeDef *hstts , STTS22H_OperatingModes opt_mode , STTS22H_ODR odr);

void STTS22H_ReadTemp(STTS22H_HandleTypeDef *hstts , int16_t *temp);

void STTS22H_SetTempHLimit(STTS22H_HandleTypeDef *hstts , int8_t temp_h);

void STTS22H_DeleteTempHLimit(STTS22H_HandleTypeDef *hstts);

void STTS22H_SetTempLLimit(STTS22H_HandleTypeDef *hstts , int8_t temp_l);

void STTS22H_DeleteTempLLimit(STTS22H_HandleTypeDef *hstts);

#endif