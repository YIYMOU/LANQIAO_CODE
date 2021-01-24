// 用来存放STM32寄存器映射的代码

// 外设 peripheral

#define PERIPH_BASE							((unsigned int)0x40000000)
#define APB1PERIPH_BASE					PERIPH_BASE
#define APB2PERIPH_BASE					(PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE					(PERIPH_BASE + 0x20000)


#define RCC_BASE								(AHBPERIPH_BASE + 0x1000)
#define GPIOC_BASE							(APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE							(APB2PERIPH_BASE + 0x1400)

#define RCC_APB2ENR							*(unsigned int*)(RCC_BASE + 0x18)
	
#define GPIOC_CRL							*(unsigned int*)(GPIOC_BASE + 0x00)
#define GPIOC_CRH							*(unsigned int*)(GPIOC_BASE + 0x04)
#define GPIOC_ODR							*(unsigned int*)(GPIOC_BASE + 0x0C)
	
#define GPIOD_CRL							*(unsigned int*)(GPIOD_BASE + 0x00)
#define GPIOD_CRH							*(unsigned int*)(GPIOD_BASE + 0x04)
#define GPIOD_ODR							*(unsigned int*)(GPIOD_BASE + 0x0C)
