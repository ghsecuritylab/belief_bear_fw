#ifndef __CONTROL_H__
#define __CONTROL_H__
/* Includes ------------------------------------------------------------------*/
#include <stm32f1xx.h>

/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN Private defines */


#define KEY1() HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)

#define KEY2() HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)

#define KEY3() HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)

#define KEY4() HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6)

//#define KEY5() HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)

//#define KEY6() HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)

//#define KEY7() HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3)

#define SW_xiaodu(X)    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,X)

#define SW_xiangxun(X)  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,X)

#define SW_jiare(X)     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,X)

#define SW_draught(X)   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,X)

#define SW_LED(X)       

/* USER CODE END Private defines */


typedef __packed  struct
{
    unsigned char on_off;
	unsigned char state;  //手动模式有按键按下置1；
	unsigned char series; //当前定时时间级数
	unsigned char mode;   //1自动定时模式， 0手动控制模式
	unsigned int  time;  //定时时间	
    
    unsigned char button_press;     //按键按下标志
}control;

extern control Ultraviolet_Light;
extern control LED_Light;
extern control heat; //加温
extern control draught; //风扇


typedef __packed  struct
{
    unsigned char on_off; 
    unsigned char state;  //柜门状态1表示打开，0表示关闭；
    unsigned char mode;   //1自动定时模式， 0手动控制模式
	
}cabinet;

typedef __packed  struct
{
    unsigned char value;   //按键值
    unsigned char up;  //按键松开标志
    unsigned char state;   //按键状态
	
}key;
extern key KEY; 


typedef __packed  struct
{
    unsigned int value;   //按键值
	unsigned char up;  //使用标志
	unsigned char state;   //超时标志
	
}time_out;
extern time_out Time_Out; 

void Control_set(void);

void Timer_set(control *Light);

void ultraviolet(control *Light,unsigned int COLOR);

void on_off(control *Light);

void Tim_interrupt(void );

void Time_set(void);

void key_Scan(void);

char Key_up(void);

void SW_control(void);


void Timeout_start(unsigned int time);

void Timeout_interrupt(void);


#endif /* __CONTROL_H__ */


