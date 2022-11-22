/*
 * Copyright 2021 BLUESINK Co., Ltd.
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 *
 */

#include "CANOpen_hw_appl.h"

/* USER CODE BEGIN Includes */
{{ user['Includes'] }}
/* USER CODE END Includes */

/* Private typedef */
/* USER CODE BEGIN PD */
{{ user['PD'] }}
/* USER CODE END PD */

/* USER CODE BEGIN 0 */
{{ user['0'] }}
/* USER CODE END 0 */  


void CANOpen_sendFrame(uint16_t cobID, uint8_t* data, uint8_t len, uint8_t rtr, void* privData){

  /* USER CODE BEGIN 1 */
{{ user['1'] }}
  /* USER CODE END 1 */  

}

int CANOpen_mutexLock(void* privData){

  /* USER CODE BEGIN 2 */
{{ user['2'] }}
  /* USER CODE END 2 */  

}

int CANOpen_mutexUnlock(void* privData){

  /* USER CODE BEGIN 3 */
{{ user['3'] }}
  /* USER CODE END 3 */  

}

/* USER CODE BEGIN 4 */
{{ user['4'] }}
/* USER CODE END 4 */ 