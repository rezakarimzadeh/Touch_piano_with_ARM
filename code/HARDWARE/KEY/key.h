#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ECA 2014
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)
#define KEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)
#define KEY4  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6) 

#define KEY_4 8
#define KEY_3	4
#define KEY_2	2
#define KEY_1	1

void KEY_Init(void);//IO initialization
u8 KEY_Scan(u8);  	//key scan function				    
#endif
