#include "bsp_key.h"

void Key_Init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef Key_InitStucture;
	
	/*开启KEY相关的GPIO外设时钟*/
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK | KEY3_GPIO_CLK,ENABLE);
	
	/*选择要控制的GPIO引脚*/
	Key_InitStucture.GPIO_Pin = KEY1_GPIO_PIN | KEY2_GPIO_PIN;
	Key_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
	/*由于开发板接了上拉电阻，设置引脚模式为浮空输入*/
	Key_InitStucture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	/*调用库函数，初始化GPIO*/
	GPIO_Init(KEY1_GPIO_PORT,&Key_InitStucture);
	
	/*选择要控制的GPIO引脚*/
	Key_InitStucture.GPIO_Pin = KEY3_GPIO_PIN | KEY4_GPIO_PIN;
	Key_InitStucture.GPIO_Speed = GPIO_Speed_50MHz;
	/*由于开发板接了上拉电阻，设置引脚模式为浮空输入*/
	Key_InitStucture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	/*调用库函数，初始化GPIO*/
	GPIO_Init(KEY3_GPIO_PORT,&Key_InitStucture);
	
}
void Delay_KEY(unsigned int ms)
{
	unsigned int i, j;
	for(i=0; i<ms; i++)
		for(j=0; j<7992; j++); // SYSCLK = 72MHz
		// for(j=0; j<1598; j++); // SYSCLK = 8MHz
}

//按键函数
//返回值为对应的键值，按下KEY1，reture 1；按下KEY2，reture 2；以此类推。
unsigned char KEY_Scan(void)
{	
	unsigned char key_val=0;//定义局部变量，存放键值
	if(((key1==0)||(key2==0)||(key3==0)||(key4==0))==1)//如果有一个被按下
	{
		Delay_KEY(10);
		if(((key1==0)||(key2==0)||(key3==0)||(key4==0))==1)//如果有一个被按下
		{
			if(key1==0)key_val=1;//按下KEY1，key_val=1
			else if(key2==0)key_val=2;//按下KEY1，key_val=2
			else if(key3==0)key_val=3;//按下KEY1，key_val=3
			else if(key4==0)key_val=4;//按下KEY1，key_val=4
		}
	}
	return key_val;//返回key_val(键值)
}

