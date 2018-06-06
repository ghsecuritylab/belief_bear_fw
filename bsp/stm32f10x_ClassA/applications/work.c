
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "board.h"
#include "sysconfig.h"
#include "work.h"
#include "rtc.h"
#include "Display.h"

static sys_t drying,disinfection,aromatherapy,reserve;

#define THREAD_TFT_REFRESH_PRIORITY     5
#define THREAD_BUSINESS_PRIORITY        6
#define THREAD_KEY_SCAN_PRIORITY        7

#define TIME_STEP                       30
#define RESERVE_STEP                    1

static rt_thread_t tft_tid = RT_NULL;
static rt_thread_t key_scan_tid = RT_NULL;
static rt_thread_t business_tid = RT_NULL;
u8 *pointer_time = RT_NULL;
static u8 start_running = 0;
static u8 step = 0;
static data_time_t reserve_time;
static data_time_t cur_time;
static rt_tick_t reserve_last_tick = 0;
static u8 reset_buff[4] = {0};
static u8 reset_index = 0;
static u8 time_update_flag = 0;    
static u8 inc_dec_press = 0;
extern uint8_t system_power_off;
//RTC时钟中断
//每秒触发一次  
static uint8_t sec_cnt = 0;
void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)//秒钟中断
	{
        if (++sec_cnt == 60) {
            sec_cnt = 0;
            time_update_flag = 1;
            if (drying.cfg.time > 0) {
                drying.cfg.time --;
            } 
            if (aromatherapy.cfg.time > 0) {
                aromatherapy.cfg.time --;
            }

            if (disinfection.cfg.switch_state == 1 && disinfection.cfg.time > 0) {
                disinfection.cfg.time --;
            }  
        }
 	}
    			  								 
	RTC_ClearITPendingBit(RTC_IT_SEC);		//清闹钟中断
	RTC_WaitForLastTask();	  	    						 	   	 
}


//外部中断0服务程序 默认接地下降沿中断
//void EXTI0_IRQHandler(void)
//{
//	rt_thread_delay(10*RT_TICK_PER_SECOND/1000);//消抖
//	if(magnetic_input == 1)	 	 //
//	{				 
//		LED_ON();	
//        start_running = 0;  //停掉整个正在运行的系统
//	}
//	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
//}

//void EXTI1_IRQHandler(void)
//{
//	rt_thread_delay(10*RT_TICK_PER_SECOND/1000);//消抖
//	if(increase_input == 0 && pointer_time != RT_NULL)	 	 //
//	{
//            
//		(*pointer_time) += step;	
//	}
//	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE0上的中断标志位  
//}

//void EXTI2_IRQHandler(void)
//{
//	rt_thread_delay(10*RT_TICK_PER_SECOND/1000);//消抖
//	if(decrease_input == 0 && pointer_time != RT_NULL)	 	 //
//	{				 
//        (*pointer_time) += step;
//	}
//	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
//}

static void clear_all_state(void) {
    drying.msg = 0;
    disinfection.msg = 0;
    aromatherapy.msg = 0;
    reserve.msg = 0;
    reserve_time.hour = 0;
    reserve_time.min = 0;
    pointer_time = RT_NULL;
    BKP_WriteBackupRegister(BKP_DR2, 0);
    BKP_WriteBackupRegister(BKP_DR3, 0);
    BKP_WriteBackupRegister(BKP_DR4, 0);
}


/*IO需要加0.1uF电容，不然线太长，电压波动不稳*/
static int magnetic_is_open() {
    if (magnetic_input == 1) {
        rt_thread_delay(RT_TICK_PER_SECOND/2);
        if (magnetic_input == 1) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}


void ap_business_thread(void *para) {

    u32 cur_tick = 0;
    u8 to_day, last_day;
    
    SYS_ON();
    while (1) {
        IWDG_ReloadCounter();
        rt_thread_delay(RT_TICK_PER_SECOND/10);
#if (DEV_TYPE == TYPE_C)        
        if (!start_running) {
            DRYING_OFF();      
            AROMATHERAPY_OFF();
            DISINFECTION_OFF();
            drying.cfg.switch_state = 0;
            aromatherapy.cfg.switch_state = 0;
            disinfection.cfg.switch_state = 0;
            uint32_t times = reserve_time.hour*60 + reserve_time.min;
            if (times > 0) {
                BKP_WriteBackupRegister(BKP_DR5, times);
            }
            enable_rtc_irq(0);
            continue ;
        } 
#endif        
        //烘干
        if (drying.cfg.time > 0) {
            DRYING_ON();
            drying.cfg.switch_state = 1;
        } else {
            DRYING_OFF();
            drying.cfg.switch_state = 0;
            drying.cfg.key_press = 0;
        }
        //香薰
        if (aromatherapy.cfg.time > 0) {
            AROMATHERAPY_ON();
            aromatherapy.cfg.switch_state = 1;
        } else {
            AROMATHERAPY_OFF();
            aromatherapy.cfg.switch_state = 0;
            aromatherapy.cfg.key_press = 0;
        }
        //杀菌等待前面两项完成
        if (drying.cfg.switch_state == 0 && aromatherapy.cfg.switch_state == 0 ) {
            if (disinfection.cfg.time > 0) {
                DISINFECTION_ON();
                disinfection.cfg.switch_state = 1;
            } else {
                DISINFECTION_OFF();
                disinfection.cfg.switch_state = 0;
                disinfection.cfg.key_press = 0;
                enable_rtc_irq(0); 
                start_running = 0;  //end of running
            }
        }
        //磁控门打开
        while (magnetic_is_open()) {

            DRYING_OFF();
            AROMATHERAPY_OFF();
            DISINFECTION_OFF();
            LED_ON();
            enable_rtc_irq(0); 
            IWDG_ReloadCounter();
            rt_thread_delay(RT_TICK_PER_SECOND/10);
        }    
        LED_OFF();
        enable_rtc_irq(1);
        rt_thread_delay(RT_TICK_PER_SECOND/10);
        //保存结束时间到backup_register
        if (time_update_flag == 1) {
#if (DEV_TYPE & TYPE_A)    
            time_update_flag = 0;
#endif            
            BKP_WriteBackupRegister(BKP_DR2, drying.cfg.time);
            BKP_WriteBackupRegister(BKP_DR3, aromatherapy.cfg.time);
            BKP_WriteBackupRegister(BKP_DR4, disinfection.cfg.time);
        }
        
#if (DEV_TYPE == TYPE_A2)
    data_time_t dtime;
    rtc_time_get(&dtime);
    if (dtime.day != last_day) {
        last_day = dtime.day;
        to_day = 1;
    }     
    if (to_day == 1 && dtime.hour == 0x07 && dtime.min == 0x1E) {
        
        to_day = 0;
        drying.cfg.time = 30;
        aromatherapy.cfg.time = 15;
        disinfection.cfg.time = 30;
    }
    enable_rtc_irq(1);
    start_running = 1;
#endif   
    }
}

void ap_tft_refresh_thread(void *para) {
  
    while (1) {
        
        // key  is press 
        if (drying.cfg.key_press ) {
           
            tft_drying(1);          // red
            tft_display_time(drying.cfg.time/60,drying.cfg.time%60);
            while (!disinfection.cfg.key_press && !aromatherapy.cfg.key_press && !start_running ) {
                if (inc_dec_press) {
                    inc_dec_press = 0;
                    tft_display_time(drying.cfg.time/60,drying.cfg.time%60);
                }
                rt_thread_delay(100*RT_TICK_PER_SECOND/1000);
            }   
            drying.cfg.key_press = 0;
            tft_drying(0);          // yellow
        } 
        if (aromatherapy.cfg.key_press) {
            tft_aromatherapy(1);
            tft_display_time(aromatherapy.cfg.time/60,aromatherapy.cfg.time%60);
            while (!drying.cfg.key_press && !disinfection.cfg.key_press && !start_running) {
                if (inc_dec_press) {
                    inc_dec_press = 0;
                    tft_display_time(aromatherapy.cfg.time/60,aromatherapy.cfg.time%60);
                }
                rt_thread_delay(100*RT_TICK_PER_SECOND/1000);
            } 
            aromatherapy.cfg.key_press = 0;
            tft_aromatherapy(0);
        } 
        
        if (disinfection.cfg.key_press ) {
            tft_disinfection(1);
            tft_display_time(disinfection.cfg.time/60,disinfection.cfg.time%60);
            while (!drying.cfg.key_press && !aromatherapy.cfg.key_press && !start_running) {
                if (inc_dec_press) {
                    inc_dec_press = 0;
                    tft_display_time(disinfection.cfg.time/60,disinfection.cfg.time%60);
                }
                rt_thread_delay(100*RT_TICK_PER_SECOND/1000);
            } 
            disinfection.cfg.key_press = 0;
            tft_disinfection(0);
        } 
        //running ...
         if (start_running) {
            tft_reserve(0);
            if (drying.cfg.switch_state) {
                tft_drying(1);
                rt_thread_delay(100*RT_TICK_PER_SECOND/1000);
                if (aromatherapy.cfg.switch_state) {
                    tft_aromatherapy(1);
                }
                tft_display_time(drying.cfg.time/60,drying.cfg.time%60);
                while (drying.cfg.switch_state) {
                    if (time_update_flag) {
                        time_update_flag = 0;
                        tft_display_time(drying.cfg.time/60,drying.cfg.time%60);
                    }
                    rt_thread_delay(100*RT_TICK_PER_SECOND/1000);
                }   
            }
            tft_display_time(0,0);
            tft_drying(0);
            if (disinfection.cfg.switch_state) {
                tft_disinfection(1);
                rt_thread_delay(100*RT_TICK_PER_SECOND/1000);
                tft_display_time(disinfection.cfg.time/60,disinfection.cfg.time%60);
                while (disinfection.cfg.switch_state) {
                    if (time_update_flag) {
                        time_update_flag = 0;
                        tft_display_time(disinfection.cfg.time/60,disinfection.cfg.time%60);
                    }
                    rt_thread_delay(100*RT_TICK_PER_SECOND/1000);
                }
            }
            tft_display_time(0,0);
            tft_disinfection(0);
            if (aromatherapy.cfg.switch_state) {
                tft_aromatherapy(1);
                rt_thread_delay(100*RT_TICK_PER_SECOND/1000);
                tft_display_time(aromatherapy.cfg.time/60,aromatherapy.cfg.time%60);
                while (aromatherapy.cfg.switch_state) {
                    if (time_update_flag) {
                        time_update_flag = 0;
                        tft_display_time(aromatherapy.cfg.time/60,aromatherapy.cfg.time%60);
                    }
                    rt_thread_delay(100*RT_TICK_PER_SECOND/1000);
                }
            }
            tft_display_time(0,0);  
            tft_aromatherapy(0);  
            clear_all_state();
        }
        rt_thread_delay(200*RT_TICK_PER_SECOND/1000);
        
        //剩余时间显示
        rtc_time_get(&cur_time);
        if (reserve.cfg.key_press ) {
            /*如果预约按下，则显示预约时间点，5s后显示当前时间,
            如果预约时间还剩一个小时，则一直显示剩余时间
            */ 

            //剩余一个小时以内
            u32 cur_times = cur_time.hour*60 + cur_time.min;
            u32 reserve_times = reserve_time.hour*60 + reserve_time.min;
            if (reserve_times >= cur_times) {
                tft_reserve(1);
            } else {
                tft_reserve(0); 
            }
            if (reserve_times - cur_times < 60) {
                reserve.cfg.time = reserve_times - cur_times;
                tft_display_time(0,reserve.cfg.time);
            } else {
                tft_display_time(reserve_time.hour,reserve_time.min);
            }
            while (rt_tick_get() < reserve_last_tick+RT_TICK_PER_SECOND) {
                if (inc_dec_press) {
                    inc_dec_press = 0;
                    reserve_last_tick = rt_tick_get();
                    tft_display_time(reserve_time.hour,reserve_time.min);  
                }
                rt_thread_delay(RT_TICK_PER_SECOND/10);
            }
                
        } else {
            tft_display_time(cur_time.hour,cur_time.min); 
        }
        
        rt_thread_delay(RT_TICK_PER_SECOND/10);
    }
}


void ap_key_scan_thread(void *para) {
    static u8 hour_min = 0;

    while (1) {
        //assist key 
        if (increase_input) {
            inc_dec_press = 1;
          //  rt_thread_delay(20*RT_TICK_PER_SECOND/1000);
           // if (increase_input) {
                (*pointer_time) += step;
          //  }
            rt_thread_delay(RT_TICK_PER_SECOND/2);
        }
        if (decrease_input) {
            inc_dec_press = 1;
           // rt_thread_delay(20*RT_TICK_PER_SECOND/1000);
           // if (decrease_input) {
                (*pointer_time) -= step;
          //  }
            rt_thread_delay(RT_TICK_PER_SECOND/2);
        }
        
        //function key
        if (drying_input) {
            drying.cfg.key_press = 1;
            drying.cfg.time = 30;
            pointer_time = &drying.cfg.time;
            step = TIME_STEP;
            reset_buff[reset_index++] = 'A';
            rt_thread_delay(RT_TICK_PER_SECOND/2);
        }
        if (disinfection_input) {
            disinfection.cfg.key_press = 1;
            disinfection.cfg.time = 30;
            pointer_time = &disinfection.cfg.time;
            step = TIME_STEP;
            reset_buff[reset_index++] = 'B';
            rt_thread_delay(RT_TICK_PER_SECOND/2);
        }
        if (aromatherapy_input) {
            aromatherapy.cfg.key_press = 1;
            aromatherapy.cfg.time = 30;
            pointer_time = &aromatherapy.cfg.time;
            step = TIME_STEP;
            //enter keyboard
            if (rt_memcmp(reset_buff,"ABBA",4) == 0) {
                //set time 
                data_time_t set_times;
                set_times.hour = 12;
                set_times.min = 30;
                step = RESERVE_STEP;
                while (!power_input) {  //break
                    if (hour_min == 1) {
                        pointer_time = &set_times.hour;
                    } else {
                        pointer_time = &set_times.min;
                    } 
                    if (aromatherapy_input) {
                        hour_min ++;
                        hour_min %=2; 
                    }
                    rt_thread_delay(50*RT_TICK_PER_SECOND/1000);
                    rtc_time_set(&set_times);
                }
                pointer_time = RT_NULL;
                drying.cfg.time = 0;
                disinfection.cfg.time = 0;
            } 
            reset_index = 0;
            rt_memset(reset_buff,0,4);
            rt_thread_delay(RT_TICK_PER_SECOND/2);
            
        }
        if (reserve_input) {
            reserve.cfg.key_press = 1;
            if (reserve_time.min == 0 && reserve_time.hour == 0) {
                rtc_time_get(&reserve_time);
////                reserve_time.hour = 12;
////                reserve_time.min = 30;
            }
            if (hour_min == 1) {
                pointer_time = &reserve_time.hour;
            } else {
                pointer_time = &reserve_time.min;
            } 
            step = RESERVE_STEP;
            hour_min ++;
            hour_min %=2; 
            reserve_last_tick = rt_tick_get();
            rt_thread_delay(RT_TICK_PER_SECOND/2);
        }
        if (power_input || reserve.cfg.time == 1) {
            
            if ( start_running == 0) {
                //如果没有设置时间则以默认值运行
                if (drying.cfg.time == 0 && aromatherapy.cfg.time == 0 && disinfection.cfg.time == 0) {
                    drying.cfg.time = 30;
                    aromatherapy.cfg.time = 15;
                    disinfection.cfg.time = 30;
                }
                reserve_time.hour = 0;
                reserve_time.min = 0;
                pointer_time = RT_NULL;
                reserve.msg = 0;
                start_running = 1;
                enable_rtc_irq(1);
            } else {
                //stop 
                clear_all_state();
            }
            rt_thread_delay(RT_TICK_PER_SECOND/2);
        }            
        rt_thread_delay(RT_TICK_PER_SECOND/100);
    }
}


void mrfi_ap(void) {

    IWDG_ReloadCounter();
    drying.cfg.time = BKP_ReadBackupRegister(BKP_DR2);
    aromatherapy.cfg.time = BKP_ReadBackupRegister(BKP_DR3);
    disinfection.cfg.time = BKP_ReadBackupRegister(BKP_DR4);
#if (DEV_TYPE == TYPE_A1)
    //Whether the system is reset
    if (system_power_off == 1) {
        drying.cfg.time = 30;
        aromatherapy.cfg.time = 15;
        disinfection.cfg.time = 30;
    }
    enable_rtc_irq(1);
    start_running = 1;
#endif    
//    uint32_t times = BKP_ReadBackupRegister(BKP_DR5);
//    reserve_time.hour = times / 60;
//    reserve_time.min = times % 60;
    
#if (DEV_TYPE == TYPE_C)     
    tft_tid = rt_thread_create("tft", ap_tft_refresh_thread, RT_NULL, 512, 
                            THREAD_TFT_REFRESH_PRIORITY, 20);
    if (tft_tid != RT_NULL) {
        rt_thread_startup(tft_tid);  
    }
    
    key_scan_tid = rt_thread_create("key_scan", ap_key_scan_thread, RT_NULL, 512, 
                            THREAD_KEY_SCAN_PRIORITY, 5);
    if (key_scan_tid != RT_NULL) {
        rt_thread_startup(key_scan_tid);
    }
#endif    
    
    business_tid = rt_thread_create("business", ap_business_thread, RT_NULL, 512, 
                            THREAD_BUSINESS_PRIORITY, 5);
    if (business_tid != RT_NULL) {
        rt_thread_startup(business_tid);
    }
}

