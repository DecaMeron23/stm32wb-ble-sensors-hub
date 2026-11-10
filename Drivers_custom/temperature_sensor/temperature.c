#include "temperature.h"

#define TEMPERATURE_ODRS (4)


void select_mode_and_odr(uint32_t freq , STTS22H_OperatingModes *mode , STTS22H_ODR *odr);

bool Temperature_Init(Temperature_TypeDef *htemp){
    if(htemp == NULL){
        return false;
    }
    htemp->is_periodic = false;

    return STTS22H_Init(&htemp->hstts) == STTS22H_STATUS_OK;
}

bool Temperature_ReadTemp(Temperature_TypeDef *htemp , int16_t *temp){
    if (htemp == NULL) {
        return false;
    }
    bool result = true;
    if(!htemp->is_periodic){
        result &= STTS22H_STATUS_OK == STTS22H_ChangeMode(&htemp->hstts, STTS22H_MODE_ONE_SHOT, STTS22H_MEAN_8);
    }
    result &= STTS22H_STATUS_OK == STTS22H_ReadTemp(&htemp->hstts, temp);
    return result;   
}

bool Temperature_StartPeriodicMeasure(Temperature_TypeDef *htemp , uint32_t ms_period){
    if (htemp == NULL) {
        return false;
    }
    bool result = true;
    uint32_t freq = 1000/ms_period; // [Hz]
    STTS22H_OperatingModes mode;
    STTS22H_ODR odr;
    select_mode_and_odr(freq , &mode , &odr);
    result &= STTS22H_STATUS_OK == STTS22H_ChangeMode(&htemp->hstts, mode, odr);
    if(mode != STTS22H_MODE_ONE_SHOT){
        htemp->is_periodic = true;
    }
    return result;
}

bool Temperature_StopPeriodicMeasure(Temperature_TypeDef *htemp){
    if (htemp == NULL) {
        return false;
    }
    bool result = true;
    STTS22H_ChangeMode(&htemp->hstts, STTS22H_MODE_OFF, STTS22H_MEAN_8);
    htemp->is_periodic = false;
    return result;
}

bool Temperature_SetHighLimit(Temperature_TypeDef *htemp,  int8_t temp){
    if(htemp == NULL){
        return false;
    }
    return STTS22H_STATUS_OK == STTS22H_SetTempHLimit(&htemp->hstts, temp);
}

bool Temperature_SetLowLimit(Temperature_TypeDef *htemp, int8_t temp){
    if(htemp == NULL){
        return false;
    }
    return STTS22H_STATUS_OK == STTS22H_SetTempLLimit(&htemp->hstts, temp);
}

bool Temperature_StopHighLimit(Temperature_TypeDef *htemp){
    if(htemp == NULL){
        return false;
    }
    return STTS22H_STATUS_OK == STTS22H_DeleteTempHLimit(&htemp->hstts);
}
bool Temperature_StopLowLimit(Temperature_TypeDef *htemp){
    if(htemp == NULL){
        return false;
    }
    return STTS22H_STATUS_OK == STTS22H_DeleteTempLLimit(&htemp->hstts);
}


bool Temperature_ReadEvent(Temperature_TypeDef *htemp , STTS22H_EventCode *event){
    if(htemp == NULL){
        return false;
    }
    return STTS22H_STATUS_OK == STTS22H_ReadEvent(&htemp->hstts, event);
}

// PRIVATE FUNCTIONS

void select_mode_and_odr(uint32_t freq , STTS22H_OperatingModes *mode , STTS22H_ODR *odr){
    if(freq == 0){
        *mode = STTS22H_MODE_ONE_SHOT;
        *odr = STTS22H_MEAN_8;
        return;
    }
    if(freq == 1){
        *mode = STTS22H_MODE_LOW_ODR;
        *odr = STTS22H_MEAN_8;    
        return;
    }
    *mode = STTS22H_MODE_FREE_RUN;
    uint8_t ODRs[TEMPERATURE_ODRS] = {25 , 50 , 100 , 200}; //[Hz]
    for(int i = 0; i < TEMPERATURE_ODRS ; i++){
        if(ODRs[i] <= freq){
            *odr = (STTS22H_ODR) i;
            return;
        }
    }
    *odr = STTS22H_ODR_200Hz;
    return;
}