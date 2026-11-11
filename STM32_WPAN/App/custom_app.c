/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    App/custom_app.c
 * @author  MCD Application Team
 * @brief   Custom Example Application (Server)
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "custom_app.h"
#include "custom_stm.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "temperature_sensor/temperature.h"
extern RTC_HandleTypeDef hrtc;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* INFO */
  /* TEMPERATURE */
  uint8_t               Temp_char_Notification_Status;
  uint8_t               Alt_temp_char_Notification_Status;
  /* USER CODE BEGIN CUSTOM_APP_Context_t */
  uint8_t               hw_ts_id_timer_temp;
  uint32_t              timeout_notify_temp;        // [ms]

  Temperature_TypeDef   temperature_sensor;
  /* USER CODE END CUSTOM_APP_Context_t */

  uint16_t              ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CUSTOM_APP_DEFAULT_TIMEOUT_ACC  (5000)      // [ms]
#define CUSTOM_APP_DEFAULT_TIMEOUT_TEMP (5000)      // [ms]

#define CUSTOM_APP_CMD_ALT_TEMP_OFF     (0x01)
#define CUSTOM_APP_CMD_ALT_TEMP_ON      (0x02)
/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define CUSTOM_APP_HW_TS_WAIT_MILLIS(ms)          ((uint32_t) (ms) * 1000U / CFG_TS_TICK_VAL)

// COMAND WRITE TEMPERATURE MACRO
#define CUSTOM_APP_CMD_TEMP_GET_UNIT(cmd)         ((cmd & 0xC0) >> 6)
#define CUSTOM_APP_CMD_TEMP_IS_VALID(cmd)         (CUSTOM_APP_CMD_TEMP_GET_UNIT(cmd) != 0x00)
#define CUSTOM_APP_CMD_TEMP_UNIT_MULT(cmd)        (CUSTOM_APP_CMD_TEMP_GET_UNIT(cmd) == 0x01 ? (1) : (CUSTOM_APP_CMD_TEMP_GET_UNIT(cmd) == 0x02 ? (60) : (60*60))) // seconds
#define CUSTOM_APP_CMD_TEMP_VALUE(cmd)            (cmd & 0x3F) // seconds

#define CUSTOM_APP_CMD_ALT_TEMP_THL_CMD(cmd)      ((cmd >> 4) & 0x0F)
#define CUSTOM_APP_CMD_ALT_TEMP_THH_CMD(cmd)      (cmd & 0x0F)
#define CUSTOM_APP_CMD_ALT_TEMP_CHANGE(cmd)       ((cmd == CUSTOM_APP_CMD_ALT_TEMP_OFF) || (cmd == CUSTOM_APP_CMD_ALT_TEMP_ON))

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

static Custom_App_Context_t Custom_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

uint8_t UpdateCharData[512];
uint8_t NotifyCharData[512];
uint16_t Connection_Handle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* INFO */
/* TEMPERATURE */
static void Custom_Temp_char_Update_Char(void);

static void Custom_Temp_char_Send_Notification(void);

static void Custom_Alt_temp_char_Update_Char(void);

static void Custom_Alt_temp_char_Send_Notification(void);

/* USER CODE BEGIN PFP */
// static void Custom_App_AccRead();
static void Custom_App_TempRead();
static void Custom_App_AlertTemp();

static void Custom_App_TempControl(uint8_t *data , uint8_t size);
static void Custom_App_TempAlertControl(uint8_t *data , uint8_t size);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void Custom_STM_App_Notification(Custom_STM_App_Notification_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_1 */
    RTC_TimeTypeDef time = {0};
    RTC_DateTypeDef date = {0};
  /* USER CODE END CUSTOM_STM_App_Notification_1 */
  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* USER CODE END CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* INFO */
    case CUSTOM_STM_UPTIME_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_UPTIME_READ_EVT */
    uint32_t uptime = HAL_GetTick() / 1000;
    uint8_t array_uptime[4] = {uptime & 0xFF, uptime >> 8 & 0xFF,
                               uptime >> 16 & 0xFF, uptime >> 24 & 0xFF};
    Custom_STM_App_Update_Char(CUSTOM_STM_UPTIME, array_uptime);
      /* USER CODE END CUSTOM_STM_UPTIME_READ_EVT */
      break;

    case CUSTOM_STM_DATETIME_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_DATETIME_READ_EVT */
      HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
      HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
      uint8_t payload[7] = {time.Hours,   time.Minutes, time.Seconds, date.WeekDay,
                            date.Date,    date.Month,   date.Year};
      Custom_STM_App_Update_Char(CUSTOM_STM_DATETIME, payload);
      /* USER CODE END CUSTOM_STM_DATETIME_READ_EVT */
      break;

    case CUSTOM_STM_DATETIME_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN CUSTOM_STM_DATETIME_WRITE_NO_RESP_EVT */
      time.Hours = pNotification->DataTransfered.pPayload[0];
	    time.Minutes = pNotification->DataTransfered.pPayload[1];
	    time.Seconds = pNotification->DataTransfered.pPayload[2];
	    time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	    time.StoreOperation = RTC_STOREOPERATION_RESET;
      time.TimeFormat = RTC_HOURFORMAT_24;

      date.WeekDay = pNotification->DataTransfered.pPayload[3];
      date.Date = pNotification->DataTransfered.pPayload[4];
      date.Month = pNotification->DataTransfered.pPayload[5];
      date.Year = pNotification->DataTransfered.pPayload[6];

      HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BCD);
      HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BCD);

      /* USER CODE END CUSTOM_STM_DATETIME_WRITE_NO_RESP_EVT */
      break;

    /* TEMPERATURE */
    case CUSTOM_STM_TEMP_CHAR_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_TEMP_CHAR_READ_EVT */
      Custom_App_TempRead();
      /* USER CODE END CUSTOM_STM_TEMP_CHAR_READ_EVT */
      break;

    case CUSTOM_STM_TEMP_CHAR_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_TEMP_CHAR_NOTIFY_ENABLED_EVT */
      Custom_App_Context.Temp_char_Notification_Status = ENABLE;
      HW_TS_Start(Custom_App_Context.hw_ts_id_timer_temp, Custom_App_Context.timeout_notify_temp);
      /* USER CODE END CUSTOM_STM_TEMP_CHAR_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_TEMP_CHAR_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_TEMP_CHAR_NOTIFY_DISABLED_EVT */
      Custom_App_Context.Temp_char_Notification_Status = DISABLE;
      HW_TS_Stop(Custom_App_Context.hw_ts_id_timer_temp);
      /* USER CODE END CUSTOM_STM_TEMP_CHAR_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_CTRL_TEMP_CHAR_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN CUSTOM_STM_CTRL_TEMP_CHAR_WRITE_NO_RESP_EVT */
      Custom_App_TempControl(pNotification->DataTransfered.pPayload, pNotification->DataTransfered.Length);
      /* USER CODE END CUSTOM_STM_CTRL_TEMP_CHAR_WRITE_NO_RESP_EVT */
      break;

    case CUSTOM_STM_ALT_TEMP_CHAR_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_ALT_TEMP_CHAR_NOTIFY_ENABLED_EVT */
      Custom_App_Context.Alt_temp_char_Notification_Status = ENABLE;
      /* USER CODE END CUSTOM_STM_ALT_TEMP_CHAR_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_ALT_TEMP_CHAR_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_ALT_TEMP_CHAR_NOTIFY_DISABLED_EVT */
      Custom_App_Context.Alt_temp_char_Notification_Status = DISABLE;
      /* USER CODE END CUSTOM_STM_ALT_TEMP_CHAR_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_CTRL_ALT_CHAR_WRITE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_CTRL_ALT_CHAR_WRITE_EVT */
      Custom_App_TempAlertControl(pNotification->DataTransfered.pPayload , pNotification->DataTransfered.Length);
      /* USER CODE END CUSTOM_STM_CTRL_ALT_CHAR_WRITE_EVT */
      break;

    case CUSTOM_STM_NOTIFICATION_COMPLETE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */

      /* USER CODE END CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_STM_App_Notification_default */

      /* USER CODE END CUSTOM_STM_App_Notification_default */
      break;
  }
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_2 */

  /* USER CODE END CUSTOM_STM_App_Notification_2 */
  return;
}

void Custom_APP_Notification(Custom_App_ConnHandle_Not_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_APP_Notification_1 */

  /* USER CODE END CUSTOM_APP_Notification_1 */

  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_APP_Notification_Custom_Evt_Opcode */

    /* USER CODE END P2PS_CUSTOM_Notification_Custom_Evt_Opcode */
    case CUSTOM_CONN_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_CONN_HANDLE_EVT */

      /* USER CODE END CUSTOM_CONN_HANDLE_EVT */
      break;

    case CUSTOM_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_DISCON_HANDLE_EVT */

      /* USER CODE END CUSTOM_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_APP_Notification_default */

      /* USER CODE END CUSTOM_APP_Notification_default */
      break;
  }

  /* USER CODE BEGIN CUSTOM_APP_Notification_2 */

  /* USER CODE END CUSTOM_APP_Notification_2 */

  return;
}

void Custom_APP_Init(void)
{
  /* USER CODE BEGIN CUSTOM_APP_Init */
  // Custom_App_Context.hw_ts_timeout_acc = CUSTOM_APP_DEFAULT_TIMEOUT_ACC;
  // UTIL_SEQ_RegTask(1 << CFG_TASK_READ_ACC_EVENT_ID, 0 , Custom_App_AccRead);
  // HW_TS_Create(CFG_TASK_READ_ACC_EVENT_ID, &Custom_App_Context.hw_ts_id_timer_acc, hw_ts_SingleShot, NULL);
  
  Custom_App_Context.timeout_notify_temp = CUSTOM_APP_DEFAULT_TIMEOUT_TEMP;
  UTIL_SEQ_RegTask(1 << CFG_TASK_READ_TEMP_EVENT_ID, 0 , Custom_App_TempRead);
  HW_TS_Create(CFG_TASK_READ_TEMP_EVENT_ID, &Custom_App_Context.hw_ts_id_timer_temp, hw_ts_SingleShot, NULL);

  UTIL_SEQ_RegTask(1 << CFG_TASK_ALT_TEMP_EVENT_ID, 0 , Custom_App_AlertTemp);

  Temperature_Init(&Custom_App_Context.temperature_sensor);
  /* USER CODE END CUSTOM_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void HW_TS_RTC_Int_AppNot(uint32_t TimerProcessID, uint8_t TimerID, HW_TS_pTimerCb_t pTimerCallBack)
{
  if(TimerProcessID == CFG_TIM_PROC_ID_ISR){
    if(pTimerCallBack != NULL){
      pTimerCallBack();
    }
  } else {
    UTIL_SEQ_SetTask(1 << TimerProcessID, CFG_SCH_PRIO_SENSORS);
  }
  return;
}

// static void Custom_App_AccRead(){
//   // // Get DATA
//   // uint8_t dummy_data_acc[6] = {0xFF , 0xEE , 0xDD , 0xCC , 0xBB , HAL_GetTick() / 1000 & 0xFF};

//   // // Send DATA
//   // Custom_STM_App_Update_Char(CUSTOM_STM_ACC, dummy_data_acc);

//   // if(Custom_App_Context.Acc_Notification_Status == ENABLE){
//   //   HW_TS_Start(Custom_App_Context.hw_ts_id_timer_acc, CUSTOM_APP_HW_TS_WAIT_MILLIS(Custom_App_Context.hw_ts_timeout_acc));
//   // }
// }

static void Custom_App_TempRead(){
  // Get DATA
  int16_t temp = 0;
  Temperature_ReadTemp(&Custom_App_Context.temperature_sensor, &temp);
  uint16_t raw = (uint16_t) temp;
  uint8_t value[2] = {raw & 0xFF , (raw >> 8) & 0XFF};

  // Send DATA
  Custom_STM_App_Update_Char(CUSTOM_STM_TEMP_CHAR, value);

  if(Custom_App_Context.Temp_char_Notification_Status == ENABLE){
    HW_TS_Start(Custom_App_Context.hw_ts_id_timer_temp, CUSTOM_APP_HW_TS_WAIT_MILLIS(Custom_App_Context.timeout_notify_temp));
  }
}

static void Custom_App_TempControl(uint8_t *data , uint8_t size){
  if(data == NULL || size != 1){
    return;
  }
  uint8_t cmd = data[0];

  if(CUSTOM_APP_CMD_TEMP_IS_VALID(cmd)){ // Change ODR
    uint32_t multiply = CUSTOM_APP_CMD_TEMP_UNIT_MULT(cmd) * 1000; // ms
    uint32_t timeout = multiply * CUSTOM_APP_CMD_TEMP_VALUE(cmd);
    if(timeout == 0){
      Custom_App_Context.timeout_notify_temp = CUSTOM_APP_DEFAULT_TIMEOUT_TEMP;
    } else {
      Custom_App_Context.timeout_notify_temp = timeout;
    }
    if(Custom_App_Context.Temp_char_Notification_Status == ENABLE){
      HW_TS_Stop(Custom_App_Context.hw_ts_id_timer_temp);
      HW_TS_Start(Custom_App_Context.hw_ts_id_timer_temp , Custom_App_Context.timeout_notify_temp);
    }
  }
}

static void Custom_App_AlertTemp(){
  uint8_t data = 0;
  int16_t temp = 0;
  Temperature_ReadTemp(&Custom_App_Context.temperature_sensor, &temp);
  Temperature_ReadEvent(&Custom_App_Context.temperature_sensor , &data);
  uint16_t raw = (uint16_t) temp;
  uint8_t value[3] = { data, raw & 0xFF, (raw >> 8) & 0XFF};
  if(Custom_App_Context.Alt_temp_char_Notification_Status == ENABLE){
    Custom_STM_App_Update_Char(CUSTOM_STM_ALT_TEMP_CHAR, value);
  }
}

static void Custom_App_TempAlertControl(uint8_t *data , uint8_t size){
  if(data == NULL || size != 3){
    return;
  }
  uint8_t cmd = data[0];
  int8_t thl = data[1];
  int8_t thh = data[2];

  if(CUSTOM_APP_CMD_ALT_TEMP_CHANGE(CUSTOM_APP_CMD_ALT_TEMP_THL_CMD(cmd))) {
    if(CUSTOM_APP_CMD_ALT_TEMP_THL_CMD(cmd) == CUSTOM_APP_CMD_ALT_TEMP_ON){
      Temperature_SetLowLimit(&Custom_App_Context.temperature_sensor, thl);
    } else {
      Temperature_StopLowLimit(&Custom_App_Context.temperature_sensor);
    }
  }

  if(CUSTOM_APP_CMD_ALT_TEMP_CHANGE(CUSTOM_APP_CMD_ALT_TEMP_THH_CMD(cmd))) {
    if(CUSTOM_APP_CMD_ALT_TEMP_THH_CMD(cmd) == CUSTOM_APP_CMD_ALT_TEMP_ON){
      Temperature_SetHighLimit(&Custom_App_Context.temperature_sensor, thh);
    } else {
      Temperature_StopHighLimit(&Custom_App_Context.temperature_sensor);
    }
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == STTS22H_ALERT_Pin){
    
  }
}

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* INFO */
/* TEMPERATURE */
__USED void Custom_Temp_char_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Temp_char_UC_1*/

  /* USER CODE END Temp_char_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_TEMP_CHAR, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Temp_char_UC_Last*/

  /* USER CODE END Temp_char_UC_Last*/
  return;
}

__USED void Custom_Temp_char_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Temp_char_NS_1*/

  /* USER CODE END Temp_char_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_TEMP_CHAR, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Temp_char_NS_Last*/

  /* USER CODE END Temp_char_NS_Last*/

  return;
}

__USED void Custom_Alt_temp_char_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Alt_temp_char_UC_1*/

  /* USER CODE END Alt_temp_char_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_ALT_TEMP_CHAR, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Alt_temp_char_UC_Last*/

  /* USER CODE END Alt_temp_char_UC_Last*/
  return;
}

__USED void Custom_Alt_temp_char_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Alt_temp_char_NS_1*/

  /* USER CODE END Alt_temp_char_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_ALT_TEMP_CHAR, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Alt_temp_char_NS_Last*/

  /* USER CODE END Alt_temp_char_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
