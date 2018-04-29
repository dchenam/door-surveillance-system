#include "rtc_timer.h"
#include "lcd.h"

uint32_t hr = 0;
uint32_t min = 0;
uint32_t sec  = 0;
uint32_t time = 0;
u8 timer_set = 0;
uint32_t temp_freq = 15;
uint32_t temp_next = 0;
uint32_t light_freq = 8;
uint32_t light_next = 0;
u8 light_go = 0; u8 temp_go = 0;
uint32_t camera_time = 0;

void RTC_Timer_Init() {

	
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	BKP_DeInit();
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {}
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_SEC | RTC_IT_ALR, ENABLE);
	RTC_WaitForLastTask();
	RTC_SetPrescaler(32767);
	RTC_WaitForLastTask();
		 
}

void set_time(uint32_t hr, uint32_t min, uint32_t sec) {
	uint32_t new_time = hr * 3600 + min * 60 + sec;
	RTC_WaitForLastTask();
	RTC_SetCounter(new_time);
	RTC_WaitForLastTask();
}

void set_alarm(uint32_t t) {
	uint32_t temp = t + RTC_GetCounter() - 1;
	if (temp > 0x0001517F)
		temp = temp - 0x0001517F;
	
	RTC_SetAlarm(temp);
}

void cancel_alarm(void) {
	RTC_WaitForLastTask();
	RTC_EnterConfigMode();
	RTC->ALRH = 0xFFFF;
	RTC->ALRL = 0xFFFF;
	RTC_ExitConfigMode();
	RTC_WaitForLastTask();
}
	

void timer_update(void) {
	time = RTC_GetCounter();
	hr = time / 3600;
	min = (time % 3600) / 60;
	sec  = (time % 3600) % 60;	
}

u8 temp_time_update(uint32_t cur_t) {
	u8 result = 0;
	
	if (cur_t == temp_next) {
		temp_next += temp_freq;
		result = 1;
	}
	
	if (temp_next > 0x0001517F) 
		temp_next  = temp_next - 0x0001517F;
	
	return result;
}

u8 update_light_sensor(uint32_t cur_t) {
	u8 result = 0;
	
	if (cur_t == light_next) {
		light_next += light_freq;
		result = 1;
	}
	
	if (light_next > 0x0001517F) 
		light_next  = light_next - 0x0001517F;
	
	return result;
}


