/************************************************
*蓝桥杯嵌入式第三届省赛题-“里程仪”
*CSDN：https://blog.csdn.net/qq_43715171
*码云：https://gitee.com/yiymou
*GitHub：https://github.com/YIYMOU
*硬件平台：国信长天CT117E
*作者：yiymou   2021-02-02
************************************************/
#include "stm32f10x.h"
#include "stdio.h"					// 使用了sprintf函数
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "i2c.h"
#include "key.h"
#include "pwm_octoggle.h"
#include "string.h"					// 使用了memset函数
#include "InputCapture.h"

u32 TimingDelay = 0;		// 延时函数的变量

uint16_t led_cnt = 0;
_Bool led_flag = 0;

uint16_t key_scan_cnt = 0;
uint16_t key_continue_cnt = 0;

uint8_t lcd_str[20];

uint8_t speed = 0;							// 用于设定速度
uint16_t total_time_cnt = 0;
uint32_t total_time = 0;				// 用于存放总时间

uint32_t total_meter = 0;				// 用于存放总里程数

union EEPROM				// 共用体，用于存放和读取里程数
{
	uint8_t uint8;
	uint16_t uint16;
	uint32_t uint32;
	uint8_t str[20];
} Write_EEPROM,Read_EEPROM;

void Delay_Ms(u32 nTime);

void Key_Scan(void)					// 按键扫描函数，10ms扫描一次
{
	Key_Read();			//更新一次按键状态
	if(falling_flag == K1)		// K1按下，“加速”按键，频率步进值为 1Hz
	{
		if(speed < 20)
		{
			if(speed == 0)
			{
				TIM_Cmd(TIM3, ENABLE);		// 把定时器打开，也就是允许产生方波
			}
			speed++;
			CCR1_Val = 1000 / speed;		// 设置方波的频率
		}
	}
	if(falling_flag == K2)		// K2按下，“减速”按键， 频率步进值为 1Hz
	{
		if(speed > 0)
		{
			speed--;
			if(speed == 0)
			{	
				TIM_Cmd(TIM3, DISABLE);			// 速度减到0，则把定时器关闭，不产生方波
			}
			else
			{
				CCR1_Val = 1000 / speed;		// 设置方波的频率
			}
		}
	}
	if(falling_flag == K3)		// K3未使用
	{
		
	}
	if(falling_flag == K4)		// K4未使用
	{
		
	}
	
	if(key_state == K1)				// 按键长按检测
	{
		
		if(key_continue_cnt == 50)		// 按键按下，超过500ms，表示长按
		{
			
		}
		else
		{
			key_continue_cnt++;		// 按键按下的时常设置，每+1，则表示时常加10ms
		}
	}
	
	if(key_state == 0x00)			// 按键松开
	{
		key_continue_cnt = 0;		// 按键松开，则把按键按下的时常清零
	}
}

//Main Body
int main(void)
{
	int i;
	SysTick_Config(SystemCoreClock/1000);			// 配置SySTick时钟为1ms
	Delay_Ms(200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		// 设置中断优先级组
	Led_Init();			// 初始化LED
	Tim4_Init();		// 初始化TIM4
	Key_Init();			// 初始化按键
	i2c_init();			// 初始化iic
	Pwm_Octoggle_Init();				// 初始化PWM输出
	TIM_Cmd(TIM3, DISABLE);			// 初始化PWM输出，此时速度为0，不产生方波，则把定时器关闭即可
	Time2_InputCapture_Init();	// 初始化定时器捕获
	STM3210B_LCD_Init();				// lcd初始化
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	for(i = 0; i < sizeof(uint32_t); i++)			// 将保存在eeprom中的行驶里程读取出来
	{
		Read_EEPROM.str[i] = Read_AT24C02(i);
	}
	total_meter = Read_EEPROM.uint32;
	while(1)
	{
		// 计算瞬时速度并显示到LCD
		memset(lcd_str,0,sizeof(lcd_str));	// 使用sprintf之前，先将lcd_str字符串清空
		sprintf((char*)lcd_str,"Vim(km/h): %-11.1f", speed==0?0:TIM2Freq*2.0*3.6);		// 这里判断speed是否为0的原因是，在每一次设置速度为0的时候，我是直接把定时器关闭了，TIM2Freq还是上一次的值，所以这里需要判断一下
		LCD_DisplayStringLine(Line0 ,lcd_str);
		// 计算平均速度并显示到LCD 
		memset(lcd_str,0,sizeof(lcd_str));	// 使用sprintf之前，先将lcd_str字符串清空
		sprintf((char*)lcd_str,"Vavg(km/h): %-8.1f", total_time==0?0:(total_meter_now*7.2/total_time));		// 这里判断total_time是为了防止除0的错误
		LCD_DisplayStringLine(Line2 ,lcd_str);
		// 显示行驶时间到LCD
		memset(lcd_str,0,sizeof(lcd_str));	// 使用sprintf之前，先将lcd_str字符串清空
		sprintf((char*)lcd_str,"T(h:m:s): %02d:%02d:%02d  ",total_time/3600,total_time%3600/60,total_time%60);
		LCD_DisplayStringLine(Line4 ,lcd_str);
		// 显示当前行驶的公里数到LCD
		memset(lcd_str,0,sizeof(lcd_str));	// 使用sprintf之前，先将lcd_str字符串清空
		sprintf((char*)lcd_str,"S(km): %12.1f",total_meter_now / 1000.0);
		LCD_DisplayStringLine(Line6 ,lcd_str);
		// 显示总行使的公里数到LCD
		memset(lcd_str,0,sizeof(lcd_str));	// 使用sprintf之前，先将lcd_str字符串清空
		sprintf((char*)lcd_str,"       Total(km):%3.0f", (total_meter + total_meter_now) / 1000.0);
		LCD_DisplayStringLine(Line9 ,lcd_str);
		
		if(TIM2Freq*2.0*3.6 < 90.0)		// 如果速度小于90km/h，LD1熄灭
		{
			Led_Ctrl(0x01,DISABLE);
		}
		
		if(TIM2Freq*2.0*3.6 >= 90.0 && led_flag)		// 如果速度大于90km/h，LD1以1s为间隔进行闪烁
		{
			led_flag = 0;
			Led_Toggle(0x01);
		}
		
		Write_EEPROM.uint32 = total_meter + total_meter_now;		// 总里程存放到共用体，便于之后进行保存到eeprom
		for(i = 0; i < sizeof(uint32_t); i++)		// 将当前的总里程存到eeprom
		{
			Write_AT24C02(i,Write_EEPROM.str[i]);
			Delay_Ms(5);
		}
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
/**
  * @brief  This function handles TIM4 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)		// 1ms中断
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		if(++led_cnt >= 1000)		// Led的1s中断
		{
			led_cnt = 0;
			led_flag = 1;
		}
		if(++key_scan_cnt >= 10)	// 10ms进行一次按键的扫描
		{
			Key_Scan();
		}
		if(speed && ++total_time_cnt >= 1000)	// 1s为单位记录行驶时间
		{
			total_time_cnt = 0;
			total_time++;
		}
  }
}
