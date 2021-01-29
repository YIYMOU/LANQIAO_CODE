#include "stm32f10x.h"
#include "lcd.h"
#include "LED.h"
#include "stdio.h"
#include "i2c.h"
#include "string.h"

u32 TimingDelay = 0;

uint8_t lcd_string[20];

//利用共用体读写EEPROM
union eeprom_dat
{
	uint8_t t1;
	uint16_t t2;
	uint32_t t3;
	int16_t t4;
	float f1;
	double f2;
	unsigned char str[20];
}eeprom_dat_write,eeprom_dat_read;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	uint8_t i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	STM3210B_LCD_Init();
	LED_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	i2c_init();					//I2C总线初始化
	
	strcpy((char *)eeprom_dat_write.str,"Hello World!!!");
	for(i = 0; i < strlen((char *)eeprom_dat_write.str); i++)
	{
		Write_eeprom(i,eeprom_dat_write.str[i]);
		Delay_Ms(5);
	}
	for(i = 0; i < strlen((char *)eeprom_dat_write.str); i++)
	{
		eeprom_dat_read.str[i] = Read_eeprom(i);
	}
	sprintf((char*)lcd_string,"%s      ",eeprom_dat_read.str);
	LCD_DisplayStringLine(Line4,(unsigned char *)lcd_string);	
	
	eeprom_dat_write.f2 = 3.1415926535;
	for(i = 0; i < sizeof(eeprom_dat_write.f2); i++)
	{
		Write_eeprom(i,eeprom_dat_write.str[i]);
		Delay_Ms(5);
	}
	for(i = 0; i < sizeof(eeprom_dat_write.f2); i++)
	{
		eeprom_dat_read.str[i] = Read_eeprom(i);
	}
	sprintf((char*)lcd_string,"%f        ",eeprom_dat_read.f2);
	LCD_DisplayStringLine(Line5,(unsigned char *)lcd_string);	
	
	while(1){}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
