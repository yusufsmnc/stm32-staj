	/*
 * system_sm.h
 *
 *  Created on: Aug 12, 2026
 *      Author: Yusuf
 */

#ifndef INC_SYSTEM_SM_H_
#define INC_SYSTEM_SM_H_

#include "main.h"
#include "stdbool.h"
#include "power_meter.h"
#include "lcd_2x16_driver.h"
#include "display_sm.h"
#include "load_types.h"


/************** State Tanımları **************/

typedef enum{
	SYS_STATE_INIT		= 0,
	SYS_STATE_MEASURE      ,
	SYS_STATE_TRANSMIT     ,
	SYS_STATE_LOAD_CHANGE
}SysState_t;

/************** Yük Tipleri **************/

extern const float LOAD_FAZ_TABLE[LOAD_COUNT];
extern const char* LOAD_NAME_TABLE[LOAD_COUNT];

/************** System State Machine Struct **************/

typedef struct{
	SysState_t current;	  			// mevcut state
	SysState_t previous;      		// önceki state
	uint32_t   stateEnterTick;	    // state'e giriş zamanı
	LoadType_t loadIndex;	  		// aktif yük tipi
	bool	   buttonEvent;	        // buton basış olayı
	volatile bool lcdUpdateFlag;	// LCD guncelleme flag
	volatile bool uartTransmitFlag;	// UART transfer flag
	DisplaySM_t displaySM;			// Display'in kendi icindeki state machine
	uint32_t 	btnPressTime;		// kısa/uzun basıs algılama
}SystemSM_t;


/************** Fonksiyon Prototipleri **************/

void SystemSM_Init(SystemSM_t *sm);
void SystemSM_Run(SystemSM_t *sm, PowerMeter_t *meter,
				  LCD_t *lcd, UART_HandleTypeDef *huart,
				  uint16_t *adcbuf, volatile bool *adcHazir);
void SystemSM_ButtonEvent(SystemSM_t *sm, uint8_t isLong);

#endif /* INC_SYSTEM_SM_H_ */
