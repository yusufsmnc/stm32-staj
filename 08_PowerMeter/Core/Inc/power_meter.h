/*
 * power_meter.h
 *
 *  Created on: Aug 6, 2026
 *      Author: Yusuf
 */

#ifndef INC_POWER_METER_H_
#define INC_POWER_METER_H_

#include "main.h"
#include "stdbool.h"
#include "stdint.h"

/* Sabitler */
#define PM_SAMPLES	100
#define PM_VREF		3.3f
#define PM_ADC_MAX	4095.0f
#define PM_PI		3.14159265f


/* Yük tipleri (faz farklari ile) */
#define PM_FAZ_REZISTIF		 0.0f
#define PM_FAZ_ENDUKTIF_30	30.0f
#define PM_FAZ_ENDUKTIF_60	60.0f
#define PM_FAZ_KAPASITIF   -30.0f

/* Struct */
typedef struct{
	/* Ham ölçümler */
	float Vrms;			// gerilim etkin değeri (V)
	float Irms;			// akım etkin değeri (A)
	float freq;			// frekans (Hz)

	/* Güç hesapları */
	float P_act;		// aktif güç (W)
	float P_react;		// reaktif güç (VAr)
	float P_app;		// görünür güç (VA)
	float PF;			// güç faktörü (0-1)

	/* Enerji sayacı */
	float energy_Wh;	// birikimli enerji (Wh)
	uint32_t lastTick;	// son hesaplama zamani (ms)

	/* Simülasyon */
	float faz_aci;		// yük faz açısı (derece)
} PowerMeter_t;

/* Fonksiyonlar */

bool PowerMeter_Init(PowerMeter_t *pm, float faz_aci);
void PowerMeter_Calculate(PowerMeter_t *pm, uint16_t *adcBuf);
//void PowerMeter_Display(PowerMeter_t *pm, UART_HandleTypeDef *huart);
void PowerMeter_SetFaz(PowerMeter_t *pm, float faz_aci);

#endif /* INC_POWER_METER_H_ */
