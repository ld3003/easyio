#include <stm32l1xx.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_exti.h>
#include <stm32l1xx_syscfg.h>

#include "stabdby.h"

int KEY_Init(void){
	//
}

int Check_up(void){
	return 0;
}
	

void Sys_Standby(void) //?????????
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD,ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_All; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_WKUP);//PA10-RX
	
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  PWR_WakeUpPinCmd(PWR_WakeUpPin_1,ENABLE);
	PWR_WakeUpPinCmd(PWR_WakeUpPin_2,ENABLE);
	PWR_WakeUpPinCmd(PWR_WakeUpPin_3,ENABLE);
	//disable watchdog
  PWR_EnterSTANDBYMode();        
}
void Sys_Enter_Standby(void)//????????,????,????????
{             
    //RCC_APB2PeriphResetCmd(0X01FC,DISABLE);    //????????IO?????0x01fc???????????RCC_APB2RSTR(??)
    Sys_Standby();
}

void EXTI0_IRQHandler(void)
{                                                          
	EXTI_ClearITPendingBit(EXTI_Line0); 

}


void Wkup_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	

	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	memset(&GPIO_InitStructure,0x0,sizeof(GPIO_InitStructure));
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	
//	#if 0
//	
//	
//	/* Configure PD.03, PC.04, as input floating */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//	#endif

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_WKUP);//PA10-RX
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; //PA0        
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;  
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;   
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
	NVIC_Init(&NVIC_InitStructure);   
	
	


	
    
	if(Check_up() == 0)   //??????????????????????????????,??????????????????????:??,????,????
	{
		//Sys_Standby();         //??????
	}      
}



void EXTI7_IRQHandler(void)
{                                                          
	EXTI_ClearITPendingBit(EXTI_Line7); 

}

void config_pa7_interrupt(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7);
	
	
	//…Ë÷√÷–∂œ¥•∑¢◊¥Ã¨ 
	EXTI_InitStructure.EXTI_Line = EXTI_Line7 ;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Rising;//EXTI_Trigger_Rising_Falling; //EXTI_Trigger_Rising
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the EXTI12\3 Interrupt on PC13 */
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);



}

