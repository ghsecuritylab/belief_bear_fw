/*
 * File      : work.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2018, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-19     Bernard      add work.h to this bsp
 */

// <<< Use Configuration Wizard in Context Menu >>>
#ifndef __WORK_H__
#define __WORK_H__
#include <stm32f10x.h>
//#include "stm32f1xx_hal.h"
//#include "gpio.h"
//#include "control.h"
//#include "rtc.h"
//#include "Display.h"
//#include "TFT.h"
/**
*   A: 主控板 A1:手动开 A2:定时开
    B：主控板 + WIFI
    C: 主控触摸面板
    D：主控触摸面板 + WIFI
    工作流程：烘干 + 香薰 => 杀菌
*/
#define TYPE_A          TYPE_A1 | TYPE_A2
#define TYPE_A1         0x01
#define TYPE_A2         0x02


#define TYPE_C          0x10

#define DEV_TYPE        TYPE_C

typedef __packed struct {

    u8 key_press:1;          //1:表示有按键按下  0：无
    u8 switch_state:1;       //1:打开    0:关
    u8 time;                 //剩余时间min
    
}cfg_t;

typedef __packed union _sys_t {
    
    uint32_t msg;
    cfg_t cfg;
    
}sys_t;

typedef enum _sys_s {
    READY = 0,
    RUNNING,
//    SUSPEND,
    SET_TIME,
    RESERVE_TIME,
    
}sys_s;


#endif /* end of __WORK_H__ */
