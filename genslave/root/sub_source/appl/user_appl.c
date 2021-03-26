{{ signature -}}
#include "user_appl.h"
#include "OD_access.h"

/* USER CODE BEGIN Includes */
{{ user['Includes'] }}
/* USER CODE END Includes */

/* USER CODE BEGIN 0 */
{{ user['0'] }}
/* USER CODE END 0 */

// Call before send PDO data
uint32_t Canopen_Application_BeforeSendData(uint16_t Index, uint8_t subIndex){
  /* USER CODE BEGIN 1 */
{{ user['1'] }}
  /* USER CODE END 1 */

  return 0;
}

/* USER CODE BEGIN 2 */
{{ user['2'] }}
/* USER CODE END 2 */

// Call after get PDO data successfully
uint32_t Canopen_Application_AfterGetData(uint16_t Index, uint8_t subIndex){
  /* USER CODE BEGIN 3 */
{{ user['3'] }}
  /* USER CODE END 3 */

  return 0; 
}