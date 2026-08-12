/*
 * system_sm.c
 *
 *  Created on: Aug 12, 2026
 *      Author: Yusuf
 */

#include "system_sm.h"
#include "stdio.h"
#include "string.h"

/************** Faz Açısı Tablosu **************/

const float LOAD_FAZ_TABLE[LOAD_COUNT] = {
		 0.0f ,  	// RESISTIVE
		30.0f ,		// INDUCTIVE_30
		60.0f ,		// INDUCTIVE_60
	   -30.0f 		// CAPACITIVE
};

const char* LOAD_NAME_TABLE[LOAD_COUNT] = {
		"Rezistif",
		"Enduktif",
		"Motor   ",
		"Kapasitif"
};


/************** Dışarıdan Erişilecek Fonksiyonlar **************/

extern void Generate_Waves(float faz_aci);

/************** State Geçiş Yardımcısı **************/

static void SystemSM_Transition(SystemSM_t *sm, SysState_t newState){
	sm->previous	   = sm->current;
	sm->current		   = newState;
	sm->stateEnterTick = HAL_GetTick();
}

/************** Init **************/

void SystemSM_Init(SystemSM_t *sm){
	sm->current			 = SYS_STATE_INIT;
	sm->previous		 = SYS_STATE_INIT;
	sm->stateEnterTick   = HAL_GetTick();
	sm->loadIndex		 = LOAD_INDUCTIVE_30;  // başlangıç: Enduktif
	sm->buttonEvent		 = false;
	sm->lcdUpdateFlag	 = false;
	sm->uartTransmitFlag = false;
}

/************** Buton Olayı **************/

void SystemSM_ButtonEvent(SystemSM_t *sm){
	sm->buttonEvent = true;
}

/************** Ana döngü **************/

void SystemSM_Run(SystemSM_t *sm, PowerMeter_t *meter,
				  LCD_t *lcd, UART_HandleTypeDef *huart,
				  uint16_t *adcBuf, volatile bool *adcHazir){
	switch(sm->current){
	// Init
	case SYS_STATE_INIT:
		LCD_Set_Cursor(lcd, 0, 0);
		LCD_Print_Padded(lcd, " PowerMeter v1");
		LCD_Set_Cursor(lcd, 1, 0);
		LCD_Print_Padded(lcd, "Baslatiliyor...");
		HAL_Delay(1500);

		// baslangic yuk tipi
		PowerMeter_SetFaz(meter, LOAD_FAZ_TABLE[sm->loadIndex]);
		Generate_Waves(LOAD_FAZ_TABLE[sm->loadIndex]);

		SystemSM_Transition(sm, SYS_STATE_MEASURE);
		break;

	// Measure
	case SYS_STATE_MEASURE:
		// ADC buffer dolunca hesapla
		if(*adcHazir) {
			*adcHazir = false;
			PowerMeter_Calculate(meter, adcBuf);
		}

		// LCD guncelle - her 500ms
		if(sm->lcdUpdateFlag){
			sm->lcdUpdateFlag = false;

			LCD_Set_Cursor(lcd, 0, 0);
		    LCD_Print_Padded(lcd, "P:%d.%04d W",(int)meter->P_act,
		            (int)((meter->P_act - (int)meter->P_act) * 10000.0f));
			LCD_Set_Cursor(lcd, 1, 0);
			LCD_Print_Padded(lcd, "E:%d.%04d Wh", (int)meter->energy_Wh,
					(int)((meter->energy_Wh - (int)meter->energy_Wh) * 10000.0f));
		}

		// UART gonder - her 1 sn
		if(sm->uartTransmitFlag){
	        sm->uartTransmitFlag = false;
	        SystemSM_Transition(sm, SYS_STATE_TRANSMIT);
		}

		// Buton basildi - yuk degistir
		if(sm->buttonEvent){
			sm->buttonEvent = false;
			SystemSM_Transition(sm, SYS_STATE_LOAD_CHANGE);
		}
		break;

	// Transmit
	case SYS_STATE_TRANSMIT:
		PowerMeter_Display(meter, huart);
		SystemSM_Transition(sm, SYS_STATE_MEASURE);
		break;
	// Load_Change
	case SYS_STATE_LOAD_CHANGE:
		// Yuk tipini ilerlet
		sm->loadIndex = (sm->loadIndex + 1) % LOAD_COUNT;

		// Yeni faz acisi
		PowerMeter_SetFaz(meter, LOAD_FAZ_TABLE[sm->loadIndex]);
		Generate_Waves(LOAD_FAZ_TABLE[sm->loadIndex]);

		// LCD'de yuk degisimi göster
	    LCD_Set_Cursor(lcd, 0, 0);
	    LCD_Print_Padded(lcd, "Yuk Degisti!");

	    LCD_Set_Cursor(lcd, 1, 0);
	    LCD_Print_Padded(lcd, "%s %d deg",
	        LOAD_NAME_TABLE[sm->loadIndex],
	        (int)LOAD_FAZ_TABLE[sm->loadIndex]);
		HAL_Delay(1000);

		SystemSM_Transition(sm, SYS_STATE_MEASURE);
		break;
	default:
		SystemSM_Transition(sm, SYS_STATE_INIT);
		break;
	}
}

