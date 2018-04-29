#include "gui.h"

static uint16_t wifi_icon[64][2] = { {7,4}, {8,4}, {9,4}, {10,4}, {11,4}, {12,4}, {5,5}, {6,5}, {7,5}, {8,5}, {9,5}, {10,5}, {11,5}, {12,5}, {13,5}, {14,5}, {3,6}, {4,6}, {5,6}, {14,6}, {15,6}, {16,6}, {2,7}, {3,7}, {8,7}, {9,7}, {10,7}, {11,7}, {16,7}, {17,7}, {2,8}, {6,8}, {7,8}, {8,8}, {9,8}, {10,8}, {11,8}, {12,8}, {13,8}, {17,8}, {4,9}, {5,9}, {6,9}, {13,9}, {14,9}, {15,9}, {4,10}, {5,10}, {9,10}, {10,10}, {14,10}, {15,10}, {7,11}, {8,11}, {9,11}, {10,11}, {11,11}, {12,11}, {7,12}, {12,12}, {9,14}, {10,14}, {9,15}, {10,15} };

//static void draw_toggle_switch(u8 state);
static void add_button(uint16_t col, uint16_t page, u8 width, u8 height, const char* text);
static void add_image(uint16_t col, uint16_t page, uint16_t** icon, uint16_t color);
static void fill_color_rect(uint16_t col, uint16_t page, uint16_t width, uint16_t height, uint16_t color);
static void put_big_num(uint16_t col, uint16_t page, u8 num, uint16_t color);



char devicename[24] = "Door Surveilance\0";
disp_state cur_state = init;
disp_state prev_state;
u8 switch_state = 0;


void gui_init(void) {
	//enum cur_disp = main_menu;
	LCD_Clear(0, 0, 240, 320, BACKGROUND);
	draw_toolbar();
	draw_mainmenu();
	
}


void draw_toolbar() {
	static uint16_t i;
	static uint16_t j;
	for (i = 0; i < 240; i++) {
		for (j = 0; j < 20; j++) {
			LCD_DrawDot(i, j, BLACK);
		}
	}
	update_toolbar(); // For time and checking if its camera mode
}

	

void draw_mainmenu() {
	cur_state = main_menu;
	static uint16_t i;
	
	LCD_Clear(0, 20, 240, 300, BACKGROUND);

    //0
    fill_color_rect(120, 67, 20, 30, BLACK); 
    fill_color_rect(124, 71, 12, 23, BACKGROUND);
    //9
    fill_color_rect(143, 67, 18, 19, BLACK); 
    fill_color_rect(145, 71, 12, 12, BACKGROUND);
    fill_color_rect(157, 86, 4, 11, BLACK); 

    //th
    fill_color_rect(162, 60, 13, 3, GREY); 
    fill_color_rect(167, 62, 3, 12, GREY);
    fill_color_rect(176, 60, 3, 14, GREY);
    fill_color_rect(177, 65, 5, 3, GREY);
    fill_color_rect(180, 60, 3, 14, GREY);

    //M
    fill_color_rect(179, 81, 3, 16, BLACK);
    fill_color_rect(182, 83, 3, 3, BLACK);
    fill_color_rect(184, 86, 3, 3, BLACK);
    fill_color_rect(186, 89, 3, 3, BLACK);
    fill_color_rect(188, 91, 3, 3, BLACK);
    fill_color_rect(190, 89, 3, 3, BLACK);
    fill_color_rect(192, 86, 3, 3, BLACK);
    fill_color_rect(194, 83, 3, 3, BLACK);
    fill_color_rect(196, 81, 3, 16, BLACK);

    //A
    fill_color_rect(202, 94, 3, 3, BLACK);
    fill_color_rect(203, 91, 3, 3, BLACK);
    fill_color_rect(204, 88, 3, 3, BLACK);
    fill_color_rect(205, 85, 3, 3, BLACK);
    fill_color_rect(206, 82, 3, 3, BLACK);
    fill_color_rect(207, 81, 3, 3, BLACK);
    fill_color_rect(208, 82, 3, 3, BLACK);
    fill_color_rect(209, 85, 3, 3, BLACK);
    fill_color_rect(210, 88, 3, 3, BLACK);
    fill_color_rect(211, 91, 3, 3, BLACK);
    fill_color_rect(212, 94, 3, 3, BLACK);
    fill_color_rect(206, 88, 7, 3, BLACK);


    //Y
    fill_color_rect(220, 94, 3, 3, BLACK);
    fill_color_rect(221, 91, 3, 3, BLACK);
    fill_color_rect(222, 88, 3, 3, BLACK);
    fill_color_rect(223, 85, 3, 3, BLACK);
    fill_color_rect(224, 82, 3, 3, BLACK);
    fill_color_rect(225, 81, 3, 3, BLACK);
    fill_color_rect(220, 85, 3, 3, BLACK);
    fill_color_rect(218, 82, 3, 3, BLACK);
    fill_color_rect(216, 81, 3, 3, BLACK);
    

	
	
    //add_button(10, 220, 60, 30, "UPDATE");
	
  //LCD_DrawString(20, 235, "CHECK INTERNET CONNECTION", RED, BACKGROUND);

}


// void toggle_switch() {
// 		if (cur_state == main_menu)
// 			draw_toggle_switch(switch_state);
// 		servo_switch(switch_state);
// }




void draw_wifimenu() {
	cur_state = wifi_setting;
	//cur_disp = wifi_setting;
	LCD_Clear(0, 20, 240, 300, BACKGROUND);
	add_button(0, 20, 50, 30, "Back");
	add_button(0, 290, 240, 30, "Find WiFi AP");
	//connection with wifi module needed here
}

void update_toolbar() {
	//	static uint16_t i = 0;
	LCD_Clear(0, 0, 240, 20, BLACK);
	LCD_DrawStringwc(0, 2, devicename,YELLOW,BLACK);
	//LCD_DrawString(164, 2, time_dummy, YELLOW, BLACK);
	//update_time();
		for (uint16_t i = 0; i < 64; i++){
		LCD_DrawDot(wifi_icon[i][0]+220, wifi_icon[i][1], YELLOW);
	}
	//if (wifi_state) {
		//add_image(220, 0, wifi_icon, YELLOW);
	//}
}


void go_camera_mode(void) {
	prev_state = cur_state;
	cur_state = camera_mode;
	LCD_Clear(0,0, 240, 320, BACKGROUND);
	// camera_time = RTC_GetCounter() + 5;
	// if (camera_time > 0x0001517F)
	// 	camera_time = camera_time - 0x0001517F;
}

void show_menu() {
		update_toolbar();
	switch(cur_state) {
		case 1:
			draw_mainmenu();
			break;
		case 3:
			draw_wifimenu();
			break;
	}
}



static void add_button(uint16_t col, uint16_t page, u8 width, u8 height, const char* text) {
	static uint16_t i;
	static uint16_t j;

	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			if ((i == 0) || (i == width - 1) || (j == 0) || (j == height -1)) {
				LCD_DrawDot(col + i, page + j, BLACK);
			}
			else {
				LCD_DrawDot(col + i, page + j, BCOLOR);
			}
		}
	}
	uint16_t test_x = (uint16_t)sizeof(text);
	uint16_t x = col + (width - 8*(uint16_t)sizeof(text))/2;
	LCD_DrawStringwc(col + (width - 8*(uint16_t)strlen(text))/2, page + (height - 16)/2, text, BLACK, BCOLOR);
}

static void add_image (uint16_t col, uint16_t page, uint16_t** icon, uint16_t color) {
	static uint16_t i = 0; 
	while (i < (uint16_t)(sizeof(icon))/2) {
		LCD_DrawDot(icon[i][0] + col, icon[i][1] + page, color);
		i++;
	}
}

static void fill_color_rect(uint16_t col, uint16_t page, uint16_t width, uint16_t height, uint16_t color) {
	//puts 10x10 sqaure
	static uint16_t i;
	static uint16_t j;

	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			LCD_DrawDot(col + i, page + j, color);
		}
	}
}

static void put_big_num(uint16_t col, uint16_t page, u8 num, uint16_t color) {
	static uint16_t i;
	static uint16_t j;
	for (i = 0; i < 40; i++) {
		for (j = 0; j < 70; j++) {
			LCD_DrawDot(col + i, page + j, color);
		}
	}

	switch(num) {
		case 0:
			fill_color_rect(col + 10, page + 10, 20, 50, BACKGROUND);
			break;
		case 1:
			fill_color_rect(col  +0, page + 0, 30, 70, BACKGROUND);
			break;
		case 2:
			fill_color_rect(col + 0, page + 10, 30, 20, BACKGROUND);
			fill_color_rect(col + 10, page + 40, 30, 20, BACKGROUND);
			break;
		case 3:
			fill_color_rect(col + 0, page + 10, 30, 20, BACKGROUND);
			fill_color_rect(col + 0, page + 40, 30, 20, BACKGROUND);
			break;
		case 4:
			fill_color_rect(col + 10, page + 0, 20, 30, BACKGROUND);
			fill_color_rect(col + 0, page + 40, 30, 30, BACKGROUND);
			break;		
		case 5:
			fill_color_rect(col + 10, page + 10, 30, 20, BACKGROUND);
			fill_color_rect(col + 0, page + 40, 30, 20, BACKGROUND);
			break;		
		case 6:
			fill_color_rect(col + 10, page + 10, 30, 20, BACKGROUND);
			fill_color_rect(col + 10, page + 40, 20, 20, BACKGROUND);
			break;			
		case 7:
			fill_color_rect(col + 10, page + 10, 20, 60, BACKGROUND);
			fill_color_rect(col + 0, page + 30, 10, 40, BACKGROUND);
			break;	
		case 8:
			fill_color_rect(col + 10, page + 10, 20, 20, BACKGROUND);
			fill_color_rect(col + 10, page + 40, 20, 20, BACKGROUND);
			break;
		case 9:
			fill_color_rect(col + 10,page + 10, 20, 20, BACKGROUND);
			fill_color_rect(col + 0, page + 40, 30, 30, BACKGROUND);
			break;	
	}
}