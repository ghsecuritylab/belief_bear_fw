//////////////////////////////////////////////////////////////////////////////////	 

//  文 件 名   : display.c
//  版 本 号   : v1.0
//  作    者   : 米鹏涛
//  生成日期   : 2017-1201
//  最近修改   : 
//  功能描述   : LED界面显示文件(stm32系列)

//******************************************************************************/

#include "Display.h"
#include "TFT.h"
//#include "bmp.h"


u8 ref=0;//刷新显示
u16 vx=15542,vy=11165;  //比例因子，此值除以1000之后表示多少个AD值代表一个像素点
u16 chx=140,chy=146;//默认像素点坐标为0时的AD起始值
void Display_Init(void)
{
    Lcd_Init();			//初始化OLED  
	BACK_COLOR=BLACK;
    LCD_Clear(BACK_COLOR); //清屏
	
	xianshi(); //显示信息
	
}


void xianshi(void )//初始化界面显示
{   
	POINT_COLOR=YELLOW;
	
    show_XGJ(96,12,0);      //信仰熊
	show_6464(26,12,0);
	show_6464(230,12,1);

	show_6464(8,120,2);     //烘干
	show_6464(86,160,3);    //消毒
	show_6464(170,160,4);   //香薰
	show_6464(248,120,5);   //预约

}

void tft_drying(uint8_t color) {
    if (color) {
        POINT_COLOR = RED;
    } else {
        POINT_COLOR = YELLOW;
    }
    show_6464(8,120,2); 
}

void tft_disinfection(uint8_t color) {
    if (color) {
        POINT_COLOR = RED;
    } else {
        POINT_COLOR = YELLOW;
    }
    show_6464(86,160,3);
}

void tft_aromatherapy(uint8_t color) {
    if (color) {
        POINT_COLOR = RED;
    } else {
        POINT_COLOR = YELLOW;
    }
    show_6464(170,160,4);
}

void tft_reserve(uint8_t color) {
    if (color) {
        POINT_COLOR = RED;
    } else {
        POINT_COLOR = YELLOW;
    }
    show_6464(248,120,5);
}
/************************************************
    指定位置显示汉子《魔法生活》
************************************************/
void show_hanzi1(unsigned char x,unsigned char y )
{
	POINT_COLOR=YELLOW;
	showhanzi(x,y,0);      //魔
	showhanzi(x+35,y,1);   //法
	showhanzi(x+70,y,2);   //生
	showhanzi(x+105,y,3);  //活
}


/************************************************
    指定位置显示时间
************************************************/

void tft_display_time(uint8_t hour, uint8_t min) {
    uint8_t x = 75;
    uint8_t y = 75;
    
	POINT_COLOR=BLUE;
	LCD_DrawPoint_big(x+85,y+26);
	LCD_DrawPoint_big(x+85,y+50);
// 	LCD_DrawPoint_big(x+112,y+20);
// 	LCD_DrawPoint_big(x+112,y+35);
	
	  /*时间数字显示*/
	LCD_Show2Num(x,y,hour,2);	
	LCD_Show2Num(x+95,y,min,2);	
	//LCD_Show2Num(x+120,y,sTime.Seconds,2);   
}


//void show_time(unsigned char x,unsigned char y )
//{
//    /* 读取时间*/
//	RTC_TimeTypeDef sTime;
//	HAL_RTC_GetTime(&hrtc, &sTime,RTC_FORMAT_BIN);
//	//dec = sprintf( st, "%f",sTime);
//	
//     /*时间格式显示*/
//	POINT_COLOR=BLUE;
//	LCD_DrawPoint_big(x+85,y+26);
//	LCD_DrawPoint_big(x+85,y+50);
//// 	LCD_DrawPoint_big(x+112,y+20);
//// 	LCD_DrawPoint_big(x+112,y+35);
//	
//	
//// 	sTime.Hours = 88;
//// 	sTime.Minutes = 88;
//	  /*时间数字显示*/
//	LCD_Show2Num(x,y,sTime.Hours,2);	
//	LCD_Show2Num(x+95,y,sTime.Minutes,2);	
//	//LCD_Show2Num(x+120,y,sTime.Seconds,2);
//}

//void show_timer(unsigned char x,unsigned char y ,unsigned int COLOR,int time)
//{
//    /* 读取时间*/
//	RTC_TimeTypeDef sTime;
//    sTime.Hours = time/60;
//  	sTime.Minutes = time%60;
//	
//	POINT_COLOR=COLOR;
//     /*时间格式显示*/
//	POINT_COLOR=BLUE;
//	LCD_DrawPoint_big(x+85,y+26);
//	LCD_DrawPoint_big(x+85,y+50);

//	  /*时间数字显示*/
//	LCD_Show2Num(x,y,sTime.Hours,2);	
//	LCD_Show2Num(x+95,y,sTime.Minutes,2);	
//	//LCD_Show2Num(x+120,y,sTime.Seconds,2);
//}
























