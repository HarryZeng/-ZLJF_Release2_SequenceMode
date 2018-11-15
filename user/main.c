#include "stm32f10x.h"
#include "project.h"
#include "key.h"
#include "stdio.h"
#include "display.h"
#include "menu.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_flash.h"
//#include "stm32f10x_dac.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_pwr.h"
#include "bsp_init.h"
#include "flash.h"

uint8_t CheckFLag = 0;

uint8_t TIM1step = 0;

RCC_ClocksTypeDef SysClock;
/****************************??????****************************/
extern uint32_t ShortCircuitLastTime;
//uint32_t timenum;
extern uint8_t EventFlag;
extern uint8_t ShortCircuit;
extern uint8_t ShortCircuitTimer;
extern int16_t OUT2_TimerCounter;
extern int16_t OUT3_TimerCounter;
extern uint8_t OUT3;
extern uint8_t OUT2;
extern uint8_t OUT1;
extern uint32_t CPV;
extern uint8_t PVD_Flag;

void timer_init(void);
void GPIO_Config(void);
uint8_t FlashCheck(void);
void GPIO_DEINIT_ALL(void);
void WriteFlash(uint32_t addr, uint32_t data);
uint8_t StartFLag=0;
uint8_t CheckStartFLag=1;
/*****************************************/
///////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  //加入以下代码,支持printf函数,而不需要选择use MicroLIB
  */
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		;
	USART_SendData(USART1, (unsigned char)ch);
	return (ch);
}

void DelaymsSet(int16_t ms)
{
	while (1)
	{
		ms--;
		if (ms <= 0)
			break;
	}
}

void TIM4_IRQHandler()
{
	if (TIM_GetITStatus(TIM4, TIM_FLAG_Update)) //判断发生update事件中断
	{
		TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update); //清除update事件中断标志
													  //GPIO_WriteBit(OUT3_GPIO_Port, OUT3_Pin, (BitAction)(1 - GPIO_ReadOutputDataBit(OUT3_GPIO_Port, OUT3_Pin)));
													  //GPIO_WriteBit(OUT3_GPIO_Port,OUT3_Pin,Bit_RESET);/*拉高*/
	}
}

extern bool timeflag;


void TIM2_IRQHandler()
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)) //判断发生update事件中断
	{
//		if(PVD_Flag==0)
//		{	
//			timenum++;
//			//	_Gpio_7_set;
//			OUT1_Mode.DelayCounter++;

//			//GPIOB->ODR ^= GPIO_Pin_8;
//			if (timenum % 10 == 0 && StartFLag) /*10us*100us=1000us*/
//			{
//				//_Gpio_7_set;
//				if (OUT2)
//					OUT2_TimerCounter++;
//				if (OUT3)
//					OUT3_TimerCounter++;

//				SMG_Diplay();
//				ShortCircuitLastTime++;
//				if (ShortCircuit)
//					ShortCircuitCounter++;
//				else
//					ShortCircuitCounter = 0;
//			}
//			if (timenum % 80 == 0 && StartFLag) /*80us*100us=8000us*/
//			{
//				//_Gpio_7_set;
//				Key_Scan(); //定时扫描按键
//				KeytempPress = 1;
//			}
//			if(timenum % 2000 ==0)
//			{
//				StartFLag = 1;
//			}
//			if (timenum >= 4000) /*5000*100us = 500,000us = 500ms*/
//			{
//				CheckStartFLag = 0;
//				GetTotalADCValue();
//				timeflag = !timeflag;
//				EventFlag = EventFlag | Blink500msFlag;
//				timenum = 0;
//			}
//		}
				TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除update事件中断标志
	}
}

void TIM1_BRK_UP_TRG_COM_IRQHandler()
{
	if (TIM_GetITStatus(TIM1, TIM_FLAG_Update)) //判断发生update事件中断
	{

		TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update); //清除update事件中断标志
	}
}

/*TIM3 IRQ*/

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update)) //判断发生update事件中断
	{
		//			_Gpio_7_set;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除update事件中断标志
	}
}
void PVD_IRQHandler(void)
{
//	if(EXTI_GetITStatus(EXTI_Line16) != RESET)
//	{
//		PVD_Flag = 1;
//		/* Clear the Key Button EXTI line pending bit */
//		EXTI_ClearITPendingBit(EXTI_Line16);
//	}
		if(PWR_GetFlagStatus(PWR_FLAG_PVDO)!=RESET) //2018-10-27
		{
				//VDD电压低于阈值电压
			 PVD_Flag = 1;
		}
		else
		{   PVD_Flag =0;
				//VDD电压高于阈值电压
		}	
		EXTI_ClearITPendingBit(EXTI_Line16);
}
/******************************************
 BSP 底层初始化
******************************************/

void bsp_init(void)
{
	RCC_Configuration();
	RCC_GetClocksFreq(&SysClock);
	//TIM2_init();
	PVD_init();
	Button_Init();
	/*等待*/
	IO_GPIO_INIT();
	SMG_GPIO_INIT();
	//TIM3_init();
	ADC1_Configuration();
#ifdef DAC_OUT_Enable
	DAC_Configuration();
#endif
	
}

uint32_t ProgramCounter = 0;
void ProgramCheck(void)
{
	//ProgramCounter = ReadFlash(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS);
	ProgramCounter 		= *(__IO uint32_t*)(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS);
	//if (ProgramCounter > 65535 || ProgramCounter < 0)
	if(ProgramCounter == 0xFFFFFFFF)
	{
		//ProgramCounter = 0;
		ResetParameter();
		WriteFlash(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS, 0x0505);
	}
	//ProgramCounter = ProgramCounter + 1;
	//WriteFlash(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS, ProgramCounter);
	DelaymsSet(50);
//	if (ProgramCounter <= 1)
//	{
//		ResetParameter();
//	}
}

void IWDG_Config(void)
{
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		RCC_ClearFlag();
	}

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	IWDG_SetReload(40000 / 20);
	IWDG_ReloadCounter();

	IWDG_Enable();
}
/******************************************
Main主函数
******************************************/
int main(void)
{
	uint32_t checkcouter;

	bsp_init();
	IWDG_Config();

	CheckFLag = FlashCheck();
	
	//if (1)
	if(CheckFLag)
	{
		/*程序运行次数检测*/
		ProgramCheck();
		/*主要运行函数*/
		Main_Function();
	}
	else
		while (1)
		{
			checkcouter++;
			IWDG_ReloadCounter();
		}
}

/*************************************************************/
#define FLASH_START_ADDR1 0x0800FFF0

uint8_t *UID = (uint8_t *)0x1FFFF7E8;  //获取UID  stm32f0:0x1FFFF7AC,stm32f100:0x1FFFF7E8
uint32_t Fml_Constant = 0x19101943;	//输入到公式的常熟
uint8_t *C = (uint8_t *)&Fml_Constant; //把常量转化成数组
uint8_t FormulaResult[4];
uint32_t FormulaCheck;
uint32_t UIDFlashResult;
uint16_t Fml_CRC16;
uint8_t D[12];

void Formula_100(uint8_t *D, uint8_t *Result)
{
	D[0] = UID[4];
	D[1] = UID[8];
	D[2] = UID[1];
	D[3] = UID[3];
	D[4] = UID[0];
	D[5] = UID[5];
	D[6] = UID[10];
	D[7] = UID[7];
	D[8] = UID[9];
	D[9] = UID[2];
	D[10] = UID[11];
	D[11] = UID[6];

	Result[0] = C[0] ^ D[0];
	Result[1] = C[1] ^ D[6] ^ D[7] ^ D[8] ^ D[9] ^ D[10] ^ D[11];
	Result[2] = C[2] ^ D[4];
	Result[3] = C[3] ^ D[2] ^ D[1];
}

uint8_t FlashCheck(void)
{
	uint8_t FlashFlag;
	Formula_100(D, FormulaResult);
	FormulaCheck = FormulaResult[0] + (FormulaResult[1] << 8) + (FormulaResult[2] << 16) + (FormulaResult[3] << 24);
	UIDFlashResult = *(__IO uint32_t *)(FLASH_START_ADDR1);
	if (UIDFlashResult == FormulaCheck)
		FlashFlag = 1;
	else
		FlashFlag = 0;

	return FlashFlag;
}
uint16_t Formula_CRC16(uint8_t *p, uint8_t len)
{
	uint8_t i;
	while (len--)
	{
		for (i = 0x80; i != 0; i >>= 1)
		{
			if ((Fml_CRC16 & 0x8000) != 0)
			{
				Fml_CRC16 <<= 1;
				Fml_CRC16 ^= 0x1021;
			}
			else
			{
				Fml_CRC16 <<= 1;
			}
			if ((*p & i) != 0)
			{
				Fml_CRC16 ^= 0x1021;
			}
		}
		p++;
	}
	return Fml_CRC16;
}
