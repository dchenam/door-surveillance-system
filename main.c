#include "stm32f10x.h"
#include "lcd.h"
#include "i2c_GY80.h"
#include "math.h"
#include <stdio.h>

#define HMC5883L_Addr	0x3C
#define L3G4200_Addr	0xD2
#define BMP085_Addr	  0xEE
#define ADXL345_Addr	0xA6


/* Private function prototypes -----------------------------------------------*/
void HMC_Init(void);
void Delayus(int duration);


int main(void)
{
	char str[20];
	uint16_t x;
	LCD_INIT(); 						// LCD_INIT
	I2C_GY80_Init();        // I2C Init
  HMC_Init();             // HMC Init
	
	x = 3;
	sprintf(str, "%d", x);
	
  while (1) {
		LCD_DrawString(50, 50, str);
  }
}



void HMC_Init(void)
{
  I2C_ByteWrite(HMC5883L_Addr, 0x00, 0x70);
  I2C_ByteWrite(HMC5883L_Addr, 0x01, 0xA0);
  Delayus(10000);
}

void Delayus(int duration)
{
		while(duration--) 
		{
			int i=0x02;				
			while(i--)
			__asm("nop");
		}
}
