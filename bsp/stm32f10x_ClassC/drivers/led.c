/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */
#include <rtthread.h>
#include <stm32f10x.h>
#include "board.h"

void rt_hw_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(LED_LINK_rcc|LED_PWRSTATE_rcc, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin   = LED_LINK_pin;
    GPIO_Init(LED_LINK_gpio, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LED_PWRSTATE_pin;
    GPIO_Init(LED_PWRSTATE_gpio, &GPIO_InitStructure);
    
    GPIO_SetBits(LED_LINK_gpio, LED_LINK_pin);
    GPIO_SetBits(LED_PWRSTATE_gpio, LED_PWRSTATE_pin);
}

void rt_hw_led_green_on(void) {
    GPIO_ResetBits(led1_gpio, led1_pin);
}

void rt_hw_led_green_off(void) {
    GPIO_SetBits(led1_gpio, led1_pin);
}
void rt_hw_led_red_on(void) {
    GPIO_ResetBits(led2_gpio, led2_pin);
}
void rt_hw_led_red_off(void) {
    GPIO_SetBits(led2_gpio, led2_pin);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
static rt_uint8_t led_inited = 0;
void led(rt_uint32_t led, rt_uint32_t value)
{
    /* init led configuration if it's not inited. */
    if (!led_inited)
    {
        rt_hw_led_init();
        led_inited = 1;
    }

    if ( led == 0 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(0);
            break;
        case 1:
            rt_hw_led_on(0);
            break;
        default:
            break;
        }
    }

    if ( led == 1 )
    {
        /* set led status */
        switch (value)
        {
        case 0:
            rt_hw_led_off(1);
            break;
        case 1:
            rt_hw_led_on(1);
            break;
        default:
            break;
        }
    }
}
FINSH_FUNCTION_EXPORT(led, set led[0 - 1] on[1] or off[0].)
#endif

