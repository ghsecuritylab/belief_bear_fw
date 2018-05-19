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
        RTC_ITConfig(RTC_IT_SEC, ENABLE);	//ʹ��RTC���ж�
    } else {
        RTC_ITConfig(RTC_IT_SEC, DISABLE);	//ʹ��RTC���ж�
    }
}

uint8_t rtc_init(void) {
    data_time_t tdat;
    
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	if (BKP_ReadBackupRegister(BKP_DR1) == CONFIGURATION_RESET) {
        rtc_time_get(&tdat);
        RTC_WaitForSynchro();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);	//ʹ��RTC���ж�
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
        time_need_sync = 0;
        system_power_off = 0;
    } else {
        // TODO, need sync time from server
        time_need_sync = 1;
        system_power_off = 1;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��   
		PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ������� 
		BKP_DeInit();	//��λ�������� 	
		RCC_LSEConfig(RCC_LSE_ON);	//�����ⲿ���پ���(LSE),ʹ��������پ���
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);	//���ָ����RCC��־λ�������,�ȴ����پ������	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ��  
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_WaitForSynchro();		//�ȴ�RTC�Ĵ���ͬ��  
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		RTC_EnterConfigMode();/// ��������	
		RTC_SetPrescaler(32767); //����RTCԤ��Ƶ��ֵ
		RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
//		RTC_Set(2009,12,2,10,0,55);  //����ʱ��	
		RTC_ExitConfigMode(); //�˳�����ģʽ  
		BKP_WriteBackupRegister(BKP_DR1, CONFIGURATION_RESET);	//��ָ���ĺ󱸼Ĵ�����д���û���������
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
//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	
//����ʱ��
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�	  
//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
uint8_t rtc_time_set(data_time_t *time) {
    u16 t;
	u32 seccount=0;
	if(time->year<1970||time->year>2099)return 1;	   
	for(t=1970;t<time->year;t++)	//��������ݵ��������
	{
		if(Is_Leap_Year(t))seccount+=31622400;//�����������
		else seccount+=31536000;			  //ƽ���������
	}
	time->month-=1;
	for(t=0;t<time->month;t++)	   //��ǰ���·ݵ����������
	{
		seccount+=(u32)mon_table[t]*86400;//�·����������
		if(Is_Leap_Year(time->year)&&t==1)seccount+=86400;//����2�·�����һ���������	   
	}
	seccount+=(u32)(time->day-1)*86400;//��ǰ�����ڵ���������� 
	seccount+=(u32)time->hour*3600;//Сʱ������
    seccount+=(u32)time->min*60;	 //����������
	seccount+=time->sec;//�������Ӽ���ȥ

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
	PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
	RTC_SetCounter(seccount);	//����RTC��������ֵ

	RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
	return 0;	 
}

uint8_t rtc_time_get(data_time_t *time) {
//	static u16 daycnt=0;
    u16 daycnt=0;
	u32 timecount=0; 
	u32 temp=0;
	u16 temp1=0;	  
    timecount=RTC_GetCounter();	 
 	temp=timecount/86400;   //�õ�����(��������Ӧ��)
	if(daycnt!=temp)//����һ����
	{	  
		daycnt=temp;
		temp1=1970;	//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))//������
			{
				if(temp>=366)temp-=366;//�����������
				else {temp1++;break;}  
			}
			else temp-=365;	  //ƽ�� 
			temp1++;  
		}   
		time->year=temp1;//�õ����
		temp1=0;
		while(temp>=28)//������һ����
		{
			if(Is_Leap_Year(time->year)&&temp1==1)//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		time->month=temp1+1;	//�õ��·�
		time->day=temp+1;  	//�õ����� 
	}
	temp=timecount%86400;     		//�õ�������   	   
	time->hour=temp/3600;     	//Сʱ
	time->min=(temp%3600)/60; 	//����	
	time->sec=(temp%3600)%60; 	//����
//	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//��ȡ����   
	return 0;
}

//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//������������������� 
//����ֵ�����ں�																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// ���Ϊ21����,�������100  
	if (yearH>19)yearL+=100;
	// ����������ֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}	

uint8_t rtc_time_need_sync(void) {
    return time_need_sync;
}
