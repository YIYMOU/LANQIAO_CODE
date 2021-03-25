#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "tim.h"
#include "key.h"
#include "rtc.h"
#include "stdio.h"
#include "string.h"
#include "pwm.h"

u32 TimingDelay = 0;

uint8_t key_cnt = 0;
_Bool key_flag = 0;

uint16_t lcd_cnt = 0;
_Bool lcd_flag = 0;

uint16_t liushui_cnt = 0;
uint8_t liushui_x = 1;
_Bool liushui_flag = 0;

uint8_t lcd_str[20];

uint8_t aim_floor = 0;				// 用来存放还剩的目标层数，表示方法为(0x01 << x)表示第(x+1)层。
															// 如果值为0x3，也就是目标是第一层和第二层
														
uint8_t now_floor = 1;

_Bool key_pressed = 0;				// 1表示有按键被按下，0表示没有按键被按下

_Bool open_flag = 0;					// 1表示电梯运行，0表示电梯停止运行

_Bool delay_1ms_flag = 0;			
uint16_t delay_1ms_cnt = 0;

_Bool delay_6ms_flag = 0;
uint16_t delay_6ms_cnt = 0;

_Bool delay_4ms_flag = 0;
uint16_t delay_4ms_cnt = 0;

_Bool arrived_flag = 1;				// 1表示电梯到达了另一层

_Bool up_flag = 1;						// 1表示电梯当前的运行方向为向上，否则向下

_Bool lcd_on_flag = 0;				// 1表示开启lcd当前楼层数1s闪烁两次

_Bool opening_flag = 0;				// 1表示当前电梯门正在被打开
uint16_t opening_cnt = 0;			

_Bool closing_flag = 0;				// 1表示当前电梯门正在被关闭
uint16_t closing_cnt = 0;

void Delay_Ms(u32 nTime);

void gpio_init(void)	// 初始化PA4和PA5
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
	
void pwm_enable(uint16_t GPIO_Pin_x, FunctionalState NewState)		// 设置pwm通道是否输出波形
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(NewState == ENABLE)	// 电机打开，使能，PWM输出波形
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else	// 电机关闭，输出低电平表示电机关闭
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		GPIOA->ODR &= ~GPIO_Pin_x;
	}
}

void key_scan(void)
{
	key_refresh();		// 更新一下当前的按键状态
	
	// 因为电梯运转期间按键无效，所以这里需要判断电梯是否正在运行（也就是处于上升或者下降，开门或者关门的状态中）
	if(arrived_flag && closing_flag == 0 && opening_flag == 0 && falling_flag != 0x00)	
	{
		key_pressed = 1;
		delay_1ms_cnt = 0;
		aim_floor |= falling_flag;		// 当前按键就对应着楼层，这样就省去了很多的判断
	}
}

// Main Body
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	STM3210B_LCD_Init();	
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	led_init();												// 初始化led
	key_init();												// 初始化按键
	tim_init();												// 初始化定时器
	rtc_init();												// 初始化rtc
	Time_Adjust(12,50,55);						// 将初始化时间设定为12：50：55
	pwm_init();												// 初始化pwm
	gpio_init();											// 初始化PA4和PA5
	pwm_enable(GPIO_Pin_7 | GPIO_Pin_6,DISABLE);	// 初始化模拟电机为关闭，及pwm不输出
	GPIOA->ODR |= GPIO_Pin_5;											// 因为上电的时候，默认是开门，PA5输出高电平
	GPIOA->ODR |= GPIO_Pin_4;											// 因为上电的时候，默认是向上，PA4输出高电平
	
	while(1)
	{
		
		Time_Refresh();		// 刷新时间
		
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"      %02d:%02d:%02d      ",THH,TMM,TSS);
		LCD_DisplayStringLine(Line6 ,lcd_str);		// 将当前时间显示到屏幕上
		
		if(up_flag)		// 如果运行方向向上
		{
			GPIOA->ODR |= GPIO_Pin_4;		// PA4输出高电平
		}
		else
		{
			GPIOA->ODR &= ~GPIO_Pin_4;		// PA4输出低电平
		}
		
		if(closing_flag || opening_flag || (open_flag && arrived_flag == 0))		// 升降机处于关门状态：电梯在运行，或者处在开关门的状态
		{
			GPIOA->ODR &= ~GPIO_Pin_5;
		}
		else		// 升降机处于开门状态：电梯没有在运行，没有处在开关门的状态
		{
			GPIOA->ODR |= GPIO_Pin_5;
		}
		
		if(closing_flag)		// 电梯正在关门
		{
			pwm_enable(GPIO_Pin_7,ENABLE);		// 打开PA7 pwm输出
			CH2_Duty = 0.5;
		}
		else if(opening_flag)		// 电梯正在关门
		{
			pwm_enable(GPIO_Pin_7,ENABLE);		// 打开PA7 pwm输出
			CH2_Duty = 0.6;		// 占空比设置为0.6
		}
		else
		{
			pwm_enable(GPIO_Pin_7,DISABLE);		// 电梯不在开关门的状态，失能pwm，输出低电平
		}
		
		if(open_flag && arrived_flag == 0 && opening_flag == 0 && closing_flag == 0)
		{		// 电梯处在上升或者下降的状态：电梯正在运行，电梯没有到达，没有处在开关门的状态
			if(up_flag)
				CH1_Duty = 0.8;		// 电梯向上，占空比设置为0.8
			else
				CH1_Duty = 0.6;		// 电梯下降，占空比设置为0.6
			pwm_enable(GPIO_Pin_6,ENABLE);	// 打开PA6 pwm输出
		}
		else
		{
			pwm_enable(GPIO_Pin_6,DISABLE);	// 关闭PA6 pwm输出，输出低电平
		}
		
		if(!lcd_flag)		// lcd显示楼层，；lcd_flag==1时显示，用于实现楼层号的闪烁
		{
			switch(now_floor)
			{
				case 0x01: LCD_DisplayStringLine(Line4 ,(uint8_t *)"         1         "); break;
				case 0x02: LCD_DisplayStringLine(Line4 ,(uint8_t *)"         2         "); break;
				case 0x04: LCD_DisplayStringLine(Line4 ,(uint8_t *)"         3         "); break;
				case 0x08: LCD_DisplayStringLine(Line4 ,(uint8_t *)"         4         "); break;
				default : break;
			}
		}
		else			// lcd_flag==0时不显示，用于实现楼层号的闪烁
		{
			LCD_DisplayStringLine(Line4 ,(uint8_t *)"                   ");
		}
		
		
		if(key_flag)		// 按键扫描
		{
			key_flag = 0;
			key_scan();
		}
		
		if(now_floor & aim_floor)		// 表示当前楼层到达目标楼层
		{
			led_ctrl(now_floor << 8,DISABLE);		// 将表示当前楼层的led熄灭
			aim_floor &= ~now_floor;	// 从目标楼层中去掉当前楼层
			if(aim_floor == 0x00)			// 如果没有剩余的目标楼层了，则将电梯关闭
			{
				open_flag = 0;
			}
		}
		
		if(aim_floor)		// 显示当前表示电梯剩余的楼层的led
		{
			led_ctrl(aim_floor << 8,ENABLE);
		}
		
		if(!arrived_flag && liushui_flag)		// 流水灯打开，表示当前电梯正在运行
		{
			liushui_flag = 0;
			led_ctrl((uint16_t)(liushui_x << 12),DISABLE);
			if(!up_flag)
			{
				liushui_x = liushui_x >> 1;
				if(liushui_x == 0x00)
					liushui_x = 0x08;
			}
			else
			{
				liushui_x = liushui_x << 1;
				if(liushui_x == 0x10)
					liushui_x = 0x01;
			}
			led_ctrl((uint16_t)(liushui_x << 12),ENABLE);
		}
		
		
		if(arrived_flag == 1 || closing_flag || opening_flag)		// 如果当前电梯没有在上升或者下降的状态，则关闭流水灯
		{
			led_ctrl(LD5 | LD6 | LD7 | LD8,DISABLE);
			liushui_x = 1;
			liushui_flag = 0;
			liushui_cnt = 0;
		}
	}
}


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
		if(++key_cnt >= 10)		// 10ms产生一次中断
		{
			key_cnt = 0;
			key_flag = 1;
		}
		if(++liushui_cnt >= 500)	// 流水灯500ms跑一次
		{
			liushui_cnt = 0;
			liushui_flag = 1;
		}
		
		if(key_pressed && ++delay_1ms_cnt >= 1000)	// 按键有效，按键按完1s后，开始运行
		{
			delay_1ms_cnt = 0;
			key_pressed = 0;
			arrived_flag = 0;
			delay_4ms_cnt = 0;
			closing_flag = 1;
			
			if(aim_floor > now_floor)		// 如果当前设置的目标楼层就是当前的楼层，那么按键无效
			{
				up_flag = 1;
				open_flag = 1;
			}
			else // 按键有效，电梯开始运行
			{
				up_flag = 0;
				open_flag = 1;
			}
		}
		
		if(open_flag && arrived_flag == 0 && opening_flag == 0 && closing_flag == 0 &&  ++delay_6ms_cnt >= 6000)
		{		// 电梯处于升降状态
			delay_6ms_cnt = 0;
			arrived_flag = 1;
			opening_flag = 1;
			lcd_on_flag = 1;
			
			if(up_flag)
				now_floor <<= 1;
			else
				now_floor >>= 1;
			
			if(up_flag && now_floor == 0x08)		// 边界判定
			{
				up_flag = 0;
			}
			else if(up_flag == 0 && now_floor == 0x01)		// 边界判定
			{
				up_flag = 1;
			}
			
		}
		
		if(lcd_on_flag && ++lcd_cnt == 250)		// 升降机运行到目标平台时，平台数字在1秒内闪烁2次
		{
			lcd_flag = 1;
		}
		else if(lcd_on_flag && lcd_cnt == 500)
		{
			lcd_flag = 0;
		}
		else if(lcd_on_flag && lcd_cnt == 750)
		{
			lcd_flag = 1;
		}
		else if(lcd_on_flag && lcd_cnt == 1000)
		{
			lcd_flag = 0;
			lcd_on_flag = 0;
			lcd_cnt = 0;
		}
		
		if(opening_flag && ++opening_cnt >= 1000)	// 电梯处于开门状态，1s
		{
			opening_flag = 0;
			opening_cnt = 0;
		}
		
		if(closing_flag && ++closing_cnt >= 1000)	// 电梯处于关门状态，1s
		{
			closing_flag = 0;
			closing_cnt = 0;
		}
		
		if(open_flag && arrived_flag && opening_flag == 0 && key_pressed == 0 && ++delay_4ms_cnt >= 2000)
		{	// 电梯处于等待状态
			delay_4ms_cnt = 0;
			arrived_flag = 0;
			closing_flag = 1;
		}
  }
}
