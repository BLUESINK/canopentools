{{ signature -}}
#include "sdo_appl.h"
#include "OD_access.h"

#include "cia301.h"

/* USER CODE BEGIN Includes */
{{ user['Includes'] }}
/* USER CODE END Includes */

/* USER CODE BEGIN 0 */
{{ user['0'] }}
/* USER CODE END 0 */

// Call before send SDO data
uint32_t Canopen_SDOapplication_BeforeSendData(uint16_t Index, uint8_t subIndex){

  /* USER CODE BEGIN 1 */
{{ user['1'] }}
  /* USER CODE END 1 */  

  return 0;
}

// Call after get SDO data successfully
uint32_t Canopen_SDOapplication_AfterGetData(uint16_t Index, uint8_t subIndex){

  uint32_t abortCode = 0;
  uint8_t go_next;

  // Pre-check device profiles
  abortCode = cia301_proc(Index, subIndex, &go_next); if(abortCode != 0 || go_next == 0) return abortCode;

  /* USER CODE BEGIN 2 */
{{ user['2'] }}
  /* USER CODE END 2 */ 

  return 0; 
}