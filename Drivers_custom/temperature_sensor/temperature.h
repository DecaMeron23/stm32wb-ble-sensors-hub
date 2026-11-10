#ifndef __TEMPERATURE_H
#define __TEMPERATURE_H

#include "STTS22H/STTS22H.h"
#include "stdbool.h"

typedef struct{
    bool is_periodic;
    STTS22H_HandleTypeDef hstts;
} Temperature_TypeDef;

bool Temperature_Init(Temperature_TypeDef *htemp);
bool Temperature_ReadTemp(Temperature_TypeDef *htemp , int16_t *temp);
bool Temperature_StartPeriodicMeasure(Temperature_TypeDef *htemp , uint32_t ms_period);
bool Temperature_StopPeriodicMeasure(Temperature_TypeDef *htemp);

bool Temperature_SetHighLimit(Temperature_TypeDef *htemp,  int8_t temp);
bool Temperature_SetLowLimit(Temperature_TypeDef *htemp, int8_t temp);
bool Temperature_StopHighLimit(Temperature_TypeDef *htemp);
bool Temperature_StopLowLimit(Temperature_TypeDef *htemp);

#endif 