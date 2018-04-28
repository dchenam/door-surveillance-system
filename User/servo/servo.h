#ifndef _SERVO_H_
#define _SERVO_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"	  
#include "stm32f10x_tim.h"


#define SERVO_TIM	TIM3
#define SERVO_PORT GPIOC
#define SERVO_PIN1 GPIO_Pin_6
#define SERVO_PIN2 GPIO_Pin_7

typedef enum{
	SERVO1,
	SERVO2
}SERVO_ID;

void servo_init(u16 presc, u16 count, u16 init_pos);
void servo_control(SERVO_ID servo_id, u16 val);

#endif
