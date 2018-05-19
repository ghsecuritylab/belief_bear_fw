//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供魔法生活使用，未经作者许可，不得用于其它任何用途

//  文 件 名   : main.c
//  版 本 号   : v1.0
//  作    者   : 米鹏涛
//  生成日期   : 2017/12/01
//  最近修改   : 
//  功能描述   : TFT 240*320显示功能函数
//              说明: 
//              ----------------------------------------------------------------
//              
//              ----------------------------------------------------------------
// 修改历史   :
// 日    期   : 2017/12/01
// 作    者   : 米鹏涛
// 修改内容   : 创建文件
//版权所有，盗版必究。
//******************************************************************************/
#ifndef __DISPLAY_H
#define __DISPLAY_H			  	 

#include "stm32f10x.h"
//#include "stdlib.h"	   

//#define	u8 unsigned char
//#define	u16 unsigned int
//#define	u32 unsigned long

//OLED模式设置
//0:4线串行模式
//1:并行8080模式
    						  
//-----------------OLED---------------- 


//#define OLED_CMD  0	//写命令
//#define OLED_DATA 1	//写数据

//extern  u16 BACK_COLOR, POINT_COLOR;   //背景色，画笔色

//void Lcd_Init(void);

void Display_Init(void);

void xianshi(void );//显示信息
void tft_drying(uint8_t color);
void tft_aromatherapy(uint8_t color);
void tft_disinfection(uint8_t color);
void tft_reserve(uint8_t color);
    
void tft_display_time(uint8_t hour, uint8_t min);
//void show_time(unsigned char x,unsigned char y );//时间显示
//void show_timer(unsigned char x,unsigned char y ,unsigned int COLOR,int time);

void show_BACK(void );
void show_hanzi1(unsigned char x,unsigned char y );



//画笔颜色
//#define WHITE         	 0xFFFF



					  		 
#endif  
	 
	 



