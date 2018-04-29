#ifndef RTC_TIMER
#define RTC_TIMER

#include <stm32f10x.h>
#include <stdio.h>
#include <stm32f10x_pwr.h>
#include <stm32f10x_bkp.h>
#include <stm32f10x_rtc.h>

extern uint32_t hr, min, sec, time;
extern u8 timer_set;
extern uint32_t temp_freq, light_freq;
extern uint32_t temp_next, light_next;
extern u8 light_go, temp_go;
extern uint32_t camera_time;

void RTC_Timer_Init(void);
void set_time(uint32_t hr, uint32_t min, uint32_t sec) ;
void set_alarm(uint32_t time) ;
void cancel_alarm(void);
void timer_update(void);
u8 temp_time_update(uint32_t cur_t);
u8 update_light_sensor(uint32_t cur_t);
			
#endif