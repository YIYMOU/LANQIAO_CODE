/************************************************
*���ű�Ƕ��ʽ�������Ľ�ʡ����-˫·���������
*CSDN��https://blog.csdn.net/qq_43715171
*���ƣ�https://gitee.com/yiymou
*GitHub��https://github.com/YIYMOU
*Ӳ��ƽ̨�����ų���CT117E
*���ߣ�yiymou   2021-02-04
************************************************/
#include "stm32f10x.h"
#include "lcd.h"						// LCDͷ�ļ�
#include "led.h"						// LEDͷ�ļ�	
#include "tim.h"
#include "uart.h"
#include "i2c.h"
#include "stdio.h"					// ʹ����sprintf����
#include "string.h"					// ʹ����memset����
#include "key.h"
#include "pwm_octoggle.h"
#include "rtc.h"

u32 TimingDelay = 0;				// ��ʱ�����ı���

uint16_t key_scan_cnt = 0;			// ������������10ms���һ��
// uint16_t key_continue_cnt = 0;	// ����ʱ�����������������û���õ������ĳ���
_Bool key_scan_flag = 0;				// ����ɨ���־

_Bool rx_flag = 0;			// ���յ��������ݵı���

_Bool ch2_enable = 0;		// ͨ����ʹ������ı�־
_Bool ch3_enable = 0;		// ͨ����ʹ������ı�־

uint8_t lcd_str[20];		// ר��������LCD����ʾ���ַ���

union EEPROM						// ������eeprom��д���ݵĹ�����
{
	uint8_t uint8;
	uint16_t uint16;
	uint32_t uint32;
	uint8_t str[20];
} Write_EEPROM,Read_EEPROM;

struct COMMEND					// ��Ŵ��ڽ��յ��������ת��������ݵĽṹ��
{
	uint8_t x;						// PAxͨ��
	uint8_t y;						// ������� y �루0<y<10��
	uint32_t sec_cnt;			// ��ʱ�������ı���
	uint8_t cnt_flag;			// ������ʱ�ı�־
	uint32_t cmd_hh;			// hhʱ 
	uint32_t cmd_mm;			// mm��
	uint32_t cmd_ss;			// ss��
	_Bool cmd_flag;
} RX_COMMEND = {0,0,0,0,0,0,0,0};

void Delay_Ms(u32 nTime);		// ��ʱ����

void PAx_Output(uint16_t GPIO_Pin_x,FunctionalState NewState)	// ����������Ϊ���������ֻ������ߵ͵�ƽ
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

void PAx_Output_PWM(uint16_t GPIO_Pin_x)	// ����������Ϊģ��������������PWM�ź�
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_x;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Pwm_Enable(void)	// ����PWM�������PWM��ʱ�򣬹رմ��ڣ������PWMʱ���򿪴���
{
	// ͨ��2
	if(!ch2_enable || CH2Duty == 0)					// �ر�ͨ��������ռ�ձ�Ϊ0������͵�ƽ
	{
		PAx_Output(GPIO_Pin_1,DISABLE);
	}
	else if(ch2_enable && CH2Duty == 100)		// ����ͨ������ռ�ձ�Ϊ100%������ߵ�ƽ
	{
		PAx_Output(GPIO_Pin_1,ENABLE);
	}
	else	// ����������������PWM��
	{
		PAx_Output_PWM(GPIO_Pin_1);
	}
	
	// ͨ��3
	if(ch3_enable && CH3Duty == 100)					// �ر�ͨ��������ռ�ձ�Ϊ0������͵�ƽ
	{
		PAx_Output(GPIO_Pin_2,ENABLE);
	}
	else if(!ch3_enable || CH3Duty == 0)		// ����ͨ������ռ�ձ�Ϊ100%������ߵ�ƽ
	{
		PAx_Output(GPIO_Pin_2,DISABLE);
	}
	else	// ����������������PWM��
	{
		PAx_Output_PWM(GPIO_Pin_2);
	}
	
	if(ch2_enable == 0 && ch3_enable == 0)	// ͨ��2��ͨ��3���չص�ʱ�򣬹رն�ʱ��2���򿪴���
	{	// һ��Ҫ�ȹض�ʱ�����ٹر�ʱ��
		/* TIM2 diable counter */
		TIM_Cmd(TIM2, DISABLE);
		/* TIM2 clock diable */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);
		
		/* Enable USART2 Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		/* Enable the USART2 */
		USART_Cmd(USART2, ENABLE);
	}
	else		// ��ͨ��������ʱ�򣬹رմ��ڣ��򿪶�ʱ��
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

void Key_Scan(void)					// ����ɨ�躯����10msɨ��һ��
{
	Key_Refresh();			//����һ�ΰ���״̬
	if(key_falling == B1)		// K1����
	{    
		ch2_enable = !ch2_enable;			// ͨ��2״̬��ת
		ch3_enable = 0;								// ����ͨ��2��ʱ�򣬽�ͨ��3�ر�
		Pwm_Enable();									// ����һ�µ�ǰ�����״̬
		RX_COMMEND.cmd_flag = 0;			// ���ڽ��յ���������ֹ
	}                 
	if(key_falling == B2)		// K2����
	{
		(CH2Duty < 100) ? (CH2Duty += 10) : (CH2Duty = 0);
		Pwm_Enable();									// ����һ�µ�ǰ�����״̬
		Write_AT24C02(0x00,CH2Duty);	// ����ǰ��ռ�ձȵ�ֵ�浽EEPROM
		Delay_Ms(5);
	}               
	if(key_falling == B3)		// K3
	{                 
		ch3_enable = !ch3_enable;			// ͨ��3״̬��ת
		ch2_enable = 0;								// ����ͨ��3��ʱ�򣬽�ͨ��2�ر�
		Pwm_Enable();									// ����һ�µ�ǰ�����״̬
		RX_COMMEND.cmd_flag = 0;			// ���ڽ��յ���������ֹ
	}                 
	if(key_falling == B4)		// K4
	{
		(CH3Duty < 100) ? (CH3Duty += 10) : (CH3Duty = 0);
		Pwm_Enable();									// ����һ�µ�ǰ�����״̬
		Write_AT24C02(0x01,CH3Duty);	// ����ǰ��ռ�ձȵ�ֵ�浽EEPROM
		Delay_Ms(5);
	}
	
// ��������û���õ������ĳ���
//	if(key_state == B1)				// �����������
//	{
//		
//		if(key_continue_cnt == 50)		// �������£�����500ms����ʾ����
//		{
//			
//		}
//		else
//		{
//			key_continue_cnt++;		// �������µ�ʱ�����ã�ÿ+1�����ʾʱ����10ms
//		}
//	}
//	
//	if(key_state == 0x00)			// �����ɿ�
//	{
//		key_continue_cnt = 0;		// �����ɿ�����Ѱ������µ�ʱ������
//	}
}

//Main Body
int main(void)
{
//	uint8_t i;
	// �����ж��������ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	// ���õδ�ʱ��
	SysTick_Config(SystemCoreClock/1000);
	
	// ��ʼ��LED
	Led_Init();
	
	// ��ʼ����ʱ��4����ʱ1ms
	Tim_Init();
	
	// ��ʼ������2
	Uart_Init();
	
	// ��ʼ��LCD
	STM3210B_LCD_Init();
	
	// ��ʼ��I2C
	i2c_init();
	
	// ��ʼ������
	Key_Init();
	
	// ��ʼ��PWM���
	TIM2_PWM_OCTOGGLE_Init();
	
	// ��ʼ��PWM��UART�������ʹ��UART��ʧ��PWM
	Pwm_Enable();
	
	// ��ʼ��RTC
	Rtc_Init();
	
	// ���ó�ʼʱ��23��59��50
	Time_Adjust(23,59,50);
	
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	CH2Duty = Read_AT24C02(0x00);		// ����һ������Ϊռ�ձȵ�ֵ��ȡ����
	CH3Duty = Read_AT24C02(0x01);		// ����һ������Ϊռ�ձȵ�ֵ��ȡ����

	while(1)
	{	
		Time_Refresh();								// ˢ��һ�µ�ǰ��ʱ��
		
		// ��ʾͨ��2��ռ�ձ�
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"  PWM-PA1: %d%%      ",CH2Duty);
		LCD_DisplayStringLine(Line0,lcd_str);	
		// ��ʾͨ��3��ռ�ձ�
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"  PWM-PA2: %d%%      ",CH3Duty);
		LCD_DisplayStringLine(Line2,lcd_str);	
		// ��ʾ��ǰ��ʱ��
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"  Time:%0.2d:%0.2d:%0.2d    ", THH, TMM, TSS);
		LCD_DisplayStringLine(Line4,lcd_str);	
		
		if(ch2_enable)				// ���ͨ��2�򿪣�LCD��ʾͨ��2�����Ҵ�LD1���ر�LD2
		{
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"  Channel: PA1     ");
			LCD_DisplayStringLine(Line6,lcd_str);	
			Led_Ctrl(LD1,ENABLE);
			Led_Ctrl(LD2,DISABLE);
		}
		else if(ch3_enable)		// ���ͨ��3�򿪣�LCD��ʾͨ��3�����Ҵ�LD2���ر�LD1
		{
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"  Channel: PA2     ");
			LCD_DisplayStringLine(Line6,lcd_str);	
			Led_Ctrl(LD2,ENABLE);
			Led_Ctrl(LD1,DISABLE);
		}
		else									//	���ͨ��2��ͨ��3���رգ���ر�LD1��LD2��LCD��ʾNONE
		{
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"  Channel: NONE    ");
			LCD_DisplayStringLine(Line6,lcd_str);
			Led_Ctrl(LD1,DISABLE);
			Led_Ctrl(LD2,DISABLE);
		}
		
		// LCD��ʾCommand
		memset(lcd_str,0,sizeof(lcd_str));
		sprintf((char *)lcd_str,"  Command:         ");
		LCD_DisplayStringLine(Line8,lcd_str);	
		
		if(key_scan_flag)
		{
			key_scan_flag = 0;
			Key_Scan();
		}
		
		if(rx_flag)			// ������ڽ��յ�����
		{
			rx_flag = 0;	// ����־��λ0
			// �������������ݵ�ת��
			RX_COMMEND.cmd_hh = (RxBuffer[0] - '0') * 10 + (RxBuffer[1] - '0');
			RX_COMMEND.cmd_mm = (RxBuffer[3] - '0') * 10 + (RxBuffer[4] - '0');
			RX_COMMEND.cmd_ss = (RxBuffer[6] - '0') * 10 + (RxBuffer[7] - '0');
			RX_COMMEND.x = RxBuffer[11] - '0';
			RX_COMMEND.y = RxBuffer[13] - '0';
			RX_COMMEND.cmd_flag = 0;
			RX_COMMEND.sec_cnt = 0;
			RX_COMMEND.cnt_flag = 0;
			RX_COMMEND.cmd_flag = 1;	// ���յ������־��1
			// lcd��ʾ��ǰ������
			memset(lcd_str,0,sizeof(lcd_str));
			sprintf((char *)lcd_str,"%20s",RxBuffer);
			LCD_DisplayStringLine(Line9,lcd_str);
			// �������ַ�����գ�Ϊ��һ�ν��մ���������׼��
			memset(RxBuffer,0,sizeof(RxBuffer));
		}
		
		if(RX_COMMEND.cmd_flag)	// ���յ�����
		{
			// �����ǰ��ʱ����ڽ��յ������趨��ʱ��
			if(THH == RX_COMMEND.cmd_hh && TMM == RX_COMMEND.cmd_mm && TSS == RX_COMMEND.cmd_ss)
			{	// �򿪶�Ӧ��ͨ������ʼ����Ӧ��ͨ�������ҿ�����ʱ����ʱ
				RX_COMMEND.x==1?(ch2_enable=1):(ch3_enable=1);
				Pwm_Enable();
				RX_COMMEND.cnt_flag = 1;
			}
		}
		else
		{	// ���û�н��յ���������LCD��ʾNone
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
		
		if(++key_scan_cnt >= 10)	// 10ms����һ�ΰ�����ɨ��
		{
			key_scan_cnt = 0;
			key_scan_flag = 1;
		}
		
		if(rx_ms_cnt != 123 && ++rx_ms_cnt >= 50)	// ���ڽ���50ms������
		{
			rx_ms_cnt = 123;	// �����ֵΪ123����һ�������ܵ�ֵ��Ϊ�˷�ֹ�ظ�����
			rx_flag = 1;
			RxCounter = 0;
		}
		
		if(RX_COMMEND.cmd_flag && RX_COMMEND.cnt_flag && ++RX_COMMEND.sec_cnt >= RX_COMMEND.y * 1000)
		{	// ��ʱʱ�䵽
			RX_COMMEND.x==1?(ch2_enable=0):(ch3_enable=0);		// �򿪶�Ӧͨ��
			RX_COMMEND.cmd_flag = 0;
			Pwm_Enable();
		}
  }
}
