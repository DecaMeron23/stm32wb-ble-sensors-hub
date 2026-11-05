#include "STTS22H.h"
#include "stdbool.h"
extern I2C_HandleTypeDef STTS22H_IC2;

#define STTS22H_REG_WHOAMI              (0x01)
#define STTS22H_REG_TEMP_H_LIMIT        (0x02)
#define STTS22H_REG_TEMP_L_LIMIT        (0x03)
#define STTS22H_REG_CTRL                (0x04)
#define STTS22H_REG_STATUS              (0x05)
#define STTS22H_REG_TEMP_L_OUT          (0x06)
#define STTS22H_REG_TEMP_H_OUT          (0x07)

#define STTS22H_CTRL_CONSTANT           (0x4A)
#define STTS22H_CTRL_OPT(opt)           (opt == STTS22H_MODE_ONE_SHOT ? 0x01 : (opt == STTS22H_MODE_FREE_RUN ? 0x04 : 0x80))
#define STTS22H_CTRL_AVG(mean)          (mean << 4)
#define STTS22H_CTRL_VALUE(opt , mean)  (STTS22H_CTRL_CONSTANT | STTS22H_CTRL_OPT(opt) | STTS22H_CTRL_AVG(mean))


static STTS22H_Status write_register(uint16_t mem_address, uint8_t *pData, uint16_t size);
static STTS22H_Status read_register(uint16_t mem_address, uint8_t *pData, uint16_t size);


STTS22H_Status STTS22H_Init(STTS22H_HandleTypeDef *hstts){
    if(hstts == NULL){
        return STTS22H_STATUS_NULL_ERR;
    }

    hstts->opt_mode = STTS22H_MODE_ONE_SHOT;
    hstts->mean = STTS22H_MEAN_8;

    uint8_t who_i_am = 0;
    STTS22H_Status status = read_register(STTS22H_REG_WHOAMI, &who_i_am, 1);
    if(status != STTS22H_STATUS_OK){
        return status;
    }
    if (who_i_am != STTS22H_WHO_I_AM){
        return STTS22H_STATUS_GEN_ERR;
    }
    return STTS22H_STATUS_OK;
}

STTS22H_Status STTS22H_ChangeMode(STTS22H_HandleTypeDef *hstts , STTS22H_OperatingModes opt_mode , STTS22H_MEAN mean){
    if(hstts == NULL){
        return STTS22H_STATUS_NULL_ERR;
    }
    if(opt_mode == hstts->opt_mode && mean == hstts->mean){
        return STTS22H_STATUS_OK;
    } 

    uint8_t data = STTS22H_CTRL_CONSTANT;
    STTS22H_Status status;
    status = write_register(STTS22H_REG_CTRL, &data, 1);
    
    data = STTS22H_CTRL_VALUE(opt_mode, mean);
    status = write_register(STTS22H_REG_CTRL, &data, 1);

    if(status == STTS22H_STATUS_OK){
        hstts->opt_mode = opt_mode;
        hstts->mean = mean;
    }
    return status;
}

STTS22H_Status STTS22H_ReadTemp(STTS22H_HandleTypeDef *hstts , int16_t *temp){
    if(hstts == NULL || temp == NULL){
        return STTS22H_STATUS_NULL_ERR;
    }
    
    uint8_t p_value[2] = {0, 0};

    STTS22H_Status status = read_register(STTS22H_REG_TEMP_L_OUT, p_value, 2);

    *temp = (int16_t)((uint16_t)(p_value[1] << 8) | p_value[0]);
    return status;
}

static STTS22H_Status write_register(uint16_t mem_address, uint8_t *pData, uint16_t size){
    bool result = true;
    result &= HAL_I2C_Mem_Write(&STTS22H_IC2, STTS22H_ADDR_I2C, mem_address, 1, pData, size, HAL_MAX_DELAY) == HAL_OK;
    return (result ? STTS22H_STATUS_OK : STTS22H_STATUS_I2C_ERR);
}

static STTS22H_Status read_register(uint16_t mem_address, uint8_t *pData, uint16_t size){
    bool result = true;
    result &= HAL_I2C_Mem_Read(&STTS22H_IC2, STTS22H_ADDR_I2C, mem_address, 1, pData, size, HAL_MAX_DELAY) == HAL_OK;
    return (result ? STTS22H_STATUS_OK : STTS22H_STATUS_I2C_ERR);
}