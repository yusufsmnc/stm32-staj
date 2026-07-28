/*
 * lis3dsh.c
 *
 *  Created on: Jul 23, 2026
 *      Author: Yusuf
 */

#include "lis3dsh.h"

#define LIS3DSH_READ_BIT    0x80
#define LIS3DSH_WRITE_MASK  0x7F
#define LIS3DSH_TIMEOUT     1000

static void LIS3DSH_CS_Low(LIS3DSH_t *lis3dsh)
{
    HAL_GPIO_WritePin(lis3dsh->cs_port, lis3dsh->cs_pin, GPIO_PIN_RESET);
}

static void LIS3DSH_CS_High(LIS3DSH_t *lis3dsh)
{
    HAL_GPIO_WritePin(lis3dsh->cs_port, lis3dsh->cs_pin, GPIO_PIN_SET);
}

uint8_t LIS3DSH_Read_Register(LIS3DSH_t *lis3dsh, uint8_t regAddr)
{
    uint8_t txData = regAddr | LIS3DSH_READ_BIT;
    uint8_t rxData = 0;

    LIS3DSH_CS_Low(lis3dsh);
    HAL_SPI_Transmit(lis3dsh->hspi, &txData, 1, LIS3DSH_TIMEOUT);
    HAL_SPI_Receive(lis3dsh->hspi, &rxData, 1, LIS3DSH_TIMEOUT);
    LIS3DSH_CS_High(lis3dsh);

    return rxData;
}

bool LIS3DSH_Write_Register(LIS3DSH_t *lis3dsh, uint8_t regAddr, uint8_t data)
{
    uint8_t txData[2];
    HAL_StatusTypeDef status;

    txData[0] = regAddr & LIS3DSH_WRITE_MASK;
    txData[1] = data;

    LIS3DSH_CS_Low(lis3dsh);
    status = HAL_SPI_Transmit(lis3dsh->hspi, txData, 2, LIS3DSH_TIMEOUT);
    LIS3DSH_CS_High(lis3dsh);

    return (status == HAL_OK);
}

uint8_t LIS3DSH_WHO_Am_I(LIS3DSH_t *lis3dsh)
{
    return LIS3DSH_Read_Register(lis3dsh, LIS3DSH_WHO_AM_I_ADDR);
}

bool LIS3DSH_Initialization(LIS3DSH_t *lis3dsh, SPI_HandleTypeDef *hspi,
                            GPIO_TypeDef *csPort, uint16_t csPin)
{
    if (lis3dsh == NULL || hspi == NULL || csPort == NULL)
        return false;

    lis3dsh->hspi    = hspi;
    lis3dsh->cs_port = csPort;
    lis3dsh->cs_pin  = csPin;

    lis3dsh->x_raw = 0;
    lis3dsh->y_raw = 0;
    lis3dsh->z_raw = 0;

    LIS3DSH_CS_High(lis3dsh);
    HAL_Delay(10);

    if (LIS3DSH_WHO_Am_I(lis3dsh) != LIS3DSH_WHO_AM_I_VALUE)
        return false;

    if (!LIS3DSH_Write_Register(lis3dsh, LIS3DSH_CTRL_REG4, 0x67))
        return false;

    if (!LIS3DSH_Write_Register(lis3dsh, LIS3DSH_CTRL_REG3, 0xC8))
        return false;

    HAL_Delay(10);

    return true;
}

bool LIS3DSH_Read_XYZ(LIS3DSH_t *lis3dsh)
{
    uint8_t txData = LIS3DSH_OUT_X_L | LIS3DSH_READ_BIT;
    uint8_t rxData[6] = {0};
    HAL_StatusTypeDef status;

    LIS3DSH_CS_Low(lis3dsh);
    HAL_SPI_Transmit(lis3dsh->hspi, &txData, 1, LIS3DSH_TIMEOUT);
    status = HAL_SPI_Receive(lis3dsh->hspi, rxData, 6, LIS3DSH_TIMEOUT);
    LIS3DSH_CS_High(lis3dsh);

    if (status != HAL_OK)
        return false;

    lis3dsh->x_raw = (int16_t)((rxData[1] << 8) | rxData[0]);
    lis3dsh->y_raw = (int16_t)((rxData[3] << 8) | rxData[2]);
    lis3dsh->z_raw = (int16_t)((rxData[5] << 8) | rxData[4]);

    lis3dsh->x_cal = lis3dsh->x_raw - lis3dsh->x_offset;
    lis3dsh->y_cal = lis3dsh->y_raw - lis3dsh->y_offset;
    lis3dsh->z_cal = lis3dsh->z_raw - lis3dsh->z_offset;

    return true;
}

void LIS3DSH_Calibrate(LIS3DSH_t *lis3dsh, uint16_t samples)
{
    int32_t x_sum = 0, y_sum = 0, z_sum = 0;

    for (uint16_t i = 0; i < samples; i++)
    {
        LIS3DSH_Read_XYZ(lis3dsh);
        x_sum += lis3dsh->x_raw;
        y_sum += lis3dsh->y_raw;
        z_sum += lis3dsh->z_raw;
        HAL_Delay(5);
    }

    lis3dsh->x_offset = x_sum / samples;
    lis3dsh->y_offset = y_sum / samples;
    lis3dsh->z_offset = (z_sum / samples) - 16384;
}

