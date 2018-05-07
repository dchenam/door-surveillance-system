#include "stm32f10x.h"
#include "lcd.h"
#include <stdio.h>
#include "ov7725/ov7725.h"
#include "button/button.h"
#include "led/led.h"
#include "bmp/bmp.h"
#include "FATFS/ff.h"
#include "gui/gui.h"
#include "pir/pir.h"




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
uint8_t mode = 0;
static uint8_t name_count = 0;
char name[40];
static uint8_t out_count = 0;

void MCO_CONFIG(void);
GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;



int main(void)
{

	LCD_INIT();
	LED_GPIO_Config();
	button_init();
	res_sd = f_mount(&fs, "0:", 1);
	OV7725_GPIO_Config();
	//ESP8266_init();
	InitHCSR04();

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

	
		
	gui_init();

	Ov7725_vsync = 0;


	while (1)
	{
		LED_RGBOFF;
			
			if(read_button(BUTTON1) == 1){
				LED_WHITE;
			}

			if(read_button(BUTTON2) == 1){
				LED_GREEN;
			}

			if(read_button(BUTTON3) == 1){
				LED_BLUE;
			}

			if(read_button(BUTTON4) == 1){
				LED_RED;
			}
			

			if(HCSR04GetDistance()>20) 
			{
				LED_PURPLE;
			}

		if(mode == 0)
		{

			if (read_button(BUTTON1) == 1  ) // go to camera
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
				LED_PURPLE;
				camera_navbar();

				mode = 1;
			}
			if (read_button(BUTTON2) == 1 ) //go to home
			{

				gui_init();

			}
			if (read_button(BUTTON3) == 1) //images menu
			{

				images_menu();
				mode = 2;
			}
		}

		if(mode == 1)  //camera mode
		{ 	
			
			if (read_button(BUTTON1) == 1 ) // keep loading new image
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
				LED_PURPLE;

				mode = 1;
			}
			if (read_button(BUTTON2) == 1 ) //go to home
			{

				gui_init();
				mode = 0;

				
			}
			if (read_button(BUTTON3) == 1) //screenshot
			{

				
				name_count++;
				//file name in SD card
				sprintf(name, "0:DoorSurveillance_%d.bmp", name_count);
				LCD_DrawString(0, 200, name);
				//LED_BLUE;
				ILI9341_GramScan(cam_mode.lcd_scan);
				if (Screen_Shot(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH, name) == 0)
				{
					LED_GREEN;
				}
				
			}
		}

		if (mode ==2) // images menu
		{
			out_count = name_count;
			if (read_button(BUTTON1) == 1  ) // keep loading next image // down
			{
				sprintf(name, "0:DoorSurveillance_%d.bmp", out_count);
				LCD_Show_BMP(0,20,name);
				if(out_count!=0){
					out_count--;
				}
				
			}
			if (read_button(BUTTON2) == 1 ) //go to home
			{

				gui_init();
				mode = 0;

				
			}
			if (read_button(BUTTON3) == 1) //up
			{
				sprintf(name, "0:DoorSurveillance_%d.bmp", out_count);
				LCD_Show_BMP(0,20,name);
				if(out_count!=name_count){
					out_count++;
				}
				
				
			}
		}
	}
	
	
}

void MCO_CONFIG (void)
{		
	// /*
	//  *  Task 1 � Output SYSCLK via MCO.
	//  */
	
	//  GPIO_InitTypeDef GPIO_InitStructure; //initialize GPIO
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//Enable the RCC APB2 Peripheral Clock for the specific port
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //configure the corresponding pins pA8 MCO pin
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //Set the mode to alternate function push pull
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // set gpio speed
    // GPIO_Init(GPIOA, &GPIO_InitStructure); //init

    // RCC->CFGR |= (RCC_MCO_SYSCLK << 24); // RCC set 
	
	/*
	 *Task - 2 student id depedent clock on TIM3.
	 */
	
	/*
	a = 2
	b = 0
	c = 3
	d = 1
	e = 0
	f = 2
	g = 4
	h = 3
	
	frequency = 200khz
	duty = 00% => 70%
	
	72M / 200k = 360
	prescaler = 360/16 = 22.5
	overflow = 16
	*/
	 //GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //enable TIM3 clock
	
	//setup TIMx Timebase
	TIM_TimeBaseStructure.TIM_Period = 359; //TIMc_ARR register
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//output compare
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 251;  // TIMx_CCRx register
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	//enable TIM3
	TIM_Cmd(TIM3, ENABLE);


		// Task 6
		TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable); 
		TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
		
		// Enable TIM4
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
		TIM_TimeBaseStructure.TIM_Period = 4000; //50HZ 
		TIM_TimeBaseStructure.TIM_Prescaler = 0; 
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
		TIM_OCInitStructure.TIM_Pulse = 299; // both released / pressed

	
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
		TIM_OC1Init(TIM4, &TIM_OCInitStructure); 
		TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Gated); 
		TIM_SelectInputTrigger(TIM4, TIM_TS_ITR2);
		
		// Enable GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
    TIM_Cmd(TIM4, ENABLE);
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
