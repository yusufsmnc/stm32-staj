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


/************** State Tanımları **************/

typedef enum{
	SYS_STATE_INIT		= 0,
	SYS_STATE_MEASURE      ,
	SYS_STATE_TRANSMIT     ,
	SYS_STATE_LOAD_CHANGE
}SysState_t;

/************** Yük Tipleri **************/

typedef enum{
	LOAD_RESISTIVE      = 0,		  //  0  derece, PF = 1.00
	LOAD_INDUCTIVE_30	   ,		  //  30 derece, PF = 0.87
	LOAD_INDUCTIVE_60	   ,		  //  60 derece, PF = 0.50
	LOAD_CAPACITIVE		   ,		  // -30 derece, PF = 0.87
	LOAD_COUNT
}LoadType_t;

extern const float LOAD_FAZ_TABLE[LOAD_COUNT];
extern const char* LOAD_NAME_TABLE[LOAD_COUNT];

/************** System State Machine Struct **************/

typedef struct{
	SysState_t current;	  			// mevcut state
	SysState_t previous;      		// önceki state
	uint32_t   stateEnterTick;	    // state'e giriş zamanı
	LoadType_t loadIndex;	  		// aktif yük tipi
	bool	   buttonEvent;	        // buton basış olayı
	volatile bool lcdUpdateFlag;
	volatile bool uartTransmitFlag;
}SystemSM_t;


/************** Fonksiyon Prototipleri **************/

void SystemSM_Init(SystemSM_t *sm);
void SystemSM_Run(SystemSM_t *sm, PowerMeter_t *meter,
				  LCD_t *lcd, UART_HandleTypeDef *huart,
				  uint16_t *adcbuf, volatile bool *adcHazir);
void SystemSM_ButtonEvent(SystemSM_t *sm);

#endif /* INC_SYSTEM_SM_H_ */
