/*
 * LCD16x2 8-bit Source File 
 * http://www.electronicwings.com
 */

#include "LCD_16x2_Header_File.h"

/****************************Functions********************************/
void LCD_Init()
{
   	LCD_Data_Dir = 0;       /* Set LCD data PORT direction as output PORT */
    LCD_Command_Dir = 0;    /* Set LCD command PORT direction as output PORT */
    MSdelay(20);            /* LCD power on initialization delay must >15mSec */
	LCD_Command(0x38);      /* Initialize with 2 line and 5*7 matrix of LCD */
    LCD_Command(0x01);      /* Clear display screen */
    MSdelay(2);
    LCD_Command(0x0c);      /* Display on cursor off */
    LCD_Command(0x06);      /* Increment cursor (shift cursor to right) */
}

void LCD_Clear()
{
    LCD_Command(0x01);      /* Clear display screen */
    MSdelay(2);
}

void LCD_Command(char Command )
{
	LCD_Data = Command;     /* Send data to PORT as a command for LCD */   
	RS = 0;                 /* Command Register is selected */
	EN = 1;                 /* High-to-Low pulse on Enable pin to latch data */ 
	NOP();
	EN = 0;
    MSdelay(1);
}

void LCD_Char(char Data)
{
	LCD_Data = Data;        /* Send data to LCD */  
	RS = 1;                 /* Data Register is selected */
	EN=1;                   /* High-to-Low pulse on Enable pin to latch data */   
	NOP();
	EN=0;
    MSdelay(1);
}

void LCD_String(const char *str)
{
	while((*str)!=0)
	{		
	  LCD_Char(*str);
	  str++;	
    }
}

void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row<1)
    {
        location=(0x80) | ((pos) & 0x0f);      /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f);      /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
   
    LCD_String(msg);
}
/*********************************Delay Function********************************/
void MSdelay(unsigned int val)
{
     unsigned int i,j;
        for(i=0;i<=val;i++)
            for(j=0;j<165;j++);      /*This count Provide delay of 1 ms for 8MHz Frequency */
}


