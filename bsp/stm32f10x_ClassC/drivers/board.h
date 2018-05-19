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
#define DRYING_IN_rcc                       RCC_AHBPeriph_GPIOB
#define DRYING_IN_gpio                      GPIOB
#define DRYING_IN_pin                       (GPIO_Pin_8)

#define DISINFECTION_IN_rcc                 RCC_AHBPeriph_GPIOB
#define DISINFECTION_IN_gpio                GPIOB
#define DISINFECTION_IN_pin                 (GPIO_Pin_7)

#define AROMATHERAPY_IN_rcc                 RCC_AHBPeriph_GPIOB
#define AROMATHERAPY_IN_gpio                GPIOB
#define AROMATHERAPY_IN_pin                 (GPIO_Pin_6)

#define RESERVE_IN_rcc                      RCC_AHBPeriph_GPIOB
#define RESERVE_IN_gpio                     GPIOB
#define RESERVE_IN_pin                      (GPIO_Pin_5)

#define POWER_IN_rcc                        RCC_AHBPeriph_GPIOA
#define POWER_IN_gpio                       GPIOA
#define POWER_IN_pin                        (GPIO_Pin_15)

//interrupt input
#define INCREASE_IN_rcc                     RCC_AHBPeriph_GPIOB
#define INCREASE_IN_gpio                    GPIOB
#define INCREASE_IN_pin                     (GPIO_Pin_4)

#define DECREASE_IN_rcc                     RCC_AHBPeriph_GPIOB
#define DECREASE_IN_gpio                    GPIOB
#define DECREASE_IN_pin                     (GPIO_Pin_3)

#define MAGNETIC_IN_rcc                     RCC_AHBPeriph_GPIOB
#define MAGNETIC_IN_gpio                    GPIOB
#define MAGNETIC_IN_pin                     (GPIO_Pin_9)
//第一版接口
//#define MAGNETIC_IN_rcc                     RCC_AHBPeriph_GPIOA
//#define MAGNETIC_IN_gpio                    GPIOA
//#define MAGNETIC_IN_pin                     (GPIO_Pin_4)

//out
#define DRYING_OUT_rcc                      RCC_AHBPeriph_GPIOA
#define DRYING_OUT_gpio                     GPIOA
#define DRYING_OUT_pin                      (GPIO_Pin_2)

#define AROMATHERAPY_OUT_rcc                RCC_AHBPeriph_GPIOA
#define AROMATHERAPY_OUT_gpio               GPIOA
#define AROMATHERAPY_OUT_pin                (GPIO_Pin_1)

#define DISINFECTION_OUT_rcc                RCC_AHBPeriph_GPIOA
#define DISINFECTION_OUT_gpio               GPIOA
#define DISINFECTION_OUT_pin                (GPIO_Pin_0)

#define LED_OUT_rcc                         RCC_AHBPeriph_GPIOC
#define LED_OUT_gpio                        GPIOC
#define LED_OUT_pin                         (GPIO_Pin_13)

#define SYS_INDICATE_rcc                    RCC_AHBPeriph_GPIOA
#define SYS_INDICATE_gpio                   GPIOA
#define SYS_INDICATE_pin                    (GPIO_Pin_10)
//////////////////////////////////////////////////////////////////////////////

//循环扫描
#define drying_input        GPIO_ReadInputDataBit(DRYING_IN_gpio,DRYING_IN_pin)
#define aromatherapy_input  GPIO_ReadInputDataBit(AROMATHERAPY_IN_gpio,AROMATHERAPY_IN_pin)
#define disinfection_input  GPIO_ReadInputDataBit(DISINFECTION_IN_gpio,DISINFECTION_IN_pin)
#define reserve_input       GPIO_ReadInputDataBit(RESERVE_IN_gpio,RESERVE_IN_pin)
#define power_input         GPIO_ReadInputDataBit(POWER_IN_gpio,POWER_IN_pin)

#define increase_input  GPIO_ReadInputDataBit(INCREASE_IN_gpio,INCREASE_IN_pin)
#define decrease_input  GPIO_ReadInputDataBit(DECREASE_IN_gpio,DECREASE_IN_pin)
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
//LED
static inline void LED_ON(void)  {
    GPIO_SetBits(LED_OUT_gpio,LED_OUT_pin);
}
static inline void LED_OFF(void) {
    GPIO_ResetBits(LED_OUT_gpio,LED_OUT_pin);
}
//system indicate
static inline void SYS_ON(void) {
    GPIO_ResetBits(SYS_INDICATE_gpio,SYS_INDICATE_pin);
}


void rt_hw_enable_watchdog(void);
void rt_hw_enable_wwdg(void);
void rt_hw_board_init(void);

#endif /* __BOARD_H__ */
