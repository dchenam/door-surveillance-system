#ifndef __ESP8266_H
#define __ESP8266_H

#include "uart/uart.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "lcd.h"
#include "stm32f10x_dma.h"

#define BUFFER_LENGTH 8096

extern char ap_list[30][80];
extern u8 ap_pass_check[30];

void clear_buffer(void);
void ESP8266_init(void);
void ESP8266_enter(char *txt);
void ESP8266_Delayus(int duration);

u8 ESP8266_wifi_init(void);
u8 ESP8266_wifi_ap_connect(void);
u8 get_weather_data();
void show_weather_data();

#endif