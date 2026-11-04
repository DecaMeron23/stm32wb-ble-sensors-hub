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
#include "custom_app.h"
#include "app_common.h"
#include "ble.h"
#include "cmsis_gcc.h"
#include "custom_stm.h"
#include "stm32_seq.h"
#include "stm32wbxx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// #include "helpers/helpers.h"
extern RTC_HandleTypeDef hrtc;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* INFO */
  /* SENSORS */
  uint8_t               Temp_Notification_Status;
  /* USER CODE BEGIN CUSTOM_APP_Context_t */

  /* USER CODE END CUSTOM_APP_Context_t */

  uint16_t              ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

__USED static Custom_App_Context_t Custom_App_Context;

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
/* SENSORS */
static void Custom_Temp_Update_Char(void);

static void Custom_Temp_Send_Notification(void);

/* USER CODE BEGIN PFP */

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

    /* SENSORS */
    case CUSTOM_STM_TEMP_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_TEMP_READ_EVT */

      /* USER CODE END CUSTOM_STM_TEMP_READ_EVT */
      break;

    case CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */

      /* USER CODE END CUSTOM_STM_TEMP_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT */

      /* USER CODE END CUSTOM_STM_TEMP_NOTIFY_DISABLED_EVT */
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

  /* USER CODE END CUSTOM_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* INFO */
/* SENSORS */
__USED void Custom_Temp_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Temp_UC_1*/

  /* USER CODE END Temp_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_TEMP, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Temp_UC_Last*/

  /* USER CODE END Temp_UC_Last*/
  return;
}

__USED void Custom_Temp_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Temp_NS_1*/

  /* USER CODE END Temp_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_TEMP, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Temp_NS_Last*/

  /* USER CODE END Temp_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
