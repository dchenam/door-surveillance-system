#include "ESP8266.h"

const char table_begin[10] = "<table";
const char table_end[10] = "</table>";
static char another_buffer[8096];
uint8_t buffer[8096] = {'0'};
uint8_t txBuf[100]; // = "AT+CWLAP\r\n";
uint8_t rxBuf[64];
uint16_t rx_buf_idx = 0;
u8 transmit_done = 0;
uint16_t prev_count = 0;
uint16_t cur_count;

char data[2][30][2][32];
char ap_list[30][80];
u8 ap_pass_check[30];

int cur_buf_idx = 0;
char *AP_SSID = "hongseo";
char *AP_PW = "password";
GPIO_InitTypeDef GPIO_InitStr;

int c = 0;
int r = 100;
u8 flag = 0;

static char line[300];
static int index = 0;
static int num = 0;
static int table_found = 0;
static int stop = 0;
static int idx_begin = 0;
static int idx = 0;

static int s1 = 0;
static int s2 = 0;
static int sub = 0;

static char buffer_loc[32];
static char buffer_temp[32];
static int buf_loc_idx = 0;
static int buf_temp_idx = 0;

void DMA1_Channel5_IRQHandler()
{
	if (DMA_GetITStatus(DMA1_IT_TC5))
	{
		if (strstr(rxBuf, "OK\r\n") != NULL)
			transmit_done = 1;
		memcpy(buffer + 64 * rx_buf_idx, rxBuf, 64);
		rx_buf_idx++;
		DMA_ClearITPendingBit(DMA1_IT_GL5);
	}
}

void clear_buffer()
{
	memset(buffer, '\0', 8096);
	cur_buf_idx = 0;
}

void print_to_lcd()
{
	LCD_Clear(LCD_Default_Max_COLUMN, LCD_Default_Max_PAGE, LCD_Default_Max_COLUMN, LCD_Default_Max_PAGE, BLUE);
	//	LCD_DrawString(0, 16, buffer);
}

void uart_listener(const u8 byte)
{
	/*
	if (byte == 0xD) {
		flag = 1;
	}
	else if (byte == 0xA) {
		if (flag) {
			r = r + 16;
			c = 0;
			flag = 0;
		}
	}

	else {
	if (c <= 232) {
		LCD_DrawChar(c , r, byte, BLACK, WHITE);
		c = c + 8;
	}
	else if (r <= 304) {
		c = 0;
		r = r + 16;
		LCD_DrawChar(c, r, byte, BLACK, WHITE);
	}
	else {
		//LCD_OpenWindow(0, 0, LCD_DispWindow_COLUMN, LCD_DispWindow_PAGE);
	 // LCD_Clear(0, 0, LCD_DispWindow_COLUMN, LCD_DispWindow_PAGE, BLUE);
	  c = 0; r = 100; 
	LCD_DrawChar(c, r, byte, BLACK, WHITE);
    c = c + 8;
	}
}
*/
	if (cur_buf_idx < BUFFER_LENGTH)
		buffer[cur_buf_idx++] = byte;
}

void ESP8266_enter(char *txt)
{
	clear_buffer();
	char buff[64];
	sprintf(buff, "%s%s", txt, "\r\n");
	uart_tx(COM1, buff); // AndroidHotspot0	981
}

static void ESP_DMA_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = (sizeof(buffer) / sizeof(*(buffer))) - 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
}

void ESP8266_init()
{
	DMA_InitTypeDef DMA_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/*
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStr.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStr.GPIO_Pin = GPIO_Pin_8; //| GPIO_Pin_9;
	GPIO_InitStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStr);
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
	*/

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txBuf; // txbuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = (sizeof(txBuf) / sizeof(*(txBuf))) - 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	//DMA_Init(DMA1_Channel4, &DMA_InitStructure);

	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = (sizeof(buffer) / sizeof(*(buffer))) - 1;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	//DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
	/*	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);

	USART_DMACmd(USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
	//DMA_Cmd(DMA1_Channel4, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);

	USART_Cmd(USART1, ENABLE);

	cur_count = DMA_GetCurrDataCounter(DMA1_Channel5);
	/*
	uart_init(COM1, 9600); //74880
	uart_interrupt_init(COM1, &uart_listener);
	//clear_buffer();
	cur_buf_idx = 0;
	*/
}

static void uart_send(const char *tx_buf, ...)
{
	va_list arglist;
	char buf[255], *fp;

	va_start(arglist, tx_buf);
	vsprintf(buf, tx_buf, arglist);
	va_end(arglist);

	fp = buf;
	while (*fp)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			;
		USART_SendData(USART1, (u16)(*fp++));
	}
}

u8 ESP8266_wifi_init()
{

	//if (buffer[0] == 'O' && buffer[1] == 'K')
	//response = 1;

	//clear_buffer();;
	//cur_buf_idx = 0;
}

u8 ESP8266_wifi_ap_connect()
{
	clear_buffer();
	prev_count = 8096 - DMA_GetCurrDataCounter(DMA1_Channel5);
	//	DMA_DeInit(DMA1_Channel5);
	//ESP_DMA_Init();
	/*
	ESP8266_enter("AT+CWLAP");
	ESP8266_Delayus(100000);
	ESP8266_Delayus(100000);
	ESP8266_Delayus(100000);
	ESP8266_Delayus(100000);
	ESP8266_Delayus(100000);
	ESP8266_Delayus(100000);	
*/
	/*
	char atcmd[11] = "AT+CWLAP\r\n";
	memcpy(txBuf, atcmd, 11);
	*/
	//while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET) {}
	uart_send("AT+CWLAP\r\n");
	//while (DMA_GetFlagStatus(DMA1_FLAG_HT5) == RESET) {}

	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	//	ESP8266_Delayus(1000000);
	//	ESP8266_Delayus(1000000);
	//	ESP8266_Delayus(1000000);
	//	ESP8266_Delayus(1000000);
	//	ESP8266_Delayus(1000000);
	//	ESP8266_Delayus(1000000);
	//	ESP8266_Delayus(1000000);

	//char another_buffer[8096];

	//char *stptr = strstr(buffer, "AT+CWLAP\r\n");
	//cur_count = 8096 - DMA_GetCurrDataCounter(DMA1_Channel5);
	//prev_count = stptr - buffer;
	memcpy(another_buffer, buffer + prev_count - 1, (8096 - prev_count));
	memcpy(another_buffer + (8096 - prev_count), buffer, prev_count);
	//prev_count = cur_count;

	u8 result = 0;
	memset(ap_list, '\0', 30 * 80 * sizeof(char));

	char line[100];
	int stop = 0;
	int idx = 0;
	int num = 0;
	char *ptr;

	if (strstr(another_buffer, "ERROR") != NULL)
	{
		stop = 1;
		result = 1;
	}

	while (stop == 0)
	{
		char *ptrr = strstr(another_buffer + idx, "+CWLAP:");
		if (ptrr == NULL)
		{
			result = 0;
			break;
		}

		//	int idx_begin = ptrr - another_buffer;
		idx_begin = ptrr - another_buffer;

		idx = idx_begin;
		while ((another_buffer[idx] != '\r') && idx < 8096)
		{
			idx++;
			//	idx++;
			//	if (idx >= 1024) {
			//		stop = 1;
			//		break;
			//	}
			//	}
		}
		if (idx >= 8096)
			stop = 1;

		if (stop != 0)
			break;

		memset(line, '\0', 100 * sizeof(char));
		/*
		for (int i = 0; i < idx -idx_begin; i++) {
			line[i] = another_buffer[idx_begin + i];
		}
		*/
		memcpy(line, ptrr, idx - idx_begin);

		if (strstr(line, "AT+CWLAP") != NULL)
		{
			idx = idx + 2;
			continue;
		}

		if (strstr(line, "OK") != NULL)
		{
			stop = 1;
			break;
		}

		if (line[8] - '0' > 0)
			ap_pass_check[num] = 1;
		else if (line[8] - '0' == 0)
			ap_pass_check[num] = 0;

		char *first_ptr;
		ptr = strstr(line, "\"");

		if (ptr != NULL)
		{
			first_ptr = ptr;
		}
		else
		{
			stop = 1;
			result = 1;
			break;
		}

		ptr = strstr(first_ptr + 1, "\"");
		if (ptr != NULL)
		{
			if (ptr - first_ptr - 1 >= 80)
			{
				memcpy(ap_list[num], first_ptr + 1, 80);
			}
			else
				memcpy(ap_list[num], first_ptr + 1, ptr - first_ptr - 1);
		}
		else
		{
			stop = 1;
			result = 1;
			break;
		}
		num++;
		idx = idx + 2;
	}
	//DMA_ClearFlag(DMA1_FLAG_TC5);
	DMA1_Channel5->CNDTR = 0x1FA0;
	return result;
}

void test_listener(const u8 byte)
{
	//uart_tx_byte(COM3, byte);
	if (byte == 0xD)
	{
		flag = 1;
	}
	else if (byte == 0xA)
	{
		if (flag)
		{
			r = r + 16;
			c = 0;
			flag = 0;
		}
	}

	else
	{
		if (c <= 232)
		{
			LCD_DrawChar(c, r, byte, BLACK, WHITE);
			c = c + 8;
		}
		else if (r <= 304)
		{
			c = 0;
			r = r + 16;
			LCD_DrawChar(c, r, byte, BLACK, WHITE);
		}
		else
		{
			//LCD_OpenWindow(0, 0, LCD_DispWindow_COLUMN, LCD_DispWindow_PAGE);
			// LCD_Clear(0, 0, LCD_DispWindow_COLUMN, LCD_DispWindow_PAGE, BLUE);
			c = 0;
			r = 100;
			LCD_DrawChar(c, r, byte, BLACK, WHITE);
			c = c + 8;
		}
		if (cur_buf_idx < BUFFER_LENGTH)
			buffer[cur_buf_idx++] = byte;
	}
}

u8 get_weather_data()
{
	uart_send("AT+CIPMUX=1\r\n");
	ESP8266_Delayus(100000);
	uart_send("AT+CIPSTART=4,\"TCP\",\"rss.weather.gov.hk\",80\r\n");
	ESP8266_Delayus(1000000);
	uart_send("AT+CIPSEND=4,66\r\n");
	ESP8266_Delayus(1000000);
	clear_buffer();
	uint16_t prev_ptr = 8096 - DMA_GetCurrDataCounter(DMA1_Channel5);
	uart_send("GET /rss/CurrentWeather.xml HTTP/1.1\r\nHost:rss.weather.gov.hk\r\n\r\n\r\n");

	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);
	ESP8266_Delayus(1000000);

	//	char another_buffer[8096];

	if (strstr(buffer, "ERROR") != NULL)
	{
		return 1;
	}

	//char *stptr = strstr(buffer, "AT+CWLAP\r\n");
	//cur_count = 8096 - DMA_GetCurrDataCounter(DMA1_Channel5);
	//prev_count = stptr - buffer;
	memcpy(another_buffer, buffer + prev_ptr - 1, (8096 - prev_ptr));
	memcpy(another_buffer + (8096 - prev_ptr), buffer, prev_ptr);
	//prev_count = cur_count;

	//	char *rmv;
	//	while ( (rmv = strstr(another_buffer, "+IPD")) != NULL) {
	//		char *rmv_end = strstr(rmv, ":");
	//		for (int i = rmv - another_buffer - 2; i < BUFFER_LENGTH - 2 - (rmv_end - rmv); i++)
	//				another_buffer[i] = another_buffer[i + 2 + (rmv_end - rmv)];
	//		for (int i = BUFFER_LENGTH - 2 - (rmv_end - rmv); i < BUFFER_LENGTH; i++)
	//				another_buffer[i] = '\0';
	//	}

	char *rmv;
	while ((rmv = strstr(another_buffer, "+IPD")) != NULL)
	{
		char *rmv_end = strstr(rmv, ":");
		memcpy(rmv, rmv_end + 1, 8096 - (rmv_end + 1 - another_buffer));
		memset(rmv + 8096 - (rmv_end + 1 - another_buffer), '\0', (rmv_end - rmv));
	}

	memset(data, '\0', 2 * 30 * 2 * 32 * sizeof(char));
	/*
	char line[300];
	int index = 0;
	int num = 0;
	int table_found = 0;
	int stop = 0;

	int idx = 0;
	*/
	index = 0;
	num = 0;
	table_found = 0;
	stop = 0;
	idx = 0;
	if (another_buffer != NULL)
	{

		while (stop == 0)
		{
			idx_begin = idx;
			if (idx >= BUFFER_LENGTH)
			{
				stop = 1;
				break;
			}
			while ((another_buffer[idx] != '\r'))
			{
				idx++;
				if (idx >= BUFFER_LENGTH || another_buffer[idx] == EOF)
				{
					stop = 1;
					break;
				}
			}
			if (stop != 0)
				break;
			;
			memset(line, '\0', 300 * sizeof(char));

			for (int i = 0; i < idx - idx_begin; i++)
			{
				line[i] = another_buffer[idx_begin + i];
			}
			//memcpy(line, &weather_data[idx_begin], idx - idx_begin);

			//		int s1 = 0;
			//int s2 = 0;
			//		int sub = 0;
			s1 = 0;
			s2 = 0;
			sub = 0;

			//	char buffer_loc[32];
			//	char buffer_temp[32];
			//			int buf_loc_idx = 0;
			//	int buf_temp_idx = 0;
			buf_loc_idx = 0;
			buf_temp_idx = 0;

			for (int i = 0; i < 32; i++)
				buffer_loc[i] = '\0';

			for (int i = 0; i < 32; i++)
				buffer_temp[i] = '\0';

			if (strstr(line, table_begin) != NULL)
				table_found = 1;

			if (table_found == 1)
			{
				if (strstr(line, table_end) != NULL)
				{
					index = 1;
					num = 0;
					table_found = 0;
				}
				for (int i = 0; i < idx - idx_begin; i++)
				{
					if (line[i] == '>')
						s1 = 1;
					else if ((s1 == 1) && (line[i] == '<'))
						s1 = 0;
					else if ((s1 == 1) && (line[i] != '<'))
					{
						s2 = 1;
						if (sub == 0)
						{
							buffer_loc[buf_loc_idx] = line[i];
							buf_loc_idx++;
						}
						if (sub > 0)
						{
							buffer_temp[buf_temp_idx] = line[i];
							buf_temp_idx++;
						}
					}
					if ((s2 == 1) && (line[i] == '<'))
					{
						s2 = 0;
						s1 = 0;
						sub++;
					}
				}
				if (buffer_temp[0] != '\0' && buffer_loc[0] != '\0')
				{
					//cout << "buffer_loc: " << buffer_loc << endl;
					//cout << "buffer_temp:" << buffer_temp << endl;
					for (int i = 0; i < 32; i++)
					{
						data[index][num][0][i] = buffer_loc[i];
						data[index][num][1][i] = buffer_temp[i];
					}
					num++;
				}
			}
			idx = idx + 2;
		}

		return 0;
	}
	return 1;
}

void show_weather_data()
{
	/*	LCD_Clear ( 0, 0, 240, 320, BACKGROUND);
		for (int i = 0; i < 2*30; i++) {
		if (data[0][i][0][0] != '\0' && data[0][i][0][1] != '\0') 
			LCD_DrawString(0, i * 16, data[0][i][0], YELLOW, BLACK);
			i++;
			LCD_DrawString(0, i * 16, data[0][i][1], RED, BLACK);
		}
	*/
	char temp_buf[2];
	temp_buf[0] = data[0][8][1][0];
	temp_buf[1] = data[0][8][1][1];
	LCD_DrawString(10, 221, data[0][8][0], BLACK, BACKGROUND);
	LCD_DrawString(10, 237, temp_buf, BLUE, BACKGROUND);
	LCD_DrawString(10 + 8 * 2, 237, " degrees", BLACK, BACKGROUND);
}

void ESP8266_Delayus(int duration)
{
	while (duration--)
	{
		int i = 0x02;
		while (i--)
			__asm("nop");
	}
}
