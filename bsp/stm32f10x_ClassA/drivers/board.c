/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 * 2013-07-12     aozima       update for auto initial.
 */

#include <rthw.h>
#include <rtthread.h>
#include <stm32f10x.h>
#include "stm32f10x_fsmc.h"
#include "board.h"
#include "usart.h"
#include "sysconfig.h"
#include "rtc.h"
#include "Display.h"
#include "work.h"
#include "sys.h"

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

const volatile uint32_t g_version __attribute__ ((at(0x08004008))) = 0x00000901;     /* v0.9.1 */

rt_uint8_t sys_restart = 0;
/**
 * @addtogroup STM32
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0000);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
//    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

#define RCC_APB2Periph_ALL      (RCC_APB2Periph_SYSCFG | RCC_APB2Periph_TIM9    \
                                 | RCC_APB2Periph_TIM10 | RCC_APB2Periph_TIM11  \
                                 | RCC_APB2Periph_ADC1 | RCC_APB2Periph_SDIO    \
                                 | RCC_APB2Periph_SPI1 | RCC_APB2Periph_USART1)
#define RCC_APB1Periph_ALL      (RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3      \
                                 | RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5    \
                                 | RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM7    \
                                 | RCC_APB1Periph_LCD | RCC_APB1Periph_WWDG     \
                                 | RCC_APB1Periph_SPI2 | RCC_APB1Periph_SPI3    \
                                 | RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3 \
                                 | RCC_APB1Periph_UART4 | RCC_APB1Periph_UART5  \
                                 | RCC_APB1Periph_I2C1 | RCC_APB1Periph_I2C2    \
                                 | RCC_APB1Periph_USB | RCC_APB1Periph_PWR      \
                                 | RCC_APB1Periph_DAC | RCC_APB1Periph_COMP)
#define RCC_AHBPeriph_ALL		(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB      \
                                 | RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD    \
                                 | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH    \
                                 | RCC_AHBPeriph_GPIOF | RCC_AHBPeriph_GPIOG    \
                                 | RCC_AHBPeriph_CRC | RCC_AHBPeriph_FLITF      \
                                 | RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2      \
                                 | RCC_AHBPeriph_AES | RCC_AHBPeriph_FSMC)


//HSE ~ 8*9=72MHZ
void SYSCLK_Config(void){
    
    RCC_DeInit();  //default RCC
    RCC_HSEConfig(RCC_HSE_ON);
    while(RCC_WaitForHSEStartUp() != SUCCESS);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); //8M * 9 =72M
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|
                            RCC_APB2Periph_GPIOC, ENABLE); 
}


void SYSCLKConfig_STOP(void)
{  
	/* After wake-up from STOP reconfigure the system clock */
	RCC_HSICmd(ENABLE); 
	while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	while (RCC_GetSYSCLKSource() != 0x04);
}

static void rt_hw_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
 	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);//使能PB,PE端口时钟
        
    GPIO_InitStructure.GPIO_Pin = DRYING_OUT_pin | DISINFECTION_OUT_pin | AROMATHERAPY_OUT_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 
    //led                    
    GPIO_InitStructure.GPIO_Pin = LED_OUT_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(LED_OUT_gpio, &GPIO_InitStructure);		
    
    //interrupt io
    GPIO_InitStructure.GPIO_Pin  = MAGNETIC_IN_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置上拉输入
    GPIO_Init(MAGNETIC_IN_gpio, &GPIO_InitStructure);		
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //tft
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);		

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_15;				 //tft
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);		
    
    //key scan
    GPIO_InitStructure.GPIO_Pin = DRYING_IN_pin | DISINFECTION_IN_pin | AROMATHERAPY_IN_pin | RESERVE_IN_pin ;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   //设置成下拉输入	
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
    
    GPIO_InitStructure.GPIO_Pin = INCREASE_IN_pin | DECREASE_IN_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   //设置成下拉输入	
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
    
    GPIO_InitStructure.GPIO_Pin = POWER_IN_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 				
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

void EXTIX_Init(void) {
    EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
    
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

// //GPIOB.2	  中断线以及中断初始化配置 下降沿触发 
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource2);
//  	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
//  	EXTI_Init(&EXTI_InitStructure);	  	

 //GPIOB.0	  中断线以及中断初始化配置  下降沿触发	
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  


//  	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								
//  	NVIC_Init(&NVIC_InitStructure); 

//    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;				
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					
//  	NVIC_Init(&NVIC_InitStructure);

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键magnetic所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

}

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim  TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}

void rt_hw_enable_watchdog(void) {
    // Time = (64/(32*10^3))*625 = 1.25s
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_64);
    IWDG_SetReload(1250);  //625
    IWDG_ReloadCounter();
    IWDG_Enable();
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init(void)
{
    /* NVIC Configuration */
    NVIC_Configuration();
    
    /* Configure the SysTick */
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    
    rt_hw_gpio_init();
 //   TIM3_Int_Init(4999,7199);//10Khz的计数频率，计数到5000为500ms  
//    EXTIX_Init();
    
#if (DEV_TYPE == TYPE_C)    
    Display_Init();
#endif
    rtc_init();
    
    sys_restart = 1;
        
//#ifdef RT_USING_SERIAL
//    rt_hw_usart_init();
//#endif
//    
//#ifdef RT_USING_CONSOLE
//    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
//#endif
//    
//#ifdef RT_USING_COMPONENTS_INIT
//    rt_components_board_init();
//#endif
}

/*@}*/
