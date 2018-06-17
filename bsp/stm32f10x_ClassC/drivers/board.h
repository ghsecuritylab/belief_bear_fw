/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-09-22     Bernard      add board.h to this bsp
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __BOARD_H__
#define __BOARD_H__

#include <stm32f10x.h>

/* board configuration */

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
// 	<i>Enable External SRAM memory
#define STM32_EXT_SRAM          0
//	<o>Begin Address of External SRAM
//		<i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//	<o>End Address of External SRAM
//		<i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>

// <o> Internal SRAM memory size[Kbytes] <8-64>
//	<i>Default: 64
#define STM32_SRAM_SIZE         16
#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)

/* USART driver select. */
#define RT_USING_UART1
//#define RT_USING_UART2
//#define RT_USING_UART3

///////////////////////////// common definition ///////////////////////////////

//scan key
#define DRYING_IN_rcc                       RCC_APB2Periph_GPIOB
#define DRYING_IN_gpio                      GPIOB
#define DRYING_IN_pin                       (GPIO_Pin_0)

#define AROMATHERAPY_IN_rcc                 RCC_APB2Periph_GPIOB
#define AROMATHERAPY_IN_gpio                GPIOB
#define AROMATHERAPY_IN_pin                 (GPIO_Pin_2)

#define DISINFECTION_IN_rcc                 RCC_APB2Periph_GPIOB
#define DISINFECTION_IN_gpio                GPIOB
#define DISINFECTION_IN_pin                 (GPIO_Pin_1)

#define RESERVE_IN_rcc                      RCC_APB2Periph_GPIOB
#define RESERVE_IN_gpio                     GPIOB
#define RESERVE_IN_pin                      (GPIO_Pin_6)

#define POWER_IN_rcc                        RCC_APB2Periph_GPIOA
#define POWER_IN_gpio                       GPIOA
#define POWER_IN_pin                        (GPIO_Pin_7)

//interrupt input
#define INCREASE_IN_rcc                     RCC_APB2Periph_GPIOB
#define INCREASE_IN_gpio                    GPIOB
#define INCREASE_IN_pin                     (GPIO_Pin_8)

#define DECREASE_IN_rcc                     RCC_APB2Periph_GPIOB
#define DECREASE_IN_gpio                    GPIOB
#define DECREASE_IN_pin                     (GPIO_Pin_7)

#define MAGNETIC_IN_rcc                     RCC_APB2Periph_GPIOB
#define MAGNETIC_IN_gpio                    GPIOB
#define MAGNETIC_IN_pin                     (GPIO_Pin_9)

//control relay output
#define DRYING_OUT_rcc                      RCC_APB2Periph_GPIOA
#define DRYING_OUT_gpio                     GPIOA
#define DRYING_OUT_pin                      (GPIO_Pin_2)

#define AROMATHERAPY_OUT_rcc                RCC_APB2Periph_GPIOA
#define AROMATHERAPY_OUT_gpio               GPIOA
#define AROMATHERAPY_OUT_pin                (GPIO_Pin_1)

#define DISINFECTION_OUT_rcc                RCC_APB2Periph_GPIOA
#define DISINFECTION_OUT_gpio               GPIOA
#define DISINFECTION_OUT_pin                (GPIO_Pin_0)

#define LED_LIGHT_OUT_rcc					RCC_APB2Periph_GPIOC
#define LED_LIGHT_OUT_gpio					GPIOC
#define LED_LIGHT_OUT_pin					(GPIO_Pin_13)

//led指示按键按下
#define LED_DRY_rcc							RCC_APB2Periph_GPIOA
#define LED_DRY_gpio						GPIOA
#define LED_DRY_pin							(GPIO_Pin_4)

#define LED_ARO_rcc							RCC_APB2Periph_GPIOA
#define LED_ARO_gpio						GPIOA
#define LED_ARO_pin							(GPIO_Pin_6)

#define LED_DIS_rcc							RCC_APB2Periph_GPIOA
#define LED_DIS_gpio						GPIOA
#define LED_DIS_pin							(GPIO_Pin_5)

#define LED_RESERVE_rcc						RCC_APB2Periph_GPIOB
#define LED_RESERVE_gpio					GPIOB
#define LED_RESERVE_pin						(GPIO_Pin_3)

#define LED_INC_rcc							RCC_APB2Periph_GPIOB
#define LED_INC_gpio						GPIOB
#define LED_INC_pin							(GPIO_Pin_5)

#define LED_DEC_rcc							RCC_APB2Periph_GPIOB
#define LED_DEC_gpio						GPIOB
#define LED_DEC_pin							(GPIO_Pin_4)

#define LED_POWER_rcc						RCC_APB2Periph_GPIOA
#define LED_POWER_gpio						GPIOA
#define LED_POWER_pin						(GPIO_Pin_3)

//蜂鸣器提示
#define BUZZER_gpio							GPIOA
#define BUZZER_pin							(GPIO_Pin_11)
//////////////////////////////////////////////////////////////////////////////

//循环扫描，默认为低，高脉冲有效
#define drying_input        GPIO_ReadInputDataBit(DRYING_IN_gpio,DRYING_IN_pin)
#define aromatherapy_input  GPIO_ReadInputDataBit(AROMATHERAPY_IN_gpio,AROMATHERAPY_IN_pin)
#define disinfection_input  GPIO_ReadInputDataBit(DISINFECTION_IN_gpio,DISINFECTION_IN_pin)
#define reserve_input       GPIO_ReadInputDataBit(RESERVE_IN_gpio,RESERVE_IN_pin)
#define power_input         GPIO_ReadInputDataBit(POWER_IN_gpio,POWER_IN_pin)
#define increase_input  GPIO_ReadInputDataBit(INCREASE_IN_gpio,INCREASE_IN_pin)
#define decrease_input  GPIO_ReadInputDataBit(DECREASE_IN_gpio,DECREASE_IN_pin)
//默认下拉，低有效
#define magnetic_input  GPIO_ReadInputDataBit(MAGNETIC_IN_gpio,MAGNETIC_IN_pin)

//烘干
static inline void DRYING_ON(void)  {
    GPIO_SetBits(DRYING_OUT_gpio, DRYING_OUT_pin);
}
static inline void DRYING_OFF(void) {
    GPIO_ResetBits(DRYING_OUT_gpio,DRYING_OUT_pin);
}
//香薰
static inline void AROMATHERAPY_ON(void)  {
    GPIO_SetBits(AROMATHERAPY_OUT_gpio, AROMATHERAPY_OUT_pin);
}
static inline void AROMATHERAPY_OFF(void) {
    GPIO_ResetBits(AROMATHERAPY_OUT_gpio,AROMATHERAPY_OUT_pin);
}
//消毒
static inline void DISINFECTION_ON(void)  {
    GPIO_SetBits(DISINFECTION_OUT_gpio, DISINFECTION_OUT_pin);
}
static inline void DISINFECTION_OFF(void) {
    GPIO_ResetBits(DISINFECTION_OUT_gpio,DISINFECTION_OUT_pin);
}
//LED 灯带
static inline void LED_LIGHT_ON(void)  {
    GPIO_SetBits(LED_LIGHT_OUT_gpio,LED_LIGHT_OUT_pin);
}
static inline void LED_LIGHT_OFF(void) {
    GPIO_ResetBits(LED_LIGHT_OUT_gpio,LED_LIGHT_OUT_pin);
}

//按键指示灯
static inline void LED_DRY_ON(void) {
	GPIO_SetBits(LED_DRY_gpio,LED_DRY_pin);
}
static inline void LED_DRY_OFF(void) {
	GPIO_ResetBits(LED_DRY_gpio,LED_DRY_pin);
}

static inline void LED_ARO_ON(void) {
	GPIO_SetBits(LED_ARO_gpio,LED_ARO_pin);
}
static inline void LED_ARO_OFF(void) {
	GPIO_ResetBits(LED_ARO_gpio,LED_ARO_pin);
}

static inline void LED_DIS_ON(void) {
	GPIO_SetBits(LED_DIS_gpio,LED_DIS_pin);
}
static inline void LED_DIS_OFF(void) {
	GPIO_ResetBits(LED_DIS_gpio,LED_DIS_pin);
}

static inline void LED_RESERVE_ON(void) {
	GPIO_SetBits(LED_RESERVE_gpio,LED_RESERVE_pin);
}
static inline void LED_RESERVE_OFF(void) {
	GPIO_ResetBits(LED_RESERVE_gpio,LED_RESERVE_pin);
}

static inline void LED_INC_ON(void) {
	GPIO_SetBits(LED_INC_gpio,LED_INC_pin);
}
static inline void LED_INC_OFF(void) {
	GPIO_ResetBits(LED_INC_gpio,LED_INC_pin);
}

static inline void LED_DEC_ON(void) {
	GPIO_SetBits(LED_DEC_gpio,LED_DEC_pin);
}
static inline void LED_DEC_OFF(void) {
	GPIO_ResetBits(LED_DEC_gpio,LED_DEC_pin);
}

static inline void LED_POWER_ON(void) {
	GPIO_SetBits(LED_POWER_gpio,LED_POWER_pin);
}
static inline void LED_POWER_OFF(void) {
	GPIO_ResetBits(LED_POWER_gpio,LED_POWER_pin);
}

//蜂鸣器提示
static inline void BUZZER_ON(void) {
	GPIO_SetBits(BUZZER_gpio,BUZZER_pin);
}
static inline void BUZZER_OFF(void) {
	GPIO_ResetBits(BUZZER_gpio,BUZZER_pin);
}

void rt_led_buzzer(u8 bit);
void rt_hw_enable_watchdog(void);
void rt_hw_enable_wwdg(void);
void rt_hw_board_init(void);

#endif /* __BOARD_H__ */
