//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ħ������ʹ�ã�δ��������ɣ��������������κ���;

//  �� �� ��   : main.c
//  �� �� ��   : v1.0
//  ��    ��   : ������
//  ��������   : 2017/12/01
//  ����޸�   : 
//  ��������   : TFT 240*320��ʾ���ܺ���
//              ˵��: 
//              ----------------------------------------------------------------
//              
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 2017/12/01
// ��    ��   : ������
// �޸�����   : �����ļ�
//��Ȩ���У�����ؾ���
//******************************************************************************/
#ifndef __DISPLAY_H
#define __DISPLAY_H			  	 

#include "stm32f10x.h"
//#include "stdlib.h"	   

//#define	u8 unsigned char
//#define	u16 unsigned int
//#define	u32 unsigned long

//OLEDģʽ����
//0:4�ߴ���ģʽ
//1:����8080ģʽ
    						  
//-----------------OLED---------------- 


//#define OLED_CMD  0	//д����
//#define OLED_DATA 1	//д����

//extern  u16 BACK_COLOR, POINT_COLOR;   //����ɫ������ɫ

//void Lcd_Init(void);

void Display_Init(void);

void xianshi(void );//��ʾ��Ϣ
void tft_drying(uint8_t color);
void tft_aromatherapy(uint8_t color);
void tft_disinfection(uint8_t color);
void tft_reserve(uint8_t color);
    
void tft_display_time(uint8_t hour, uint8_t min);
//void show_time(unsigned char x,unsigned char y );//ʱ����ʾ
//void show_timer(unsigned char x,unsigned char y ,unsigned int COLOR,int time);

void show_BACK(void );
void show_hanzi1(unsigned char x,unsigned char y );



//������ɫ
//#define WHITE         	 0xFFFF



					  		 
#endif  
	 
	 



