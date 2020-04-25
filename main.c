#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "Configuration_header_file.h" //electronigwings'den al?nd?
#include "LCD_16x2_Header_File.h" //electronigwings'den al?nd?

#define  BW_RATE            44    //0x2C
#define  POWER_CTL          45    //0x2D
#define  DATA_FORMAT        49    //0x31
#define  DATAX0             50    //0x32
#define  DATAX1             51    //0x33
#define  DATAY0             52    //0x34
#define  DATAY1             53    //0x35
#define  DATAZ0             54    //0x36
#define  DATAZ1             55    //0x37
#define  FIFO_CTL           56    //0x38            ADXL1345    1010011   0x53   Device
#define CHIP_Write    0xA6        // adxl345 address for writing    10100110  0xA6   Write
#define CHIP_Read     0xA7        // and reading                    10100111  0xA7   Read
#define _XTAL_FREQ  20000000        // Xtal speed
//I2C master mod, saat h?z? (clock) = FOSC/(4 * (SSPxADD+1)), MSSP1

// Hardware I2C functions
void I2CInit(void);
void I2CStart();
void I2CStop();
void I2CRestart();
void I2CAck();
void I2CNak();
void I2CWait();
void I2CSend(unsigned char dat);
unsigned char I2CRead(void);
char E_Write(int addr, unsigned char ch);
unsigned char E_Read(int addr);

void ini_adxl345()
{
E_Write(FIFO_CTL,0x9f);
__delay_ms(10);    
E_Write(DATA_FORMAT,0x09);
__delay_ms(10);  
E_Write(BW_RATE,0x0d);
__delay_ms(10);        
E_Write(POWER_CTL,0x08);  // activate  
}

void main(void)
{
    char bufferx[25];
    char buffery[25];
    char bufferz[25];
    CMCON = 0x07;                        // Comparitors off
	OSCCON = 0x72;		/* Internal Oscillator frequency 8 MHz */
	TRISB &= ~(1<<4);	/* Make pin5 of PORT B as input */										
	LCD_Init();		/* Initialize LCD */
	//LCD_String("IVME OLCER");
    int index = 0;                        // text variable
    unsigned char ch;
    char a,b,c;
    // Initialize I2C Port
    I2CInit();
    // Initialize ADXL345 Accelerometer
    ini_adxl345();
    
    signed long x,y,z;
    signed long xhi,xlo,yhi,ylo,zhi,zlo;
    signed long Xaccumulate, Yaccumulate, Zaccumulate;
    signed long Xaverage, Yaverage, Zaverage;
    int i;
    LCD_String_xy(0,0,"X");
    LCD_String_xy(1,0,"Y");
    LCD_String_xy(1,8,"Z");
    while(1) 
    {
    
    Xaccumulate = Yaccumulate = Zaccumulate = 0;
    for (i=0; i<16; i++) {  // Read sequentially 16 times then get an average.
            I2CStart();
            I2CSend(CHIP_Write);
            I2CSend(DATAX0);    // DATAX0 is the first of 6 bytes
            I2CRestart();
            I2CSend(CHIP_Read);                            //
            xlo = I2CRead();                                // read character
            I2CAck();
            xhi = I2CRead();                                // read character
            I2CAck();
            ylo = I2CRead();                                // read character
            I2CAck();
            yhi = I2CRead();                                // read character
            I2CAck();
            zlo = I2CRead();                                // read character
            I2CAck();
            zhi = I2CRead();                                // read character
            I2CNak();
            I2CStop();

            Xaccumulate += ((xhi<<8) | xlo); //Xaccumulate = Xaccumulate + (xhi*256 + xlo)
            Yaccumulate += ((yhi<<8) | ylo);
            Zaccumulate += ((zhi<<8) | zlo);
                          }  
div_t xavg ; // div_t is a structure explained above
div_t yavg ;
div_t zavg ;
xavg = div(Xaccumulate, 16);
yavg = div(Yaccumulate, 16);
zavg = div(Zaccumulate, 16);

	

sprintf(bufferx,"%5d",xavg.rem);
		LCD_String_xy(0,1,bufferx);/* Print heading on LCD */
sprintf(buffery,"%5d",yavg.quot);
		LCD_String_xy(1,1,buffery);/* Print heading on LCD */
sprintf(bufferz,"%5d",zavg.quot);
		LCD_String_xy(1,9,bufferz);/* Print heading on LCD */
        __delay_ms(25);
    }

}

void I2CInit(void)
{
    TRISBbits.TRISB0 = 1; /* SDA and SCL as input pin */
     TRISBbits.TRISB1 = 1; /* these pins can be configured either i/p or o/p */
    SSPSTAT |= 0x80; /* Slew rate disabled */
    SSPCON1 = 0x28;//Enable SDA and SCL, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
                /* SSPEN = 1, I2C Master mode, clock = FOSC/(4 * (SSPADD + 1)) */
    SSPCON2 = 0x00;      // Reset MSSP Control Register
    SSPADD = 49;      //20000000 / 4= 5000000, 5000000/ 100000= 50 50-1=49
    PIR1bits.SSPIF=0;    // Clear MSSP Interrupt Flag
    PIR2bits.BCLIF=0;
    
}

void I2CStart()
{
    SEN = 1;         /* Start condition enabled */
    while(SEN);      /* automatically cleared by hardware */
                     /* wait for start condition to finish */
}

void I2CStop()
{
    PEN = 1;         /* Stop condition enabled */
    while(PEN);      /* Wait for stop condition to finish */
                     /* PEN automatically cleared by hardware */
}

void I2CRestart()
{
    RSEN = 1;        /* Repeated start enabled */
    while(RSEN);     /* wait for condition to finish */
}

void I2CAck()
{
    ACKDT = 0;       /* Acknowledge data bit, 0 = ACK */
    ACKEN = 1;       /* Ack data enabled */
    while(ACKEN);    /* wait for ack data to send on bus */
}

void I2CNak()
{
    ACKDT = 1;       /* Acknowledge data bit, 1 = NAK */
    ACKEN = 1;       /* Ack data enabled */
    while(ACKEN);    /* wait for ack data to send on bus */
}

void I2CWait()
{
    while ((SSPCON2 & 0x1F ) || ( SSPSTAT & 0x04 ) );
    /* wait for any pending transfer */
}

void I2CSend(unsigned char dat)
{
    SSPBUF = dat;    /* Move data to SSPBUF */
    while(BF);       /* wait till complete data is sent from buffer */
    I2CWait();       /* wait for any pending transfer */
}

unsigned char I2CRead(void)
{
    unsigned char temp;
/* Reception works if transfer is initiated in read mode */
    RCEN = 1;        /* Enable data reception */
    while(!BF);      /* wait for buffer full */
    temp = SSPBUF;   /* Read serial buffer and store in temp register */
    I2CWait();       /* wait to check any pending transfer */
    return temp;     /* Return the read data from bus */
}

// This is random writing. Write to a specified address
char E_Write(int addr, unsigned char ch)
    {
    /* Send Start condition */
    I2CStart();
    /* Send ADXL1345 slave address with write operation */
    I2CSend(CHIP_Write);
    /* Send subaddress, we are writing to this location */
    I2CSend(addr);
    /* send I2C data one by one */
    I2CSend(ch);
    /* Send a stop condition - as transfer finishes */
    I2CStop();
    return 1;                                        // All went well
    }
// The function takes an address and Returns a character
// This is random reading. Read from a specified addess
unsigned char E_Read(int addr)
    {
    unsigned char byte;
    unsigned char ch;
    /* Send Start condition */
    I2CStart();
    /* Send ADXL1345 slave address with write operation */
    I2CSend(CHIP_Write);
    /* this address is actually where we are going to read from */
    I2CSend(addr);
    /* Send a repeated start, after a dummy write to start reading */
    I2CRestart();
    /* send slave address with read bit set */
    I2CSend(CHIP_Read);
 
    ch = I2CRead();                                // read character
    I2CNak();
    I2CStop();
    return ch;
    }
 


