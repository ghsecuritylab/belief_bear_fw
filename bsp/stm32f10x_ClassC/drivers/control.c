
#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "control.h"
#include "rtc.h"
#include "Display.h"
#include "TFT.h"
control Ultraviolet_Light;
control LED_Light;
control heat;
control draught;
char key_value=0,key_up = 0,SW_state = 0;//按键值，key_up按键按松开标志

time_out Time_Out; 



void Control_set(void)
{
	if(key_value ==0) return;
	switch (key_value)
  {
  	case 1: Timer_set(&Ultraviolet_Light);
		ultraviolet(&Ultraviolet_Light,GBLUE);//消毒·
  		break;
  	case 2: Timer_set(&draught);
		ultraviolet(&Ultraviolet_Light,BRRED);  //香薰
  		break;
		case 3: Timer_set(&draught);           //烘干
		ultraviolet(&Ultraviolet_Light,RED);
  		break;
		case 4:
  		break;
		case 5:
  		break;
		case 6:
  		break;
		case 7:
  		break;
  	default:
  		break;
  }
	Timeout_start(8000);
	while(1)
	{
		HAL_Delay(999);
	  if(Time_Out.state == 1)
		{
			break;
		}
	  if(Key_up() == 1)  //消毒·
		{
			Timer_set(&Ultraviolet_Light);
			ultraviolet(&Ultraviolet_Light,GBLUE);
			Timeout_start(8000);
			//显示刷新
		}
	 else if(Key_up() == 2) //香薰
		{
			Timer_set(&draught);
			ultraviolet(&Ultraviolet_Light,BRRED);
			Timeout_start(8000);
			//显示刷新
		}
		else if(Key_up() == 3) //预约定时界面
		{
			Timer_set(&draught);
			ultraviolet(&Ultraviolet_Light,RED);
			Timeout_start(8000);
			//显示刷新
		}
		else break;
	}	
	//显示返回主界面，开始工作
    SW_control();


}



void Timer_set(control *Light)
{
  Light->state = 1;
	if(Light->series<5)
	{
		Light->series++;
	}
	else 
	{
		Light->series =0;
	}
}


//手动模式时间设置
void ultraviolet(control *Light,unsigned int COLOR)
{
  if(Light->state ==1)
	{
	 switch (Light->series)
    {
	case 0: Light->time = 0; Light->on_off = 0;
		break;
	case 1: Light->time = 30;Light->on_off = 1;
		break;
	case 2: Light->time = 60;Light->on_off = 1;
		break;
	case 3: Light->time = 120;Light->on_off = 1;
		break;
	case 4: Light->time = 180;Light->on_off = 1;
		break;
	case 5: Light->time = 300;Light->on_off = 1;
		break;
	case 6: Light->time = 480;Light->on_off = 1;
		break;
	default: Light->time = 0;Light->on_off = 0;
		break;
    }	
	}
	
    show_timer(75,75,COLOR,Light->time);
}

void on_off(control *Light)
{
  if(Light->on_off == 1)
	{
	 // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_BIT_RESET);
	}
	else if(Light->on_off == 0)
	{
		Light->series = 0;
		// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_BIT_RESET);
	}

}
/********************************************************
       定时时间中断，每1S减一
********************************************************/
void Tim_interrupt(void )
{
  if(Ultraviolet_Light.time>0)
	{
		Ultraviolet_Light.time--;
	}
	else 
	{
		Ultraviolet_Light.on_off = 0;
		on_off(&Ultraviolet_Light);
	}
}


/********************************************************
             RTC时间设置
********************************************************/
void Time_set(void)
{
	RTC_TimeTypeDef sTime;
	HAL_RTC_GetTime(&hrtc, &sTime,RTC_FORMAT_BIN);
}
/********************************************************
         按键检测(中断判断)
********************************************************/
void key_Scan(void)
{
	HAL_Delay(20);
	if(KEY1()==0)key_value = 1;
	else if(KEY2()==0)key_value = 2;
	else if(KEY3()==0)key_value = 3;
	else if(KEY4()==0)key_value = 4;
//	else if(KEY5()==0)key_value = 5;
//	else if(KEY6()==0)key_value = 6;
//	else if(KEY7()==0)key_value = 7;
	
//	else if(KEY1()==1&&KEY2()==1&&KEY3()==1&&KEY4()==1&&KEY5()==1&&KEY6()==1&&KEY7()==1) 	    
// 	key_value = 0;// 无按键按下
}


/********************************************************
         按键响应，处理相关功能
********************************************************/
char Key_up(void)
{
	if(key_value ==0)return 0;
	HAL_Delay(500);
	switch (key_value)
  {
  	case 1:    if(KEY1() == 0) return 1;
  	case 2:    if(KEY2() == 0) return 2; 
		case 3:    if(KEY3() == 0)return 3; 
		case 4:    if(KEY4() == 0)return 4; 
//		case 5:    if(KEY5() == 0)return 5; 
//		case 6:    if(KEY6() == 0)return 6; 
//		case 7:    if(KEY7() == 0)return 7; 
  	default:   key_value = 0; break;
  }	
	return 0;
}



/********************************************************
         控制开关判断控制
********************************************************/
void SW_control(void)
{
	if(SW_state == 0) return;
    if(Ultraviolet_Light.on_off ==1)
	{
        SW_xiaodu(0);	
	}
	else 
	{
		SW_xiaodu(1);
	}
	if(heat.on_off ==1)
	{
        SW_jiare(0);	
	}
	else 
	{
		SW_jiare(1);
	}
	if(draught.on_off ==1)
	{
        SW_draught(0);	
	}
	else 
	{
		SW_draught(1);
	}
}

/**********************************************************
        超时开始
**********************************************************/
void Timeout_start(unsigned int time)
{
    Time_Out.up = 1;
	Time_Out.state = 0;
	Time_Out.value = time;
}
void Timeout_interrupt(void)
{
  if(Time_Out.up == 1)
	{
	  if(Time_Out.value!=0)
	     Time_Out.value --;
		else 
		{
			Time_Out.state  = 1;
			Time_Out.up = 0;
		}	
	}
}


