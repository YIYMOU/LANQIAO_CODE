/************************************************
*蓝桥杯嵌入式——第四届省赛题-双路输出控制器
*CSDN：https://blog.csdn.net/qq_43715171
*码云：https://gitee.com/yiymou
*GitHub：https://github.com/YIYMOU
*硬件平台：国信长天CT117E
*作者：yiymou   2021-02-04
************************************************/
#include "stm32f10x.h"
#include "lcd.h"						// LCD头文件
#include "led.h"						// LED头文件	
#include "tim.h"
#include "uart.h"
#include "i2c.h"
#include "stdio.h"					// 使用了sprintf函数
#include "string.h"					// 使用了memset函数
#include "key.h"
#include "pwm_octoggle.h"
#include "rtc.h"

u32 TimingDelay = 0;				// 延时函数的变量

uint16_t key_scan_cnt = 0;			// 按键检测变量，10ms检测一次
// uint16_t key_continue_cnt = 0;	// 长按时间检测变量，本次赛题没有用到按键的长按
_Bool key_scan_flag = 0;				// 按键扫描标志

_Bool rx_flag = 0;			// 接收到串口数据的变量

_Bool ch2_enable = 0;		// 通道二使能输出的标志
_Bool ch3_enable = 0;		// 通道三使能输出的标志

uint8_t lcd_str[20];		// 专门用来在LCD上显示的字符串

union EEPROM						// 用于向eeprom读写数据的共用体
{
	uint8_t uint8;
	uint16_t uint16;
	uint32_t uint32;
	uint8_t str[20];
} Write_EEPROM,Read_EEPROM;

struct COMMEND					// 存放串口接收到的命令经过转换后的数据的结构体
{
	uint8_t x;						// PAx通道
	uint8_t y;						// 持续输出 y 秒（0<y<10）
	uint32_t sec_cnt;			// 定时器计数的变量
	uint8_t cnt_flag;			// 开启定时的标志
	uint32_t cmd_hh;			// hh时 
	uint32_t cmd_mm;			// mm分
	uint32_t cmd_ss;			// ss秒
	_Bool cmd_flag;
} RX_COMMEND = {0,0,0,0,0,0,0,0};

void Delay_Ms(u32 nTime);		// 延时函数

void PAx_Output(uint16_t GPIO_Pin_x,FunctionalState NewState)	// 将引脚配置为推挽输出，只能输出高低电平
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	if(NewState == ENABLE)
		GPIO_SetBits(GPIOA,GPIO_Pin_x);
	else
		GPIO_ResetBits(GPIOA,GPIO_Pin_x);
}

void PAx_Output_PWM(uint16_t GPIO_Pin_x)	// 将引脚配置为模拟输出，允许输出PWM信号
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Pwm_Enable(void)	// 配置PWM，在输出PWM的时候，关闭串口，不输出PWM时，打开串口
{
	// 通道2
	if(!ch2_enable || CH2Duty == 0)					// 关闭通道，或者占空比为0，输出低电平
	{
		PAx_Output(GPIO_Pin_1,DISABLE);
	}
	else if(ch2_enable && CH2Duty == 100)		// 开启通道，且占空比为100%，输出高电平
	{
		PAx_Output(GPIO_Pin_1,ENABLE);
	}
	else	// 其他情况，正常输出PWM波
	{
		PAx_Output_PWM(GPIO_Pin_1);
	}
	
	// 通道3
	if(ch3_enable && CH3Duty == 100)					// 关闭通道，或者占空比为0，输出低电平
	{
		PAx_Output(GPIO_Pin_2,ENABLE);
	}
	else if(!ch3_enable || CH3Duty == 0)		// 开启通道，且占空比为100%，输出高电平
	{
		PAx_Output(GPIO_Pin_2,DISABLE);
	}
	else	// 其他情况，正常输出PWM波
	{
		PAx_Output_PWM(GPIO_Pin_2);
	}
	
	if(ch2_enable == 0 && ch3_enable == 0)	// 通道2和通道3都闭关的时候，关闭定时器2，打开串口
	{	// 一定要先关定时器，再关闭时钟
		/* TIM2 diable counter */
		TIM_Cmd(TIM2, DISABLE);
		/* TIM2 clock diable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
		
		/* Enable USART2 Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		/* Enable the USART2 */
		USART_Cmd(USART2, ENABLE);
	}
	else		// 有通道开启的时候，关闭串口，打开定时器
	{
		/* diable the USART2 */
		USART_Cmd(USART2, DISABLE);
		/* diable USART2 Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);
		
		/* TIM2 clock enable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		/* TIM2 enable counter */
		TIM_Cmd(TIM2, ENABLE);

	}
}

void Key_Scan(void)					// 按键扫描函数，10ms扫描一次
{
	Key_Refresh();			//更新一次按键状态
	if(key_falling == B1)		// K1按下
	{    
		ch2_enable = !ch2_enable;			// 通道2状态翻转
		ch3_enable = 0;								// 操作通道2的时候，将通道3关闭
		Pwm_Enable();									// 更新一下当前的输出状态
		RX_COMMEND.cmd_flag = 0;			// 串口接收到的命令终止
	}                 
	if(key_falling == B2)		// K2按下
	{
		(CH2Duty < 100) ? (CH2Duty += 10) : (CH2Duty = 0);
		Pwm_Enable();									// 更新一下当前的输出状态
		Write_AT24C02(0x00,CH2Duty);	// 将当前的占空比的值存到EEPROM
		Delay_Ms(5);
	}               
	if(key_falling == B3)		// K3
	{                 
		ch3_enable = !ch3_enable;			// 通道3状态翻转
		ch2_enable = 0;								// 操作通道3的时候，将通道2关闭
		Pwm_Enable();									// 更新一下当前的输出状态
		RX_COMMEND.cmd_flag = 0;			// 串口接收到的命令终止
	}                 
	if(key_falling == B4)		// K4
	{
		(CH3Duty < 100) ? (CH3Duty += 10) : (CH3Duty = 0);
		Pwm_Enable();									// 更新一下当前的输出状态
		Write_AT24C02(0x01,CH3Duty);	// 将当前的占空比的值存到EEPROM
		Delay_Ms(5);
	}
	
// 本次赛题没有用到按键的长按
//	if(key_state == B1)				// 按键长按检测
//	{
//		
//		if(key_continue_cnt == 50)		// 按键按下，超过500ms，表示长按
//		{
//			
//		}
//		else
//		{
//			key_continue_cnt++;		// 按键按下的时常设置，每+1，则表示时常加10ms
//		}
//	}
//	
//	if(key_state == 0x00)			// 按键松开
//	{
//		key_continue_cnt = 0;		// 按键松开，则把按键按下的时常清零
//	}
}

//Main Body
int main(void)
{
//	uint8_t i;
	// 配置中断向量优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	// 配置滴答定时器
	SysTick_Config(SystemCoreClock/1000);
	
	// 初始化LED
	Led_Init();
	
	// 初始化定时器4，定时1ms
	Tim_Init();
	
	// 初始化串口2
	Uart_Init();
	
	// 初始化LCD
	STM3210B_LCD_Init();
	
	// 初始化I2C
	i2c_init();
	
	// 初始化按键
	Key_Init();
	
	// 初始化PWM输出
	TIM2_PWM_OCTOGGLE_Init();
	
	// 初始化PWM和UART的输出，使能UART，失能PWM
	Pwm_Enable();
	
	// 初始化RTC
	Rtc_Init();
	
	// 设置初始时间23：59：50
	Time_Adjust(23,59,50);
	
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	CH2Duty = Read_AT24C02(0x00);		// 将上一次设置为占空比的值读取出来
	CH3Duty = Read_AT24C02(0x01);		// 将上一次设置为占空比的值读取出来

	while(1)
	{	
		Time_Refresh();								// 刷新一下当前的时间
		
		// 显示通道2的占空比
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"  PWM-PA1: %d%%      ",CH2Duty);
		LCD_DisplayStringLine(Line0,lcd_str);	
		// 显示通道3的占空比
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"  PWM-PA2: %d%%      ",CH3Duty);
		LCD_DisplayStringLine(Line2,lcd_str);	
		// 显示当前的时间
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"  Time:%0.2d:%0.2d:%0.2d    ", THH, TMM, TSS);
		LCD_DisplayStringLine(Line4,lcd_str);	
		
		if(ch2_enable)				// 如果通道2打开，LCD显示通道2，并且打开LD1，关闭LD2
		{
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"  Channel: PA1     ");
			LCD_DisplayStringLine(Line6,lcd_str);	
			Led_Ctrl(LD1,ENABLE);
			Led_Ctrl(LD2,DISABLE);
		}
		else if(ch3_enable)		// 如果通道3打开，LCD显示通道3，并且打开LD2，关闭LD1
		{
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"  Channel: PA2     ");
			LCD_DisplayStringLine(Line6,lcd_str);	
			Led_Ctrl(LD2,ENABLE);
			Led_Ctrl(LD1,DISABLE);
		}
		else									//	如果通道2和通道3都关闭，则关闭LD1和LD2，LCD显示NONE
		{
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"  Channel: NONE    ");
			LCD_DisplayStringLine(Line6,lcd_str);
			Led_Ctrl(LD1,DISABLE);
			Led_Ctrl(LD2,DISABLE);
		}
		
		// LCD显示Command
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"  Command:         ");
		LCD_DisplayStringLine(Line8,lcd_str);	
		
		if(key_scan_flag)
		{
			key_scan_flag = 0;
			Key_Scan();
		}
		
		if(rx_flag)			// 如果串口接收到数据
		{
			rx_flag = 0;	// 将标志置位0
			// 进行命令中数据的转换
			RX_COMMEND.cmd_hh = (RxBuffer[0] - '0') * 10 + (RxBuffer[1] - '0');
			RX_COMMEND.cmd_mm = (RxBuffer[3] - '0') * 10 + (RxBuffer[4] - '0');
			RX_COMMEND.cmd_ss = (RxBuffer[6] - '0') * 10 + (RxBuffer[7] - '0');
			RX_COMMEND.x = RxBuffer[11] - '0';
			RX_COMMEND.y = RxBuffer[13] - '0';
			RX_COMMEND.cmd_flag = 0;
			RX_COMMEND.sec_cnt = 0;
			RX_COMMEND.cnt_flag = 0;
			RX_COMMEND.cmd_flag = 1;	// 接收到命令标志置1
			// lcd显示当前的命令
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"%20s",RxBuffer);
			LCD_DisplayStringLine(Line9,lcd_str);
			// 将接收字符串清空，为下一次接收串口数据做准备
			memset(RxBuffer,0,sizeof(RxBuffer));
		}
		
		if(RX_COMMEND.cmd_flag)	// 接收到命令
		{
			// 如果当前的时候等于接收到命令设定的时间
			if(THH == RX_COMMEND.cmd_hh && TMM == RX_COMMEND.cmd_mm && TSS == RX_COMMEND.cmd_ss)
			{	// 打开对应的通道，初始化对应的通道，并且开启定时器计时
				RX_COMMEND.x==1?(ch2_enable=1):(ch3_enable=1);
				Pwm_Enable();
				RX_COMMEND.cnt_flag = 1;
			}
		}
		else
		{	// 如果没有接收到命令，则输出LCD显示None
			LCD_DisplayStringLine(Line9,(unsigned char *)"            None    ");
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
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		if(++key_scan_cnt >= 10)	// 10ms进行一次按键的扫描
		{
			key_scan_cnt = 0;
			key_scan_flag = 1;
		}
		
		if(rx_ms_cnt != 123 && ++rx_ms_cnt >= 50)	// 串口接收50ms的数据
		{
			rx_ms_cnt = 123;	// 这里幅值为123，是一个不可能的值，为了防止重复进入
			rx_flag = 1;
			RxCounter = 0;
		}
		
		if(RX_COMMEND.cmd_flag && RX_COMMEND.cnt_flag && ++RX_COMMEND.sec_cnt >= RX_COMMEND.y * 1000)
		{	// 定时时间到
			RX_COMMEND.x==1?(ch2_enable=0):(ch3_enable=0);		// 打开对应通道
			RX_COMMEND.cmd_flag = 0;
			Pwm_Enable();
		}
  }
}
