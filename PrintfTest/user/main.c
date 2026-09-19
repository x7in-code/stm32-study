#include "stm32f10x.h"
#include <stdio.h>
#include "Delay.h"

void My_USART_SendBytes(USART_TypeDef*USARTx,uint8_t *pData,uint16_t Size);
void My_USART1_Init(void);	
int main(void)
{
	Delay_Init();
	My_USART1_Init();
	
//	printf("hello word.\r\n");
	while(1)
	{
	  uint32_t currentTick=GetTick();
	
    uint32_t miliseconds=currentTick%1000;
		currentTick=currentTick/1000;
		
		uint32_t seconds=currentTick%60;
		currentTick=currentTick/60;
		
		uint32_t minutes=currentTick%60;
		currentTick=currentTick/60;
		
		uint32_t hour=currentTick;
		
		printf("%02u:%02u:%02u.%03u\r\n",hour,minutes,seconds,miliseconds);
		
		Delay(100);
		
	}
}

//简介；通过串口发送多个字节
//参数 USARTx；填写串口名称
//参数：pDdata:要发送数据的
//参数：Size要发送数据的数量，单位为字节
void My_USART_SendBytes(USART_TypeDef*USARTx,uint8_t *pData,uint16_t Size)
{
	uint32_t i;
for(i=0;i<Size;i++)
	{
		//		等待发送数据寄存器为空
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
//	写入发送数据寄存器中
		USART_SendData(USART1,pData[i]);
		
	}
//	等待发送完成
while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
}
//简介:对USART进行初始化
//PB6-Tx,PB7-Rx
//115200,8,1,None,双向
void My_USART1_Init(void)
{
	//初始化PB6,PB7
GPIO_InitTypeDef GPIO_InitStruct;
	
////	PA9 Tx
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
//	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
//	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_10MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);

////	PA10 Rx
//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
//GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
//GPIO_Init(GPIOA,&GPIO_InitStruct);

RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);

//PB6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
//	PB7
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
GPIO_InitStruct.GPIO_Pin=GPIO_Pin_7;
GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
GPIO_Init(GPIOB,&GPIO_InitStruct);
	//初始化USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_InitTypeDef USART_InitStruct;
	
	USART_InitStruct.USART_BaudRate=115200;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
  USART_InitStruct.USART_Parity=USART_Parity_No;

	USART_Init(USART1,&USART_InitStruct);
	
	USART_Cmd(USART1,ENABLE);//闭合总开关 
}
int fputc(int ch,FILE*f)
{
	//等待TDR为空
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	//发送
	USART_SendData(USART1,(uint8_t)ch);
	return ch;
}
