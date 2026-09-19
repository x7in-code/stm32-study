#include "stm32f10x.h"
#include "usart.h"
void My_USART1_Init(void);
	
int main(void)
{
	My_USART1_Init();
	
//	My_USART_SendByte(USART1,0x5a);
	
//	uint8_t byteArray[]={1,2,3,4,5};
//	My_USART_SendBytes(USART1,byteArray,5);
	
//	My_USART_SendChar(USART1,'a');
//	My_USART_SendString(USART1,"HELLO WORD.\r\n");
	
//	const char*StrName="Tom";
//	My_USART_Printf(USART1,"Hi,%s!Nice to meet to you!\r\n",StrName);
//	GPIO_InitTypeDef GPIO_InitStruct;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
//	
//	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;
//	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;
//	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
//	
//	
//	GPIO_Init(GPIOC,&GPIO_InitStruct);
//	
	
	while(1)
	{
//		char c=My_USART_ReceiveByte(USART1);
//		if(c=='0')
//		{
//		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
//		}
//			else if(c=='1')
//			{
//			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
//			}
		char buffer[100];
		
		if(My_USART_ReceiveLine(USART1,buffer,100,LINE_SEPERATOR_CRLF,-1)==0)
		{
		My_USART_SendString(USART1,buffer);
		}
	}
}
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
