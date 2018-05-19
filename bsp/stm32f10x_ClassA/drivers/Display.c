//////////////////////////////////////////////////////////////////////////////////	 

//  �� �� ��   : display.c
//  �� �� ��   : v1.0
//  ��    ��   : ������
//  ��������   : 2017-1201
//  ����޸�   : 
//  ��������   : LED������ʾ�ļ�(stm32ϵ��)

//******************************************************************************/

#include "Display.h"
#include "TFT.h"
//#include "bmp.h"


u8 ref=0;//ˢ����ʾ
u16 vx=15542,vy=11165;  //�������ӣ���ֵ����1000֮���ʾ���ٸ�ADֵ����һ�����ص�
u16 chx=140,chy=146;//Ĭ�����ص�����Ϊ0ʱ��AD��ʼֵ
void Display_Init(void)
{
    Lcd_Init();			//��ʼ��OLED  
	BACK_COLOR=BLACK;
    LCD_Clear(BACK_COLOR); //����
	
	xianshi(); //��ʾ��Ϣ
	
}


void xianshi(void )//��ʼ��������ʾ
{   
	POINT_COLOR=YELLOW;
	
    show_XGJ(96,12,0);      //������
	show_6464(26,12,0);
	show_6464(230,12,1);

	show_6464(8,120,2);     //���
	show_6464(86,160,3);    //����
	show_6464(170,160,4);   //��޹
	show_6464(248,120,5);   //ԤԼ

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
    ָ��λ����ʾ���ӡ�ħ�����
************************************************/
void show_hanzi1(unsigned char x,unsigned char y )
{
	POINT_COLOR=YELLOW;
	showhanzi(x,y,0);      //ħ
	showhanzi(x+35,y,1);   //��
	showhanzi(x+70,y,2);   //��
	showhanzi(x+105,y,3);  //��
}


/************************************************
    ָ��λ����ʾʱ��
************************************************/

void tft_display_time(uint8_t hour, uint8_t min) {
    uint8_t x = 75;
    uint8_t y = 75;
    
	POINT_COLOR=BLUE;
	LCD_DrawPoint_big(x+85,y+26);
	LCD_DrawPoint_big(x+85,y+50);
// 	LCD_DrawPoint_big(x+112,y+20);
// 	LCD_DrawPoint_big(x+112,y+35);
	
	  /*ʱ��������ʾ*/
	LCD_Show2Num(x,y,hour,2);	
	LCD_Show2Num(x+95,y,min,2);	
	//LCD_Show2Num(x+120,y,sTime.Seconds,2);   
}


//void show_time(unsigned char x,unsigned char y )
//{
//    /* ��ȡʱ��*/
//	RTC_TimeTypeDef sTime;
//	HAL_RTC_GetTime(&hrtc, &sTime,RTC_FORMAT_BIN);
//	//dec = sprintf( st, "%f",sTime);
//	
//     /*ʱ���ʽ��ʾ*/
//	POINT_COLOR=BLUE;
//	LCD_DrawPoint_big(x+85,y+26);
//	LCD_DrawPoint_big(x+85,y+50);
//// 	LCD_DrawPoint_big(x+112,y+20);
//// 	LCD_DrawPoint_big(x+112,y+35);
//	
//	
//// 	sTime.Hours = 88;
//// 	sTime.Minutes = 88;
//	  /*ʱ��������ʾ*/
//	LCD_Show2Num(x,y,sTime.Hours,2);	
//	LCD_Show2Num(x+95,y,sTime.Minutes,2);	
//	//LCD_Show2Num(x+120,y,sTime.Seconds,2);
//}

//void show_timer(unsigned char x,unsigned char y ,unsigned int COLOR,int time)
//{
//    /* ��ȡʱ��*/
//	RTC_TimeTypeDef sTime;
//    sTime.Hours = time/60;
//  	sTime.Minutes = time%60;
//	
//	POINT_COLOR=COLOR;
//     /*ʱ���ʽ��ʾ*/
//	POINT_COLOR=BLUE;
//	LCD_DrawPoint_big(x+85,y+26);
//	LCD_DrawPoint_big(x+85,y+50);

//	  /*ʱ��������ʾ*/
//	LCD_Show2Num(x,y,sTime.Hours,2);	
//	LCD_Show2Num(x+95,y,sTime.Minutes,2);	
//	//LCD_Show2Num(x+120,y,sTime.Seconds,2);
//}
























