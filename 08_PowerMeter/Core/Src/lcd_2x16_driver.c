/*
 * lcd_2x16_driver.c
 *
 *  Created on: Aug 10, 2026
 *      Author: Yusuf
 */

#include "lcd_2x16_driver.h"

static void LCD_Pulse(void)
{
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);
}

static void LCD_Write4bit(uint8_t nibble)
{
    /* Üst 4 biti D4-D7 pinlerine yaz */
    HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, (nibble & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, (nibble & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, (nibble & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, (nibble & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    LCD_Pulse();
}

void LCD_Initialization(LCD_t *lcd){
    HAL_Delay(50);

    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);

    LCD_Write4bit(0x30);
    HAL_Delay(5);
    LCD_Write4bit(0x30);
    HAL_Delay(1);
    LCD_Write4bit(0x30);
    HAL_Delay(1);

    /* 4-bit moda geç */
    LCD_Write4bit(0x20);
    HAL_Delay(1);

    LCD_Send_Command(lcd, LCD_Cmd_FunctionSet | LCD_4BIT_MODE | LCD_2_LINE | LCD_5x8_DOTS);
    HAL_Delay(1);

    LCD_Send_Command(lcd, LCD_Cmd_DisplayOnOff);
    HAL_Delay(1);

    LCD_Send_Command(lcd, LCD_Cmd_ClearDisplay);
    HAL_Delay(2);

    LCD_Send_Command(lcd, LCD_Cmd_EntryMode | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DECREMENT);
    HAL_Delay(1);

    lcd->display_control = LCD_Display_On;
    LCD_Send_Command(lcd, LCD_Cmd_DisplayOnOff | lcd->display_control);
    HAL_Delay(1);
}


void LCD_Clear(LCD_t *lcd)
{
    LCD_Send_Command(lcd, LCD_Cmd_ClearDisplay);
    HAL_Delay(2);
}


void LCD_Send_Command(LCD_t *lcd, uint8_t cmd)
{
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);  // RS=0 komut

    LCD_Write4bit(cmd & 0xF0);

    LCD_Write4bit((cmd << 4) & 0xF0);

    HAL_Delay(1);
}


void LCD_Set_Cursor(LCD_t *lcd, uint8_t row, uint8_t col){
	const uint8_t row_offset[] = {0x00, 0x40};

	if(row >= lcd->rows)
		row = 0;
	if(col >= 16)
		col = 15;

	LCD_Send_Command(lcd, LCD_Cmd_Set_DDRAM_Address | (col + row_offset[row]));
}

void LCD_Send_Data(LCD_t *lcd, uint8_t data){
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);    // RS=1 data

    LCD_Write4bit(data & 0xF0);

    LCD_Write4bit((data << 4) & 0xF0);
    HAL_Delay(1);
}

void LCD_Send_Char(LCD_t *lcd, char ch){
    LCD_Send_Data(lcd, (uint8_t)ch);
}

void LCD_Send_String(LCD_t *lcd, const char *str){
    while(*str)
        LCD_Send_Char(lcd, *str++);
}

void LCD_Home(LCD_t *lcd){
    LCD_Send_Command(lcd, LCD_Cmd_ReturnHome);
    HAL_Delay(2);
}


void LCD_Cursor_Show(LCD_t *lcd){
    lcd->display_control |= LCD_Cursor_On;
    LCD_Send_Command(lcd, LCD_Cmd_DisplayOnOff | lcd->display_control);
}

void LCD_Cursor_Hide(LCD_t *lcd){
    lcd->display_control &= ~LCD_Cursor_On;
    LCD_Send_Command(lcd, LCD_Cmd_DisplayOnOff | lcd->display_control);
}

void LCD_Print_Padded(LCD_t *lcd, const char *format, ...)
{
    char buf[17] = "                ";  // 16 boşluk + null
    char tmp[33];

    va_list args;
    va_start(args, format);
    vsnprintf(tmp, sizeof(tmp), format, args);
    va_end(args);

    uint8_t len = strlen(tmp);
    if(len > 16) len = 16;
    memcpy(buf, tmp, len);

    LCD_Send_String(lcd, buf);
}

void LCD_Printf(LCD_t *lcd, const char *format, ...){
    char buf[33];  // 16x2 = 32 karakter max + null
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    LCD_Send_String(lcd, buf);
}

void LCD_Scroll_Text(LCD_t *lcd, const char *text, uint8_t row, uint8_t delayMs){
    uint8_t len = strlen(text);
    char buf[17];  // 16 karakter + null

    // Metin 16 karakterden kısaysa direkt yaz
    if(len <= 16){
        LCD_Set_Cursor(lcd, row, 0);
        LCD_Send_String(lcd, text);
        return;
    }

    // Metin uzunsa sola kaydır
    for(uint8_t i = 0; i <= len - 16; i++){
        LCD_Set_Cursor(lcd, row, 0);
        strncpy(buf, text + i, 16);
        buf[16] = '\0';
        LCD_Send_String(lcd, buf);
        HAL_Delay(delayMs);
    }
}
