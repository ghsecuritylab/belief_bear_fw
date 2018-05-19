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
	unsigned char state;  //�ֶ�ģʽ�а���������1��
	unsigned char series; //��ǰ��ʱʱ�伶��
	unsigned char mode;   //1�Զ���ʱģʽ�� 0�ֶ�����ģʽ
	unsigned int  time;  //��ʱʱ��	
    
    unsigned char button_press;     //�������±�־
}control;

extern control Ultraviolet_Light;
extern control LED_Light;
extern control heat; //����
extern control draught; //����


typedef __packed  struct
{
    unsigned char on_off; 
    unsigned char state;  //����״̬1��ʾ�򿪣�0��ʾ�رգ�
    unsigned char mode;   //1�Զ���ʱģʽ�� 0�ֶ�����ģʽ
	
}cabinet;

typedef __packed  struct
{
    unsigned char value;   //����ֵ
    unsigned char up;  //�����ɿ���־
    unsigned char state;   //����״̬
	
}key;
extern key KEY; 


typedef __packed  struct
{
    unsigned int value;   //����ֵ
	unsigned char up;  //ʹ�ñ�־
	unsigned char state;   //��ʱ��־
	
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


