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
        return STTS22H_STATUS_ERR_NULL;
    }

    hstts->opt_mode = STTS22H_MODE_ONE_SHOT;
    hstts->mean = STTS22H_MEAN_8;

    uint8_t who_i_am = 0;
    read_register(STTS22H_REG_WHOAMI, &who_i_am, 1);
    
    if (who_i_am != STTS22H_WHO_I_AM){
        return STTS22H_STATUS_GEN_ERR;
    }
    return STTS22H_STATUS_OK;
}

static STTS22H_Status write_register(uint16_t mem_address, uint8_t *pData, uint16_t size){
    bool result = true;
    result &= HAL_I2C_Mem_Write(&STTS22H_IC2, STTS22H_ADDR_I2C, mem_address, 1, pData, size, HAL_MAX_DELAY) == HAL_OK;
    return (result ? STTS22H_STATUS_OK : STTS22H_STATUS_GEN_ERR);
}

static STTS22H_Status read_register(uint16_t mem_address, uint8_t *pData, uint16_t size){
    bool result = true;
    result &= HAL_I2C_Mem_Read(&STTS22H_IC2, STTS22H_ADDR_I2C, mem_address, 1, pData, size, HAL_MAX_DELAY) == HAL_OK;
    return (result ? STTS22H_STATUS_OK : STTS22H_STATUS_GEN_ERR);
}