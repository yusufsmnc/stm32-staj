/*
 * flash_storage.h
 *
 *  Created on: Aug 14, 2026
 *      Author: Yusuf
 */

#ifndef INC_FLASH_STORAGE_H_
#define INC_FLASH_STORAGE_H_

#include "main.h"
#include "stdbool.h"
#include "string.h"

/************** Flash Sabit Tanımları **************/

#define FLASH_STORAGE_ADDR		0x08E0000UL  // Sektor 11
#define FLASH_MAGIC_NUMBER		0xDEADBEEFUL // Gecerlilk Kontrolu

/************** Flash Veri Yapısı **************/

typedef struct{
	uint32_t  magic;		// gecerlilik kontrolu
	float energy_Wh;		// birikimli enerji
	uint32_t checksum;		// bozulma kontrolu
}FlashData_t;


/************** Flash Sabit Tanımları **************/

bool Flash_IsValid(void);
bool Flash_LoadEnergy(float *enery_Wh);
bool Flash_SaveEnergy(float energy_Wh);
void Flash_Erase(void);

#endif /* INC_FLASH_STORAGE_H_ */
