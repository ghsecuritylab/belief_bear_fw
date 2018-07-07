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

/*
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
*/

static void rt_hw_gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
	//�ر�JTAG���ܣ�ʹ��GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);//ʹ��PB,PE�˿�ʱ��
        
    GPIO_InitStructure.GPIO_Pin = DRYING_OUT_pin | DISINFECTION_OUT_pin | AROMATHERAPY_OUT_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);	

    GPIO_InitStructure.GPIO_Pin = LED_LIGHT_OUT_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(GPIOC, &GPIO_InitStructure);		
	
    //led output indicate                    
    GPIO_InitStructure.GPIO_Pin = LED_DRY_pin | LED_ARO_pin | LED_DIS_pin | LED_POWER_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = LED_INC_pin | LED_DEC_pin | LED_RESERVE_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
	  
	//buzzer indication
	GPIO_InitStructure.GPIO_Pin = BUZZER_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
    GPIO_Init(BUZZER_gpio, &GPIO_InitStructure);	

    //magnetic input io
    GPIO_InitStructure.GPIO_Pin  = MAGNETIC_IN_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //������������
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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   //���ó���������	
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
    
    GPIO_InitStructure.GPIO_Pin = INCREASE_IN_pin | DECREASE_IN_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   //���ó���������	
    GPIO_Init(GPIOB, &GPIO_InitStructure);	
    
    GPIO_InitStructure.GPIO_Pin = POWER_IN_pin;				 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 				
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

void EXTIX_Init(void) {
    EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
    
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

// //GPIOB.2	  �ж����Լ��жϳ�ʼ������ �½��ش��� 
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource2);
//  	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
//  	EXTI_Init(&EXTI_InitStructure);	  	

 //GPIOB.0	  �ж����Լ��жϳ�ʼ������  �½��ش���	
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

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ�ܰ���magnetic���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�1 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

}

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim  TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx					 
}

void rt_led_indication_default(void) {
	LED_DRY_OFF();
	LED_ARO_OFF();
	LED_DIS_OFF();
	LED_RESERVE_OFF();
	LED_INC_OFF();
	LED_DEC_OFF();
	LED_POWER_OFF();
	BUZZER_OFF();
}

//����ָʾ
void rt_led_buzzer(u8 bit) {
	switch (bit) {
		case 1: LED_DRY_ON(); break; 
		case 2: LED_DIS_ON(); break;
		case 3: LED_ARO_ON(); break;
		case 4: LED_RESERVE_ON(); break;
		case 5: LED_INC_ON(); break;
		case 6: LED_DEC_ON(); break;
		case 7: LED_POWER_ON(); break;
		default: break;
	}
	BUZZER_ON();
    rt_enter_critical();
	rt_thread_delay(20*RT_TICK_PER_SECOND/1000);
    rt_exit_critical();
	switch (bit) {
		case 1: LED_DRY_OFF(); break; 
		case 2: LED_DIS_OFF(); break;
		case 3: LED_ARO_OFF(); break;
		case 4: LED_RESERVE_OFF(); break;
		case 5: LED_INC_OFF(); break;
		case 6: LED_DEC_OFF(); break;
		case 7: LED_POWER_OFF(); break;
		default: break;
	}
	BUZZER_OFF();
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
 //   TIM3_Int_Init(4999,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  
//    EXTIX_Init();
 
    Display_Init();

    rtc_init();
	rt_led_indication_default();
        
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
