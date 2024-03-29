#include "stm32f10x.h"
#include "lcd.h"
#include <stdio.h>
#include "ov7725/ov7725.h"
#include "button/button.h"
#include "led/led.h"
#include "bmp/bmp.h"
#include "FATFS/ff.h"

#define ITM_Port8(n) (*((volatile unsigned char *)(0xE0000000 + 4 * n)))
#define ITM_Port16(n) (*((volatile unsigned short *)(0xE0000000 + 4 * n)))
#define ITM_Port32(n) (*((volatile unsigned long *)(0xE0000000 + 4 * n)))

#define DEMCR (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA 0x01000000

void Delayus(int duration);
extern uint8_t Ov7725_vsync;
extern OV7725_MODE_PARAM cam_mode;
uint8_t retry = 0;
float frame_count = 0;
FATFS fs; /* FatFs system */
FRESULT res_sd;

int main(void)
{
	LCD_INIT();
	LED_GPIO_Config();
	button_init();
	res_sd = f_mount(&fs, "0:", 1);
	OV7725_GPIO_Config();
	//ESP8266_init();

	while (OV7725_Init() != SUCCESS)
	{
		retry++;
		if (retry > 5)
		{
			while (1)
				;
		}
	}

	OV7725_Light_Mode(cam_mode.light_mode);
	OV7725_Color_Saturation(cam_mode.saturation);
	OV7725_Brightness(cam_mode.brightness);
	OV7725_Contrast(cam_mode.contrast);
	OV7725_Special_Effect(cam_mode.effect);
	OV7725_Window_Set(cam_mode.cam_sx, cam_mode.cam_sy, cam_mode.cam_width, cam_mode.cam_height, cam_mode.QVGA_VGA);
	ILI9341_GramScan(cam_mode.lcd_scan);

	//LCD_DrawString(100, 0, "OV7725 initialize success!");

	Ov7725_vsync = 0;

	while (1)
	{
		if (read_button(BUTTON1) == 1)
		{
			if (Ov7725_vsync == 2)
			{
				frame_count++;
				FIFO_PREPARE;
				ImagDisp(cam_mode.lcd_sx,
						 cam_mode.lcd_sy,
						 cam_mode.cam_width,
						 cam_mode.cam_height);
				Ov7725_vsync = 0;
			}
			LED_PURPLE
		}
		if (read_button(BUTTON2) == 1)
		{

			static uint8_t name_count = 0;
			char name[40];
			name_count++;
			//file name in SD card
			sprintf(name, "0:DoorSurveillance_%d.bmp", name_count);
			LCD_DrawString(0, 0, name);
			//LED_BLUE;
			ILI9341_GramScan(cam_mode.lcd_scan);
			if (Screen_Shot(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH, name) == 0)
			{
				LED_GREEN;
			}
		}
	}
}

void Delayus(int duration)
{
	while (duration--)
	{
		int i = 0x02;
		while (i--)
			__asm("nop");
	}
}

/* ------------------------------------------end of file---------------------------------------- */
