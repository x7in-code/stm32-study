#include "stm32f10x.h"
#include "delay.h"

void App_Button_Init(void);
void App_OnBoardLED_Init(void);


int main(void)
{
	uint8_t previous=Bit_SET,current=Bit_SET;
	
	App_Button_Init();//按钮初始化
  App_OnBoardLED_Init();//板载初始化
	
	while(1)
	{
		previous=current;
		
		current=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
		
		if(current!=previous)
		{
			//改变LED
			if(current==Bit_SET)
			{
				if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13)==Bit_SET)
				{
					GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
					
				}
				else
					{
						GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
						
					}
			}
			else
			{
				
				
			}
			Delay(10);
			
		}
		
		
	}
}
void App_Button_Init(void)
{
	//PA0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}
void App_OnBoardLED_Init(void)
{
	//PC13
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
}
