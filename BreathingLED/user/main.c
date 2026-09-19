#include "stm32f10x.h"
#include "delay.h"
#include "math.h"

void App_PWM_Init(void);

int main(void)
{
	App_PWM_Init();
	
	while(1)
	{
		float t=GetTick() * 1.0e-3f;
		float duty=0.5*(sin(2*3.14*t)+1);
		uint16_t ccr1=duty*1000;
		
		TIM_SetCompare1(TIM1,ccr1);
		
	}
}
void App_PWM_Init(void)
{
	//初始化IO引脚
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//PA8
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//PB13
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//配置时基单元
	
	//开启TIM1时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	//配置时基单元参数
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
  TIM_TimeBaseInitStruct.TIM_Prescaler=71;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=999;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	
	//ARR寄存器预加载
	
	TIM_ARRPreloadConfig(TIM1,ENABLE);
	
	//闭合开关
	
	TIM_Cmd(TIM1,ENABLE);
	
	//初始化输出比较
	//初始化输出比较通道1
	
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCNPolarity_High;
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=0;
	
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);
	
	//闭合MOE开关
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	//CCR寄存器预加载
	
	TIM_CCPreloadControl(TIM1,ENABLE);
	
}
