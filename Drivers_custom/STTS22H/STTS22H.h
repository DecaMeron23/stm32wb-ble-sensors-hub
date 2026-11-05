#ifndef __STTS22H_H
#define __STTS22H_H

#include <main.h>

#define STTS22H_ADDR_I2C (0x70)
#define STTS22H_IC2 (hi2c3)

#define STTS22H_WHO_I_AM (0xA0)


typedef enum{
    STTS22H_MODE_ONE_SHOT,
    STTS22H_MODE_FREE_RUN,
    STTS22H_MODE_LOW_ODR
}STTS22H_OperatingModes;

typedef enum{
    STTS22H_ODR_25Hz = 0,
    STTS22H_ODR_50Hz = 1,
    STTS22H_ODR_100Hz = 2,
    STTS22H_ODR_200Hz = 3,
}STTS22H_ODR;

typedef enum{
    STTS22H_MEAN_8 = 0,     // Equivalent of ODR 25Hz
    STTS22H_MEAN_4 = 1,     // Equivalent of ODR 50Hz
    STTS22H_MEAN_2 = 2,     // Equivalent of ODR 100Hz
    STTS22H_MEAN_1 = 3,     // Equivalent of ODR 200Hz
}STTS22H_MEAN;

typedef enum{
    STTS22H_STATUS_OK,          // OK
    STTS22H_STATUS_NULL_ERR,    // Tring to access to a NULL pointer
    STTS22H_STATUS_I2C_ERR,    // Tring to access to a NULL pointer
    STTS22H_STATUS_GEN_ERR,     // Generic Error
}STTS22H_Status;

typedef enum{
    STTS22H_EVENT_NONE      = 0,
    STTS22H_EVENT_OVER_THH  = (1U << 0), 
    STTS22H_EVENT_UNDER_THL = (1U << 1),
    STTS22H_EVENT_BOTH      = STTS22H_EVENT_UNDER_THL | STTS22H_EVENT_OVER_THH
}STTS22H_EventCode;

typedef struct{
    STTS22H_OperatingModes opt_mode;
    STTS22H_MEAN mean;
}STTS22H_HandleTypeDef;

STTS22H_Status STTS22H_Init(STTS22H_HandleTypeDef *hstts);

STTS22H_Status STTS22H_ChangeMode(STTS22H_HandleTypeDef *hstts , STTS22H_OperatingModes opt_mode , STTS22H_MEAN mean);

STTS22H_Status STTS22H_ReadTemp(STTS22H_HandleTypeDef *hstts , int16_t *temp);

STTS22H_Status STTS22H_SetTempHLimit(STTS22H_HandleTypeDef *hstts , int8_t temp_h);

STTS22H_Status STTS22H_DeleteTempHLimit(STTS22H_HandleTypeDef *hstts);

STTS22H_Status STTS22H_SetTempLLimit(STTS22H_HandleTypeDef *hstts , int8_t temp_l);

STTS22H_Status STTS22H_DeleteTempLLimit(STTS22H_HandleTypeDef *hstts);

STTS22H_Status STTS22H_ReadEvent(STTS22H_HandleTypeDef *hstts , STTS22H_EventCode *event);

#endif