/*
 * lcd_2x16_driver.h
 *
 *  Created on: Aug 10, 2026
 *      Author: Yusuf
 */

#ifndef INC_LCD_2X16_DRIVER_H_
#define INC_LCD_2X16_DRIVER_H_

#include "main.h"
#include "stdbool.h"
#include "stdarg.h"
#include "string.h"

/* ************* PINs ************* */
#define LCD_RS_PORT     GPIOB
#define LCD_RS_PIN      GPIO_PIN_0

#define LCD_EN_PORT     GPIOB
#define LCD_EN_PIN      GPIO_PIN_1

#define LCD_D4_PORT     GPIOB
#define LCD_D4_PIN      GPIO_PIN_4

#define LCD_D5_PORT     GPIOB
#define LCD_D5_PIN      GPIO_PIN_5

#define LCD_D6_PORT     GPIOB
#define LCD_D6_PIN      GPIO_PIN_6

#define LCD_D7_PORT     GPIOB
#define LCD_D7_PIN      GPIO_PIN_7

// Function Set Flags
#define LCD_8BIT_MODE 				0x10
#define LCD_4BIT_MODE				0x00
#define LCD_2_LINE					0x08
#define LCD_1_LINE					0x00
#define LCD_5x10_DOTS				0x04
#define LCD_5x8_DOTS				0x00

// Entry Mode Set Flags
#define LCD_ENTRY_RIGHT				0x00
#define LCD_ENTRY_LEFT				0x02
#define LCD_ENTRY_SHIFT_INCREMENT	0x01
#define LCD_ENTRY_SHIFT_DECREMENT	0x00

/* ************* LCD Commands ************* */
typedef enum{
	LCD_Cmd_ClearDisplay 			= 0x01,
	LCD_Cmd_ReturnHome				= 0x02,
	LCD_Cmd_EntryMode				= 0x04,
	LCD_Cmd_DisplayOnOff			= 0x08,
	LCD_Cmd_CursorShift				= 0x10,
	LCD_Cmd_FunctionSet				= 0x20,
	LCD_Cmd_Set_CGRAM_Address		= 0x40,
	LCD_Cmd_Set_DDRAM_Address		= 0x80
}LCD_Commands;

/* ************* Display Setups ************* */
typedef enum {
	LCD_Display_On					= 0x04,
	LCD_Cursor_On					= 0x02,
	LCD_Cursor_Blink_On				= 0x01
}LCD_Display_Control_Flags;

/* ************* LCD Structure ************* */
typedef struct {
    uint8_t  columns;
    uint8_t  rows;
    uint8_t  display_control;
} LCD_t;

/* ************* LCD APIs Functions ************* */
void LCD_Initialization(LCD_t *lcd);
void LCD_Clear(LCD_t *lcd);
void LCD_Home(LCD_t *lcd);
void LCD_Set_Cursor(LCD_t *lcd, uint8_t row, uint8_t col);
void LCD_Send_Char(LCD_t *lcd, char ch);
void LCD_Send_String(LCD_t *lcd, const char *str);
void LCD_Send_Command(LCD_t *lcd, uint8_t cmd);
void LCD_Send_Data(LCD_t *lcd, uint8_t data);
void LCD_Cursor_Show(LCD_t *lcd);
void LCD_Cursor_Hide(LCD_t *lcd);
void LCD_Print_Padded(LCD_t *lcd, const char *format, ...);
void LCD_Printf(LCD_t *lcd, const char *format, ...);
void LCD_Scroll_Text(LCD_t *lcd, const char *text, uint8_t row, uint8_t delayMs);

#endif /* INC_LCD_2X16_DRIVER_H_ */
