#ifndef SMARTHOME_GUI_H
#define SMARTHOME_GUI_H

#include "./lcd/lcd.h"
#include <stdlib.h>
#include <stdio.h>
//#include "./ESP8266/ESP8266.h"
//#include "rtc_timer.h"
// #include "dht11.h"



typedef enum {main_menu = 1, settings_menu = 2, wifi_setting = 3, time_setting = 4, sensor_setting = 5, other_menu = 6,  other_menu_sys_time = 7, camera_mode = 8, change_dht11 = 9, change_ldr = 10, wifi_choose = 11, init = 0 } disp_state;

// typedef struct {
// 	u8 low_temp_state, 
// 			low_temp_val,
// 			high_temp_state,
// 			high_temp_val;
// } switch_setttings;

extern char devicename[24];
extern disp_state cur_state;
extern disp_state prev_state;

extern uint32_t timer_hr, timer_min, timer_sec;

void gui_init(void);
void draw_toolbar(void);
void draw_mainmenu(void);
void draw_homenav(void);
void draw_toggle_switch(u8 state);
void update_toolbar(void);
void toggle_switch();
void go_camera_mode(void);
void show_menu(void);
void camera_navbar(void);
void images_menu(void);
void image_navbar(void);
//void wifi_temp_update(u8 key);
//void wifi_ap_connect_test(uint16_t y_coord);

//void wifi_ap_search();

#endif 