#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "core_cm3.h"


//定义全局变量
unsigned long TimingDelay=0;//用于滴答定时器计数
unsigned char key_val=0;//用于读取键值

int main(void)
{
	LED_GPIO_Init();
	Key_Init();
	SysTick_Config(SystemCoreClock / 1000);
	
	while(1)
	{
		key_val = KEY_Scan();
//		if(key_val==1)	{LED_GPIO_Init(); LED_ON(LED_1);}//按下KEY1，所有灯熄灭，LED1亮
//		if(key_val==2)	{LED_GPIO_Init(); LED_ON(LED_2);}//按下KEY2，所有灯熄灭，LED2亮
//		if(key_val==3)	{LED_GPIO_Init(); LED_ON(LED_3);}//按下KEY3，所有灯熄灭，LED3亮
//		if(key_val==4)	{LED_GPIO_Init(); LED_ON(LED_4);}//按下KEY4，所有灯熄灭，LED4亮
		if(key_val==1)	{LED_TOGGLE(LED_1);}//按下KEY1，所有灯熄灭，LED1亮
		if(key_val==2)	{LED_TOGGLE(LED_2);}//按下KEY2，所有灯熄灭，LED2亮
		if(key_val==3)	{LED_TOGGLE(LED_3);}//按下KEY3，所有灯熄灭，LED3亮
		if(key_val==4)	{LED_TOGGLE(LED_4);}//按下KEY4，所有灯熄灭，LED4亮
	}
}

void SysTick_Handler(void)//滴答定时器函数
{
	TimingDelay++;//开始计时
	if(TimingDelay<100)//0~100ms
	{
		LED_ON(LED_8);//LED2亮
	}
	if(TimingDelay==200)//如果计数到200ms
	{
		TimingDelay=0;//又回到0，从0开始计时
	}
	if(TimingDelay>100)//100ms~200ms
	{
		LED_OFF(LED_8);;//LED2灭
	}
}
