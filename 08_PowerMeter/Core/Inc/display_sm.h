/*
 * display_sm.h
 *
 *  Created on: Aug 13, 2026
 *      Author: Yusuf
 */

#ifndef INC_DISPLAY_SM_H_
#define INC_DISPLAY_SM_H_

#include "main.h"
#include "power_meter.h"
#include "lcd_2x16_driver.h"
#include "load_types.h"

/************** Display State Tanımları **************/

typedef enum{
	DISP_STATE_POWER = 0,		// P_act + enerji
	DISP_STATE_VRMS_IRMS,		// Vrms + Irms + yük
	DISP_STATE_PF,				// PF + frekans + Q(reaktif)
	DISP_STATE_COUNT
}DisplayState_t;

/************** Display SM Struct **************/

typedef struct{
	DisplayState_t current;		// mevcut ekran
	DisplayState_t previous;	// onceki ekran
}DisplaySM_t;

/************** Fonksiyon Prototipleri **************/

void DisplaySM_Init(DisplaySM_t *dsm);
void DisplaySM_Next(DisplaySM_t *dsm);
void DisplaySM_Update(DisplaySM_t *dsm, PowerMeter_t *meter, LCD_t *lcd, LoadType_t loadIndex);

#endif /* INC_DISPLAY_SM_H_ */
