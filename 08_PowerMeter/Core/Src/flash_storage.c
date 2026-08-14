/*
 * flash_storage.c
 *
 *  Created on: Aug 14, 2026
 *      Author: Yusuf
 */

#include "flash_storage.h"

/************** Checksum Hesapla **************/

static uint32_t Flash_CalcChecksum(float energy_Wh){
	uint32_t val;
	memcpy(&val, &energy_Wh, sizeof(float));
	return FLASH_MAGIC_NUMBER ^ val;

}

/************** Flash Gecerlimi  **************/

bool Flash_IsValid(void){
	FlashData_t *data = (FlashData_t*)FLASH_STORAGE_ADDR;
	if(data->magic != FLASH_MAGIC_NUMBER)
		return false;
	return(data->checksum == Flash_CalcChecksum(data->energy_Wh));
}

/************** Flash'tan Enerjiyi Oku **************/

bool Flash_LoadEnergy(float *energy_Wh){
	if(!Flash_IsValid())
		return false;
	FlashData_t *data = (FlashData_t*)FLASH_STORAGE_ADDR;
	*energy_Wh = data->energy_Wh;
	return true;
}

/************** Flash Sektorunu Erase'le **************/

void Flash_Erase(void){
	FLASH_EraseInitTypeDef eraseInit;
	uint32_t sectorError;

	eraseInit.TypeErase		= FLASH_TYPEERASE_SECTORS;
	eraseInit.Sector		= FLASH_SECTOR_11;
	eraseInit.NbSectors 	= 1;
	eraseInit.VoltageRange  = FLASH_VOLTAGE_RANGE_3;

	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&eraseInit, &sectorError);
	HAL_FLASH_Lock();
}


/************** Flash^a enerjiyi yaz **************/

bool Flash_SaveEnergy(float eneghy_Wh){
	FlashData_t data;
	data.magic		= FLASH_MAGIC_NUMBER;
	data.energy_Wh 	= eneghy_Wh;
	data.checksum   = Flash_CalcChecksum(eneghy_Wh);

	Flash_Erase();

	HAL_FLASH_Unlock();

	uint32_t addr	= FLASH_STORAGE_ADDR;
	uint32_t *ptr	= (uint32_t*)&data;
	bool 	 result = true;

	for(uint8_t i = 0; i < sizeof(FlashData_t) / 4; i++){
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, ptr[i]) != HAL_OK){
			result = false;
			break;
		}
		addr+=4;
	}
	HAL_FLASH_Lock();
	return result;
}
