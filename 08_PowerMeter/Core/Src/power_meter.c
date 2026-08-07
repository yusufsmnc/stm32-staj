/*
 * power_meter.c
 *
 *  Created on: Aug 6, 2026
 *      Author: Yusuf
 */

#include "power_meter.h"
#include "math.h"
#include "string.h"
#include "stdio.h"

bool PowerMeter_Init(PowerMeter_t *pm, float faz_aci){
	if(pm == NULL)
		return false;
	// Tüm alanları sıfırla
	memset(pm, 0 ,sizeof(PowerMeter_t));

	// Faz açısını ayarla
	pm->faz_aci = faz_aci;

	// Enerji sayacı için başlangıç zamanı
	pm->lastTick = HAL_GetTick();

	return true;
}

void PowerMeter_Calculate(PowerMeter_t *pm, uint16_t *adcBuf){

	/* DC offset hesapla */
	float mean_V = 0.0f;
	float mean_I = 0.0f;
	for(int i = 0; i < PM_SAMPLES; i++){
		mean_V += (float)adcBuf[i * 2];
		mean_I += (float)adcBuf[i * 2 + 1];
	}
	mean_V /= (float)PM_SAMPLES;
	mean_I /= (float)PM_SAMPLES;

	/* AC bileşenleri çıkar */
	static float V[PM_SAMPLES];
	static float I[PM_SAMPLES];
	for(int i = 0; i < PM_SAMPLES; i++){
		V[i] = ((float)adcBuf[i * 2] - mean_V) / PM_ADC_MAX * PM_VREF;
		I[i] = ((float)adcBuf[i * 2 + 1] - mean_I) / PM_ADC_MAX * PM_VREF;
	}

	/* Vrms ve Irms */
	float sumV = 0.0f;
	float sumI = 0.0f;
	for(int i = 0; i < PM_SAMPLES; i++){
		sumV += V[i] * V[i];
		sumI += I[i] * I[i];
	}
	pm->Vrms = sqrtf(sumV / (float)PM_SAMPLES);
	pm->Irms = sqrtf(sumI / (float)PM_SAMPLES);

	/* Aktif Güç */
	float sumP = 0.0f;
	for(int i = 0; i < PM_SAMPLES; i++){
		sumP += V[i] * I[i];
	}
	pm->P_act = sumP / (float)PM_SAMPLES;

	/* Reaktif Güç */
	// Gerilimi 90 derece kaydır
	float sumQ = 0.0f;
	for(int i = 0; i < PM_SAMPLES; i++){
		float v90 = V[(i + PM_SAMPLES / 4) % PM_SAMPLES];
		sumQ += v90 * I[i];
	}
	pm->P_react = sumQ / (float)PM_SAMPLES;

	/* Görünür Güç ve PF */
    pm->P_app = sqrtf(pm->P_act   * pm->P_act +
                      pm->P_react * pm->P_react);
    pm->PF    = (pm->P_app > 0.0f) ? (pm->P_act / pm->P_app) : 0.0f;

    /* Frekans */
    uint8_t gecis = 0;
    for(int i = 1; i < PM_SAMPLES; i++){
    	if(V[i - 1] < 0.0f && V[i] >= 0.0f)
    		gecis++;
    }
    // Örnekleme frekansı = 5000Hz, bir periyotta 1 geçiş
    pm->freq = (gecis > 0) ? ((float)gecis * 5000.0f / (float)PM_SAMPLES) : 0.0f;

    /* Enerji Birikimi */
    uint32_t now  = HAL_GetTick();
    float    dt_h = (float)(now - pm->lastTick) / 3600000.0f;
    pm->energy_Wh += pm->P_act * dt_h;
    pm->lastTick   = now;
}

void PowerMeter_SetFaz(PowerMeter_t *pm, float faz_aci){
    pm->faz_aci = faz_aci;
}


/*void PowerMeter_Display(PowerMeter_t *pm, UART_HandleTypeDef *huart){
    char buf[200];

    int len = snprintf(buf, sizeof(buf),
        "%.4f,%.4f,%.2f,%.4f,%.4f,%.4f,%.4f,%.4f\r\n",
        pm->Vrms,
        pm->Irms,
        pm->freq,
        pm->P_act,
        pm->P_react,
        pm->P_app,
        pm->PF,
        pm->energy_Wh);

    HAL_UART_Transmit(huart, (uint8_t*)buf, len, 100);
}
*/
