/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2013-07-12     aozima       update for auto initial.
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>
#include "sysconfig.h"
#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif


static void reboot_after_24hour() {
//    rt_tick_t curtick = rt_tick_get();
//    rt_uint32_t hour = curtick / RT_TICK_PER_SECOND / 3600;
//    if (hour > 24) {
//        extern rt_uint8_t mrfi_msg_pending(void);
//        if (mrfi_msg_pending() == 0) {
//            // reboot everyday
//            soft_reset();
//        }
//    } else {
//        rt_thread_delay(1);
//    }
}

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else
        rt_kprintf("File System initialzation failed!\n");
#endif  /* RT_USING_DFS */
    
#ifdef CFG_ACCESS_POINT
	extern int mrfi_ap(void);
	mrfi_ap();
    rt_hw_enable_watchdog();
#endif
    
    // change to lower priority
//    rt_uint8_t pri = 30;
//    rt_thread_control(rt_thread_self(), RT_THREAD_CTRL_CHANGE_PRIORITY, &pri);
    while (1) {
        rt_thread_delay(RT_TICK_PER_SECOND*10);
        
//        extern rt_uint8_t mrfi_need_restart(void);
//        extern void mrfi_restart();
//        if (mrfi_need_restart()) {
//            rt_kprintf("restart mrfi thread!!!!!!!!!!!!\n");
//            mrfi_restart();
//        }

#ifdef CFG_ACCESS_POINT
        reboot_after_24hour();
#endif
    }
}

int rt_application_init(void)
{
    rt_thread_t init_thread;

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   512, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   512, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}

/*@}*/
