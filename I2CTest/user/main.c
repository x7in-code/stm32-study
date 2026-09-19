#include "stm32f10x.h"
void My_I2C_Init(void);
void My_OnBoardLED_Init(void);
int My_I2C_SendBytes(I2C_TypeDef*I2Cx,uint8_t Addr,uint8_t *pData,uint16_t Size);
int My_I2C_ReceiveBytes(I2C_TypeDef *I2Cx,uint8_t Addr,uint8_t *pBuffer,uint16_t Size);

int main(void)
{
	My_I2C_Init();
	My_OnBoardLED_Init();
	
	uint8_t commands[]={0x00,0x8d,0x14,0xaf,0xa5};
	
	My_I2C_SendBytes(I2C1,0x78,commands,5);
	
	uint8_t rcvd;
	My_I2C_ReceiveBytes(I2C1,0x78,&rcvd,1);
	
	if(((rcvd & (0x01<<6)) == 0))
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
	}
	
	while(1)
	{
	}
}

void My_I2C_Init(void)
{
//IO引脚初始化
	//对I2C1进行重映射
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
//对PB8和PB9进行初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//开启I2C1时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1,ENABLE);//施加复位信号
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1,DISABLE);//释放复位信号
	
	I2C_InitTypeDef I2C_InitStruct;
	
	I2C_InitStruct.I2C_ClockSpeed=400000;//波提率400k
	I2C_InitStruct.I2C_Mode=I2C_Mode_I2C;//标准的I2C模式
	I2C_InitStruct.I2C_DutyCycle=I2C_DutyCycle_2;//占空比2：1
	
	I2C_Init(I2C1,&I2C_InitStruct);
	
	I2C_Cmd(I2C1,ENABLE);//闭合I2C1开关
	
}
int My_I2C_SendBytes(I2C_TypeDef*I2Cx,uint8_t Addr,uint8_t *pData,uint16_t Size)
{
//等待总线空闲
	while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_BUSY)==SET);
	//发送起始位
	I2C_GenerateSTART(I2Cx,ENABLE);
	while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_SB)==RESET);
	//寻址阶段
	I2C_ClearFlag(I2Cx,I2C_FLAG_AF);
	
	I2C_SendData(I2Cx,Addr & 0xfe);
	while(1)
	{
	if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_ADDR)==SET)
		{
		break;
		}
		if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_AF)==SET)
		{
			I2C_GenerateSTOP(I2Cx,ENABLE);
			return -1;//寻址失败
		}
	}
//清除ADDR
	I2C_ReadRegister(I2Cx,I2C_Register_SR1);
	I2C_ReadRegister(I2Cx,I2C_Register_SR2);
	//发送数据
	for(uint16_t i=0;i<Size;i++)
	{
	while(1)
	 {
	if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_AF)==SET)
	  {
	  I2C_GenerateSTOP(I2Cx,ENABLE);
		return -2;//数据被拒收
	  }
	if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_TXE)==SET)
	  {
	   break;
	  }
	 }
	I2C_SendData(I2Cx,pData[i]);
	}
	while(1)
	{
		if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_AF)==SET)
		{
			I2C_GenerateSTOP(I2Cx,ENABLE);
		return -2;//数据被拒收
		}
		if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_BTF)==SET)
		{
			break;
		}
	}
	//发送停止位
	I2C_GenerateSTOP(I2Cx,ENABLE);
  return 0;//成功
}
int My_I2C_ReceiveBytes(I2C_TypeDef *I2Cx,uint8_t Addr,uint8_t *pBuffer,uint16_t Size)
{
	//发送起始位
	I2C_GenerateSTART(I2Cx,ENABLE);
	while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_SB)==RESET)
	
	//寻址阶段
	I2C_ClearFlag(I2Cx,I2C_FLAG_AF);
	I2C_SendData(I2Cx,Addr|0x01);
	while(1)
	{
		if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_AF)==SET)
		{
			I2C_GenerateSTOP(I2Cx,ENABLE);
			return -1;//寻址失败
		}
		if(I2C_GetFlagStatus(I2Cx,I2C_FLAG_ADDR)==SET)
		{
			break;
		}
		
	}
		
	//接收数据
	if(Size==1)
	{
		//清除Addr
		I2C_ReadRegister(I2Cx,I2C_Register_SR1);
		I2C_ReadRegister(I2Cx,I2C_Register_SR2);
		//ACK=0,STOP=1
		I2C_AcknowledgeConfig(I2Cx,DISABLE);
		I2C_GenerateSTOP(I2Cx,ENABLE);
		//等待数据接受完成RXNE变成1
		while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_RXNE)==1)
			//读数据
		pBuffer[0]=I2C_ReceiveData(I2Cx);
		
	}
	else if(Size==2)
	{
		//清除ADDR标志位
		I2C_ReadRegister(I2Cx,I2C_Register_SR1);
		I2C_ReadRegister(I2Cx,I2C_Register_SR2);
		//ACK=1
		I2C_AcknowledgeConfig(I2Cx,ENABLE);
		//等待第一个字节接收完成
		while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_RXNE)==RESET);
		//读取第一个字节
		pBuffer[0]=I2C_ReceiveData(I2Cx);
		
		//ACK=0,STOP=1
		I2C_AcknowledgeConfig(I2Cx,DISABLE);
		I2C_GenerateSTOP(I2Cx,ENABLE);
		//等待数据接受完成
		while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_RXNE)==RESET);
		//读取第二个字节
		pBuffer[1]=I2C_ReceiveData(I2Cx);
		
	}
	else
	{
		//清除Addr
		I2C_ReadRegister(I2Cx,I2C_Register_SR1);
		I2C_ReadRegister(I2Cx,I2C_Register_SR2);
		//ACK=1
		I2C_AcknowledgeConfig(I2Cx,ENABLE);
		for(uint16_t i=0;i<Size-1;i++)
		{
			//等待数据接受完成
		while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_RXNE)==RESET);
		//读取数据
		pBuffer[i]=I2C_ReceiveData(I2Cx);
			
		}
		//ACK=0,STOP=1
		I2C_AcknowledgeConfig(I2Cx,DISABLE);
		I2C_GenerateSTOP(I2Cx,ENABLE);
		//等待最后一个数据接受完成
		while(I2C_GetFlagStatus(I2Cx,I2C_FLAG_RXNE)==RESET);
		//读取最后一个数据
		pBuffer[Size-1]=I2C_ReceiveData(I2Cx);
		
		
	}
	
	return 0;//接受成功
}
void My_OnBoardLED_Init(void)
{

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
  
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	

}
