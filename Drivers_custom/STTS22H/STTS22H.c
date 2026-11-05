#include "STTS22H.h"

extern I2C_HandleTypeDef STTS22H_IC2;

#define STTS22H_REG_WHOAMI (0x01)

STTS22H_Status STTS22H_Init(STTS22H_HandleTypeDef *hstts){
    if(hstts == NULL){
        return STTS22H_STATUS_ERR_NULL;
    }

    uint8_t who_i_am = 0;
    HAL_I2C_Mem_Read(&STTS22H_IC2, STTS22H_ADDR_I2C, STTS22H_REG_WHOAMI, 1, &who_i_am, 1, 100);
    
    if (who_i_am != STTS22H_WHO_I_AM){
        return STTS22H_STATUS_GEN_ERR;
    }
    return STTS22H_STATUS_OK;
}

