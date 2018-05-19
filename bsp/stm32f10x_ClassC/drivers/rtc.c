#include <stm32f10x.h>
#include <rtthread.h>
#include "rtc.h"

//#define RTCClockSource_LSI
static uint8_t time_need_sync = 1;
uint8_t system_power_off = 0;
static void RTC_NVIC_Config(void){
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void enable_rtc_irq(uint8_t en) {
    if (en == 1) {
        RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
    } else {
        RTC_ITConfig(RTC_IT_SEC, DISABLE);	//使能RTC秒中断
    }
}

uint8_t rtc_init(void) {
    data_time_t tdat;
    
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	if (BKP_ReadBackupRegister(BKP_DR1) == CONFIGURATION_RESET) {
        rtc_time_get(&tdat);
        RTC_WaitForSynchro();	//等待最近一次对RTC寄存器的写操作完成
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
        time_need_sync = 0;
        system_power_off = 0;
    } else {
        // TODO, need sync time from server
        time_need_sync = 1;
        system_power_off = 1;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟   
		PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问 
		BKP_DeInit();	//复位备份区域 	
		RCC_LSEConfig(RCC_LSE_ON);	//设置外部低速晶振(LSE),使用外设低速晶振
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);	//检查指定的RCC标志位设置与否,等待低速晶振就绪	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟  
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_WaitForSynchro();		//等待RTC寄存器同步  
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//使能RTC秒中断
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
		RTC_EnterConfigMode();/// 允许配置	
		RTC_SetPrescaler(32767); //设置RTC预分频的值
		RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成
//		RTC_Set(2009,12,2,10,0,55);  //设置时间	
		RTC_ExitConfigMode(); //退出配置模式  
		BKP_WriteBackupRegister(BKP_DR1, CONFIGURATION_RESET);	//向指定的后备寄存器中写入用户程序数据
    }  
    if (time_need_sync == 0) {
        rtc_time_set(&tdat);
    } else {  
        tdat.year = 2018;
        tdat.month = 1;
        tdat.day = 1;
        tdat.hour = 0;
        tdat.min = 0;
        tdat.sec = 0;
        rtc_time_set(&tdat);
        
        rt_memset(&tdat, 0, sizeof(data_time_t));
        rtc_time_get(&tdat);
    }
//        tdat.year = 2018;
//        tdat.month = 5;
//        tdat.day = 12;
//        tdat.hour = 17;
//        tdat.min = 25;
//        tdat.sec = 0;
//        rtc_time_set(&tdat);
//        BKP_WriteBackupRegister(BKP_DR1, CONFIGURATION_RESET);
    
    RTC_NVIC_Config();
	return 0;
}
//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //必须能被4整除
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	
//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
//平年的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
uint8_t rtc_time_set(data_time_t *time) {
    u16 t;
	u32 seccount=0;
	if(time->year<1970||time->year>2099)return 1;	   
	for(t=1970;t<time->year;t++)	//把所有年份的秒钟相加
	{
		if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
		else seccount+=31536000;			  //平年的秒钟数
	}
	time->month-=1;
	for(t=0;t<time->month;t++)	   //把前面月份的秒钟数相加
	{
		seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加
		if(Is_Leap_Year(time->year)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数	   
	}
	seccount+=(u32)(time->day-1)*86400;//把前面日期的秒钟数相加 
	seccount+=(u32)time->hour*3600;//小时秒钟数
    seccount+=(u32)time->min*60;	 //分钟秒钟数
	seccount+=time->sec;//最后的秒钟加上去

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
	RTC_SetCounter(seccount);	//设置RTC计数器的值

	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成  	
	return 0;	 
}

uint8_t rtc_time_get(data_time_t *time) {
//	static u16 daycnt=0;
    u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
    timecount=RTC_GetCounter();	 
 	temp=timecount/86400;   //得到天数(秒钟数对应的)
	if(daycnt!=temp)//超过一天了
	{	  
		daycnt=temp;
		temp1=1970;	//从1970年开始
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//是闰年
			{
				if(temp>=366)temp-=366;//闰年的秒钟数
				else {temp1++;break;}  
			}
			else temp-=365;	  //平年 
			temp1++;  
		}   
		time->year=temp1;//得到年份
		temp1=0;
		while(temp>=28)//超过了一个月
		{
			if(Is_Leap_Year(time->year)&&temp1==1)//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;//闰年的秒钟数
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;  
		}
		time->month=temp1+1;	//得到月份
		time->day=temp+1;  	//得到日期 
	}
	temp=timecount%86400;     		//得到秒钟数   	   
	time->hour=temp/3600;     	//小时
	time->min=(temp%3600)/60; 	//分钟	
	time->sec=(temp%3600)%60; 	//秒钟
//	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//获取星期   
	return 0;
}

//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日 
//返回值：星期号																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}	

uint8_t rtc_time_need_sync(void) {
    return time_need_sync;
}
