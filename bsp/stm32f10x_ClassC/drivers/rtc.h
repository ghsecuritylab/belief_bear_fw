#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f10x.h"

#define CONFIGURATION_RESET 		0x5555

typedef struct __data_time {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} data_time_t;

void enable_rtc_irq(uint8_t en);
uint8_t rtc_init(void);
uint8_t rtc_time_get(data_time_t *time);
uint8_t rtc_time_set(data_time_t *time);
uint8_t rtc_time_need_sync(void);

#endif /* end of __RTC_H__ */
