/*
 * flash_storage.c
 *
 *  Created on: Aug 14, 2026
 *      Author: Yusuf
 */

#include "flash_storage.h"
#include "string.h"

/************** Checksum Hesapla **************/

/*
 * Flash_CalcChecksum
 *
 * Enerji degerinin dogruluğunu kontrol etmek icin checksum hesaplar.
 *
 * Yontem: float degerini uint32_t'ye donustur (memcpy ile),
 * ardından magic number ile XOR isle.
 *
 * Ornek:
 *   energy_Wh = 0.0234f → binary: 0x3CBF70A4
 *   checksum  = 0xDEADBEEF XOR 0x3CBF70A4 = 0xE212CE4B
 *
 * Enerji degeri degisirse XOR sonucu da degisir → bozulma tespit edilir.
 */

static uint32_t Flash_CalcChecksum(float energy_Wh){
	uint32_t val;
	memcpy(&val, &energy_Wh, sizeof(float));
	return FLASH_MAGIC_NUMBER ^ val;

}

/************** Flash Gecerlimi  **************/

/*
 * Flash_IsValid
 *
 * Flash'ta gecerli veri olup olmadigini iki adimda kontrol eder:
 *
 * Adim 1 — Magic Number kontrolu:
 *   Flash ilk acilista 0xFFFFFFFF degerindedir.
 *   Biz yazmissak magic = 0xDEADBEEF olmali.
 *   Eslesmiyor → "hic yazilmamis veya baska program yazmis" → false don.
 *
 * Adim 2 — Checksum kontrolu:
 *   Yazma sirasinda hesaplanan checksum Flash'ta sakli.
 *   Simdi ayni formul ile yeniden hesapla.
 *   Eslesmiyor → "veri bozulmus, elektrik kesilmis olabilir" → false don.
 *
 * Her iki kontrol gecerse → Flash'taki veri guvenilir → true don.
 */

bool Flash_IsValid(void){
    FlashData_t *data = (FlashData_t *)FLASH_STORAGE_ADDR;

    if (data->magic != FLASH_MAGIC_NUMBER)
        return false;

    return (data->checksum == Flash_CalcChecksum(data->energy_Wh));
}

/************** Flash'tan Enerjiyi Oku **************/

/*
 * Flash_LoadEnergy
 *
 * Reset veya guc kesintisi sonrasi Flash'tan enerji degerini yukler.
 *
 * Once Flash_IsValid ile veri gecerliligi kontrol edilir.
 * Gecerli ise Flash adresini dogrudan pointer ile okur —
 * kopyalama gerekmez, Flash bellek haritasinda gorunur.
 *
 * Basarili → *energy_Wh guncellenir, true don.
 * Basarisiz → *energy_Wh degismez, false don → cagiran 0'dan baslar.
 */

bool Flash_LoadEnergy(float *energy_Wh){
    if (!Flash_IsValid())
        return false;

    FlashData_t *data = (FlashData_t *)FLASH_STORAGE_ADDR;
    *energy_Wh = data->energy_Wh;
    return true;
}

/************** Flash Sektorunu Erase'le **************/

/*
 * Flash_Erase
 *
 * Yeni veri yazmadan once Sektor 11'i siler.
 *
 * Flash'a yazmadan once mutlaka silinmeli —
 * Flash bitleri yalnizca 1'den 0'a degisebilir,
 * 0'dan 1'e degistirmek icin once silme gerekir.
 *
 * Silme islemi ~1 saniye surer — bu sure zarfinda
 * kesme servisleri de yavaslar.
 *
 * VoltageRange_3 → 2.7V-3.6V arasi calisma gerilimi icin (Discovery karti).
 */

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

/************** Flash'a enerjiyi yaz **************/

/*
 * Flash_SaveEnergy
 *
 * Birikmis enerji degerini Flash Sektor 11'e kalici olarak kaydeder.
 * Reset veya guc kesintisi sonrasi deger korunur.
 *
 * Islem adimlari:
 *   1. FlashData_t yapisi doldurulur:
 *      - magic     = 0xDEADBEEF  (gecerlilik isaretcisi)
 *      - energy_Wh = kaydedilecek deger
 *      - checksum  = magic XOR energy_Wh  (bozulma kontrolu)
 *
 *   2. Sektor 11 silinir (Flash_Erase).
 *
 *   3. FlashData_t yapisi 4'er byte'lik (Word) parcalar halinde yazilir.
 *      HAL_FLASH_Program her cagrisinda 4 byte yazar.
 *      sizeof(FlashData_t) = 12 byte → 3 yazma islemi.
 *
 *   4. Basarili → true, herhangi bir adimda hata → false.
 *
 * Uyari: Her cagri bir Flash silme islemi gerceklestirir.
 *        STM32F407 Flash omru 10.000 silme/yazma —
 *        bu nedenle her 10 saniyede bir cagirilmaktadir.
 */

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
