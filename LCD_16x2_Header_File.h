
/*
 * LCD 16x2 Header File  
 * http://www.electronicwings.com  
 */

#ifndef LCD_16X2_HEADER_FILE_H
#define	LCD_16X2_HEADER_FILE_H

#include <xc.h>                     /* Include processor files - each processor file is guarded. */  
#include <pic18f4550.h>             /* Include PIC18F4550 header file */

#define RS LATA0                    /* PORT 0 pin is used for Register Select */
#define EN LATA1                    /* PORT 1 pin is used for Enable */
#define LCD_Data LATD               /* PORT is used for transmitting data to LCD */
#define LCD_Data_Dir TRISD          /* LCD data PORT direction */
#define LCD_Command_Dir TRISA       /* LCD command PORT direction */

void LCD_Init();                    /* LCD initialization function */
void LCD_Command(char );            /* LCD command function */
void LCD_Char(char x);              /* LCD character data function */
void LCD_String(const char*);       /* LCD string function */
void MSdelay(unsigned int );        /* millisecond delay function */
void LCD_String_xy(char, char, const char*);/* LCD string function with Row and position option */

#endif	/* LCD_16X2_HEADER_FILE_H */

