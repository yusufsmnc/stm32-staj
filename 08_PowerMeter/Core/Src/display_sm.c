/*
 * display_sm.c
 *
 *  Created on: Aug 13, 2026
 *      Author: Yusuf
 */

#include "display_sm.h"


/************** Init **************/

void DisplaySM_Init(DisplaySM_t *dsm){
	dsm->current  = DISP_STATE_POWER;
	dsm->previous = DISP_STATE_POWER;
}

/************** Sonraki Ekrana gecis **************/

void DisplaySM_Next(DisplaySM_t *dsm){
	dsm->previous = dsm->current;
	dsm->current  = (dsm->current + 1) % DISP_STATE_COUNT;
}

/************** LCD guncelle **************/

void DisplaySM_Update(DisplaySM_t *dsm, PowerMeter_t *meter, LCD_t *lcd, LoadType_t loadIndex){
	switch (dsm->current){
		case DISP_STATE_POWER:
			LCD_Set_Cursor(lcd, 0, 0);
			LCD_Print_Padded(lcd, "P:%d.%04d W", (int)meter->P_act,
					(int)((meter->P_act - (int)meter->P_act) * 10000.0f));
			LCD_Set_Cursor(lcd, 1, 0);
			LCD_Print_Padded(lcd, "E:%d.%04d Wh", (int)meter->energy_Wh,
					(int)((meter->energy_Wh - (int)meter->energy_Wh) * 10000.0f));
			break;
		case DISP_STATE_VRMS_IRMS:
			LCD_Set_Cursor(lcd, 0, 0);
			LCD_Print_Padded(lcd, "V:%d.%03d I:%d.%03d",(int)meter->Vrms,
					(int)((meter->Vrms - (int)meter->Vrms) * 1000.0f), (int)meter->Irms, (int)((meter->Irms - (int)meter->Irms) * 1000.0f));
			LCD_Set_Cursor(lcd, 1, 0);
			LCD_Print_Padded(lcd, "Yuk:%s", LOAD_NAME_TABLE[loadIndex]);
			break;
		case DISP_STATE_PF:
			float q_abs = (meter->P_react < 0) ? -meter->P_react : meter->P_react;
			char  q_sign = (meter->P_react < 0) ? '-' : '+';
			LCD_Set_Cursor(lcd, 0, 0);
			LCD_Print_Padded(lcd, "PF:%d.%02d f:%dHz", (int)meter->PF,
					(int)((meter->PF - (int)meter->PF) * 100.0f), (int)meter->freq);
			LCD_Set_Cursor(lcd, 1, 0);
			LCD_Print_Padded(lcd, "Q:%c%d.%03d VAr", q_sign, (int)q_abs, (int)((q_abs - (int)q_abs) * 1000.0f));
			break;
		default:
			DisplaySM_Init(dsm);
			break;
	}
}
