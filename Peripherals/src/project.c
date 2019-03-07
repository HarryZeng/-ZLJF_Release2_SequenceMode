
///**
//  ********************************  STM32F0x1  *********************************
//  * @�ļ���     �� differentialDC.c
//  * @����       �� HarryZeng
//  * @��汾     �� V1.5.0
//  * @�ļ��汾   �� V1.0.0
//  * @����       �� 2017��05��11��
//  * @ժҪ       �� ���ݴ���
//  ******************************************************************************/
///*----------------------------------------------------------------------------
//  ������־:
//  2017-05-11 V1.0.0:��ʼ�汾
//  ----------------------------------------------------------------------------*/
///* ������ͷ�ļ� --------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "project.h"
#include "bsp_init.h"
#include "key.h"
#include "display.h"
#include "SelfStudy.h"
#include "menu.h"
#include "flash.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_exti.h"
#include "misc.h"

/*DSP��궨�壺ARM_MATH_CM0*/

uint32_t DealyBaseTime = 8;
uint16_t DEL = 300;

int16_t HI = 1000;
int16_t LO = 700;
bool timeflag = 0;
uint8_t LastOUT1 = 0;
uint8_t RegisterA = 0;
uint8_t LastRegisterA = 0;
uint8_t RegisterB = 1;
uint8_t RegisterC = 0;
uint8_t OUT1 = 1;
uint8_t OUT2 = 0;
uint8_t OUT3 = 0;
int16_t OUT2_TimerCounter = 0;
uint16_t OUT2_Timer = 0;
int16_t OUT3_TimerCounter = 0;
uint16_t OUT3_Timer = 0;
uint32_t ADCRawValue = 0;
int32_t ADC_Display = 0;
int32_t DACOUT1 = 1000;
int32_t DACOUT2 = 1000;
int32_t CPV = 0;
uint32_t TotalCPV = 0;
uint8_t PVD_Flag = 0;
uint8_t KeyEnterFlag=0;
Button_STATUS KEY = ULOC;
uint8_t ConfirmShortCircuit = 0;
uint32_t ShortCircuitCounter = 0;
uint32_t ShortCircuitLastTime = 0;
uint8_t KeyLockFlag = 0;
uint8_t KeyMappingFlag = 0;
uint8_t EventFlag = 0x00;
uint8_t ShortCircuit = 0;
uint32_t ShortCircuitTimer = 0;
uint8_t displayModeONE_FLAG = 0;
uint8_t DisplayModeNo = 0;
uint8_t DisplayCPVModeNo = 0;

void SetRegisterA(uint32_t GetADCValue);
void DisplayMODE(void);
void DisplayModeONE(void);
void DisplayModeTWO(void);
void DisplayModeTHIRD(void);
void DisplayModeTotalCPV(void);
void DisplayModeFour(void);
void ShortCircuitProtection(void);
void SetOUT1Status(void);
void SetOUT2Status(void);
void SetOUT3Status(void);
void ButtonMapping(void);
void DEL_Set(void);
void DisplayModeONE_STD(void);
void DisplayModeONE_AREA(void);
void ResetParameter(void);
void Test_Delay(uint32_t ms);
uint32_t ADCDispalyProcess(uint32_t *ADC_RowValue, uint16_t Length);
uint8_t CheckDust(void);

extern int8_t PERCENTAGE;
extern int16_t ATT100;
extern uint16_t FSV;
//extern int32_t SV;
extern uint8_t SelftStudyflag;
extern int8_t DSC;
/*----------------------------------�궨��-------------------------------------*/

uint8_t DustFlag = 0;

int32_t SA_Sum = 0;
float SA_Final = 0;

uint8_t S_Index = 0;
uint8_t S_Flag = 0;
float S_Total_Final = 0;
float S_Total_SUM = 0;

int32_t SX[4];
int32_t SX_Final[32];
uint8_t SX_Flag;
uint8_t SX_Index = 0;

int32_t S_Final = 0; /*S-SET*/
uint8_t S_Final_FinishFlag = 0;

uint32_t S32 = 0;
uint8_t S32_Flag = 0;

int32_t S_SET = 0;
uint32_t S_Selft = 0;

int16_t Threshold = 1000;

int16_t DX = 0;
int16_t Last_DX = 0;
int16_t Min_DX = 0;
uint8_t DX_Flag = 1;

uint8_t TX_Index = 0;
int32_t TX = 0;
int32_t TX_Signal[8];
int16_t FX = 0;

uint32_t Display_Signal[256];
uint32_t Display_Signal_Index = 0;
uint32_t Display_Signal_Flag = 0;
uint32_t DisplayADCValue_Sum = 0;
/*ȥ�������Сֵ��ʣ�µ�������ƽ��*/
uint32_t DeleteMaxAndMinGetAverage(uint32_t *ary, uint8_t Length, uint32_t *Max, uint32_t *Min)
{
	int j;
	uint32_t Sum = 0;
	*Max = ary[0];
	*Min = ary[0];

	for (j = 1; j < Length; j++)
	{
		if (*Max < ary[j])
			*Max = ary[j];

		if (*Min > ary[j])
			*Min = ary[j];
	}

	for (j = 0; j < Length; j++)
	{
		Sum += ary[j];
	}
	//return (Sum-*Max-*Min)/(Length-2);
	return (Sum) / (Length);
}
/*���ܺ�*/
void GetSum(uint32_t *SUM, uint32_t *arry, uint8_t arryLength)
{
	int j;
	for (j = 0; j < arryLength; j++)
	{
		*SUM += arry[j];
	}
}

/*��ƽ��*/
void GetAverage(uint32_t *Average, uint32_t *arry, uint8_t arryLength)
{
	int j;
	uint32_t sum;
	for (j = 0; j < arryLength; j++)
	{
		sum += arry[j];
	}
	*Average = sum / j;
}

/*���㺯��*/
void ClearData(uint32_t *ary, uint8_t Length)
{
	int j;
	for (j = 0; j < Length; j++)
	{
		*ary = 0x00;
		ary++; /*ָ����λ*/
	}
}



uint8_t PWMCounter = 0;
uint32_t S1024 = 0;
uint16_t S1024_Index = 0;
uint32_t S1024_Sum = 0;

#define S1024_Or_S8192 1024
void JudgeDX(void)
{
	/*�жϻҲ�Ӱ��̶�*/
	if (displayModeONE_FLAG == 0)
	{
		if (RegisterA) //STD����Mode
		{

			//S1024_Sum = S1024_Sum + S_Final;
			S1024_Sum = S1024_Sum + SX_Final[SX_Index];
			S1024_Index++;
			if (S1024_Index >= S1024_Or_S8192)
			{
				S1024_Index = 0;
				S1024 = S1024_Sum / S1024_Or_S8192; //���S1024

				if (DSC)				//modifiy 20180105
					DX = S_SET - S1024; //modifiy 20171230
				else
					DX = 0;

				S1024_Sum = 0;
			}
			if (DX <= -2000) //�����ʣ�����С��-1500ʱ��û�б�����С����
				DX = -2000;
			else if (DX >= 2000)
				DX = 2000;
			//
			//			Last_DX = DX;
			//
			//			if(Last_DX<Min_DX) 	/*���ڼ�¼��С��DXֵ*/
			//				Min_DX = DX;

			//DustFlag = CheckDust(); /*�Ҳ�������أ�DUST*/
		}
	}
	else if (displayModeONE_FLAG == 1) /*Area Mode*/
	{
		if (RegisterC)
		{
			//S1024_Sum = S1024_Sum + S_Final;
			S1024_Sum = S1024_Sum + SX_Final[SX_Index];
			S1024_Index++;
			if (S1024_Index >= S1024_Or_S8192)
			{
				S1024_Index = 0;
				S1024 = S1024_Sum / S1024_Or_S8192; //���S1024
				if (DSC)							//modifiy 20180105
					DX = S_SET - S1024;				//modifiy 20171230
				else
					DX = 0;
				S1024_Sum = 0;
			}
			if (DX <= -2000) //�����ʣ�����С��-1500ʱ��û�б�����С����
				DX = -2000;
			else if (DX >= 2000)
				DX = 2000;
			//
			//			Last_DX = DX;
			//
			//			if(Last_DX<Min_DX) 	/*���ڼ�¼��С��DXֵ*/
			//				Min_DX = DX;
			//
			//				//DustFlag = CheckDust(); /*�Ҳ�������أ�DUST*/
		}
	}
}

uint16_t RunCounter[100];
uint8_t Runflag = 0;
uint8_t RunIndex = 0;
uint8_t sample_finish = 0;
extern int32_t CSV;
/*DD61-TWO-DAC funtion*/
float Max_Threshold = 0;
float Min_Threshold = 0;

uint8_t DMA_Counter = 0;
uint8_t RegisterA_1_Counter = 0;
uint8_t RegisterA_0_Counter = 0;
uint8_t TempRegisterA = 0;
int16_t DMA_ADC_Counter = 0;
uint8_t CheckCounter = 0;
int32_t ADC_Start_Counter=0;
extern uint8_t StartFLag;
uint32_t DACOriginalValue=1000;
void DMA1_Channel1_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA_IT_TC)) //�ж�DMA��������ж�
	{
		if (PVD_Flag==0 && StartFLag )
		{
				for (DMA_Counter = 0; DMA_Counter < 16;)
				{
					SA_Sum += adc_dma_tab[DMA_Counter++];
				}
				ADC_Start_Counter++;
				if(ADC_Start_Counter>50)
				{
					if (GPIO_ReadInputDataBit(COMP_OUT1_GPIO_Port, COMP_OUT1_Pin)) //�˷�Ϊ��
					{
						CheckCounter++;
						DMA_ADC_Counter++;
						SA_Final = SA_Sum / DMA_Channel;
						S_Total_SUM += SA_Final;
						SA_Sum = 0;

						if (DMA_ADC_Counter >= 4096)
						{
							DMA_ADC_Counter = 0;
							S_Total_Final = S_Total_SUM / 4096;
							S_Total_SUM = 0;
							/*��ֵ��DAC*/
							DACOUT1 = S_Total_Final + DEL;
							DAC_SetChannel1Data(DAC_Align_12b_R, (uint16_t)DACOUT1);
							DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
						}
						/*�ۼ�10��*/
						if (CheckCounter % 10 == 0 && CheckCounter!=0)
						{
							RegisterA = 0;
							CheckCounter = 0;
						}

						sample_finish = 1;
					}
					else
					{
						RegisterA = 1;
						CheckCounter = 0;
					}

					/*����OUT1��״̬*/
					SetOUT1Status();
					/*OUT2���*/
					SetOUT2Status();
		//			if (LastRegisterA == 1 && RegisterA == 0)
		//			{
		//				CPV++;
		//				if (CPV >= CSV) /*����������ﵽԤ���趨��CSV�����㣬OUT2���һ���ߵ�ƽ*/
		//				{
		//					OUT2 = 1;
		//					CPV = 0;
		//				}
		//			}
					/*��ʾOUT1��OUT2��״̬*/
					SMG_DisplayOUT_STATUS(OUT1, OUT2);
					//LastRegisterA = RegisterA;
				}
		}
	}
	DMA_ClearITPendingBit(DMA_IT_TC); //���DMA�жϱ�־λ
}


/*******************************************************************************
delay_us
*******************************************************************************/
void delayus(u32 time_us)
{
		int i=0;
		for(i=0;i<time_us*6;i++);
}

//���ADCֵ
//ch:0~3
uint16_t Get_Adc(uint8_t ch)   
{
      //ָ��ADC������ͨ��,һ������,����ʱ��
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_7Cycles5 );    //ADC1,ADCͨ��,����ʱ��Ϊ7.5����                      
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);        //ʹ��ָ����ADC1���ת����������    
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
    return ADC_GetConversionValue(ADC1);           //����ADC1��ת�����
}

uint16_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
    u32 temp_val=0;
    u8 t;
    for(t=0;t<times;t++)
    {
        temp_val+=Get_Adc(ch);
        delayus(5000);
    }
    return temp_val/times;
}

/***********************************************
�ж�RegisterA �� ��� OUT
***********************************************/
uint8_t REGISTER_AI_0 = 0;
uint8_t REGISTER_AI_1 = 0;
void SetRegisterAandOUT(void)
{
		uint8_t k=0;
		uint8_t Counter = 1;  //2019-2-27->set 3
		REGISTER_AI_1 = 0;
		REGISTER_AI_0= 0;
		GPIOA->BSRR = 0x0080;
		for(k=0;k<=Counter;k++,REGISTER_AI_0++,REGISTER_AI_1++)
		{		
			if (GPIO_ReadInputDataBit(COMP_OUT1_GPIO_Port, COMP_OUT1_Pin)) //�˷�Ϊ�ߣ�û����ͨ��
			{
				REGISTER_AI_1 = 0;
				
				if(REGISTER_AI_0>=Counter)
				{
					REGISTER_AI_0 = 0;
					RegisterA = 0;
				}
			}
			else //�˷�Ϊ�ͣ�������ͨ��
			{
				REGISTER_AI_0 = 0;
				
				if(REGISTER_AI_1>=Counter)
				{
					REGISTER_AI_1 = 0 ;
					RegisterA = 1;
				}
			}
		}
		GPIOA->BRR = 0x00080;
}


void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2)!=RESET) 
	{
		SetRegisterAandOUT();
		EXTI_ClearITPendingBit(EXTI_Line2); 
	}
}

/*��Ҫ�Ĵ�����*/
uint32_t timenum;
extern uint32_t KeytempPress;
uint16_t DAC_OUTput=0;
uint8_t CPV_DispalyFlag = 0;
int16_t RegisterA_clearTime = 0;
void Main_Function(void)
{
	GetEEPROM();
	
	//DACOriginalValue = Get_Adc_Average(ADC_Channel_3,8);
	DACOriginalValue = 950;
	DACOUT1 = DACOriginalValue + DEL;
	DAC_SetChannel1Data(DAC_Align_12b_R, (uint16_t)DACOUT1);
	
	while (1)
	{
		if(PVD_Flag==0)
		{
			timenum++;
			//GPIOA->BSRR = 0x0080;
			/**************************************/
			/*�ж�RegisterA �� ��� OUT*/
			//SetRegisterAandOUT();  //�˺�������ʱ������20us  // 2019-2-23 change to exti mode to check
			/*�ȴ�10us*/
			//delayus(10);
						/*����OUT1��״̬*/
			SetOUT1Status();
			/*OUT2���*/
			SetOUT2Status();
			/*��ʾOUT1��OUT2��״̬*/
			SMG_DisplayOUT_STATUS(OUT1, OUT2);	
			
			/**************************************/
			/*������ʾģʽ*/
			
			DisplayMODE();
			/*��������*/
			ButtonMapping();

			if (KEY == ULOC) /*�жϰ����Ƿ�����*/
			{
				/*Mode�˵�ģʽ*/
				menu();
			}
			//���ϴ�delayus(10)��ʼ������ʱ������12us
			/*�ȴ�10us*/
			//delayus(8);
			/**************************************/
			//���ϴ�while(1)��ʼ������ʱ������40us
			if(RegisterA ==1)
			{
				if(timenum % 2 == 0)						
					RegisterA_clearTime++;
				if(RegisterA_clearTime>=5)  //5*40 =  200us 
				{
					RegisterA = 0;
					RegisterA_clearTime = 0;
				}
			}
			if(timenum % 3 == 0)						// 120/40 = 3  120us
			{
				OUT1_Mode.DelayCounter++;
			}
			if(timenum % 25 == 0)					// 1,000/40 = 25  1ms
			{
				if(OUT2)
					OUT2_TimerCounter++;
				SMG_Diplay();  /*ˢ�������*/
			}
			if (timenum % 200 == 0) // 8000/40 = 200  8ms
			{
				Key_Scan(); //��ʱɨ�谴��
				KeytempPress = 1;
				IWDG_ReloadCounter();
			}
			if (timenum >= 12500)  // 500,000/40 = 12500  500ms
			{
				EventFlag = EventFlag | Blink500msFlag;
				timenum = 0;
			}
			//���ϴ�while(1)��ʼ������ʱ������27us
			//GPIOA->BRR = 0x00080;
		}
	}
}


/*******************************
*
*��ʾģʽ�л�
*
*******************************/

void DisplayMODE(void)
{		
		if ( KeyEnterFlag==0)
		{
			if((KEY == LOC &&ModeButton.Status == Release && DownButton.Status == Release && UpButton.Status == Release)||KEY==ULOC)
			{
				/***************ѡ��***********
				2019-2-23 origin versin
				******************************/
//				if (ModeButton.PressCounter == 0)
//				{
//					DisplayCPVModeNo = 0;
//				}
//				else if (ModeButton.Effect == PressShort && ModeButton.PressCounter == 1 && DownButton.Status == Release && UpButton.Status == Release)
//				{
//					DisplayCPVModeNo = 1;
//				}
//				else if (ModeButton.Effect == PressShort && ModeButton.PressCounter == 2 && DownButton.Status == Release && UpButton.Status == Release ) //if need to display Mode_Four,PressCounter=4
//				{
//					ModeButton.PressCounter = 0;
//				}				
				/*****************��ʾ****************/
				if(DisplayCPVModeNo==0)
				{
					DisplayModeTHIRD();
				}
				else if(DisplayCPVModeNo==1)
				{
					DisplayModeTotalCPV();
				}
			}
			else if (KEY == LOC && (ModeButton.Status == Press || DownButton.Status == Press || UpButton.Status == Press))
			{
				ButtonMappingDisplay(1);
			}
		}
		else
		{

				
		}
}

/*******************************
*
*��ʾģʽ1
*
*******************************/
int8_t LastDSC;
void DisplayModeONE(void)
{
	if (displayModeONE_FLAG)
	{
		if (DSC)
		{
			LastDSC = DSC;
			DSC = 0; //����ģʽ�£�DXһֱΪ0
		}
		DisplayModeONE_AREA();
	}
	else
	{
		if (LastDSC)
		{
			LastDSC = 0;
			DSC = 1;
		}
		DisplayModeONE_STD();
	}
}

/*******************************
*
*��ʾģʽ1_DETECT_STD
*
*******************************/
uint32_t KeytempPress = 0;
void DisplayModeONE_STD(void)
{
	static uint8_t lastCounter;
	static int16_t LastThreshold;

	/*�������ʾ*/
	SMG_DisplayModeONE(Threshold, ADC_Display);

	if (ModeButton.Status == Release && KeyLockFlag == 0 && KEY == ULOC)
	{
		/*Up Button*/
		LastThreshold = Threshold;
		if (UpButton.PressCounter != lastCounter && UpButton.Effect == PressShort)
		{
			lastCounter = UpButton.PressCounter;
			UpButton.PressCounter = 0;
			Threshold = Threshold + 1;
		}
		else if (UpButton.Status == Press && (UpButton.Effect == PressLong))
		{ /*�����Ű���������ʱ�䳬������ʱ��*/
			UpButton.PressCounter = 0;
			if (UpButton.PressTimer <= KEY_LEVEL_1)
			{
				if (UpButton.PressTimer % KEY_LEVEL_1_SET == 0 && KeytempPress == 1)
				{
					Threshold = Threshold + 1;
					KeytempPress = 0;
				}
			}
			else if (UpButton.PressTimer > KEY_LEVEL_1 && UpButton.PressTimer <= KEY_LEVEL_2)
			{
				if (UpButton.PressTimer % KEY_LEVEL_2_SET == 0 && KeytempPress == 1)
				{
					KeytempPress = 0;
					Threshold = Threshold + 2;
				}
			}
			else
			{
				if (UpButton.PressTimer % KEY_LEVEL_3_SET == 0 && KeytempPress == 1)
				{
					KeytempPress = 0;
					Threshold = Threshold + 5;
				}
			}
		}
		else
		{
			UpButton.Effect = PressShort;
		}
		/*Down Button*/
		if (DownButton.PressCounter != lastCounter && DownButton.Effect == PressShort)
		{
			DownButton.PressCounter = 0;
			Threshold = Threshold - 1;
		}
		else if (DownButton.Status == Press && (DownButton.Effect == PressLong))
		{
			DownButton.PressCounter = 0;
			if (DownButton.PressTimer < KEY_LEVEL_1)
			{
				if (DownButton.PressTimer % KEY_LEVEL_1_SET == 0 && KeytempPress == 1)
				{
					Threshold = Threshold - 1;
					KeytempPress = 0;
				}
			}
			else if (DownButton.PressTimer > KEY_LEVEL_1 && DownButton.PressTimer < KEY_LEVEL_2)
			{
				if (DownButton.PressTimer % KEY_LEVEL_2_SET == 0 && KeytempPress == 1)
				{
					Threshold = Threshold - 2;
					KeytempPress = 0;
				}
			}
			else
			{
				if (DownButton.PressTimer % KEY_LEVEL_3_SET == 0 && KeytempPress == 1)
				{
					KeytempPress = 0;
					Threshold = Threshold - 5;
				}
			}
		}
		else
		{
			DownButton.Effect = PressShort;
		}
		if (LastThreshold != Threshold && DownButton.Status == Release && UpButton.Status == Release)
		{
			WriteFlash(Threshold_FLASH_DATA_ADDRESS, Threshold);
		}
	}
	if (Threshold >= 9999)
		Threshold = 9999;
	else if (Threshold <= 0)
		Threshold = 0;
}

/*******************************
*
*��ʾģʽ1_DETECT_AREA
*
*******************************/
void DisplayModeONE_AREA(void)
{
	static uint8_t lastCounter;
	static int16_t LastHIValue;
	static int16_t LastLOValue;

	/*HI display mode*/
	if (DisplayModeNo == 0)
	{
		if (ModeButton.Status == Release && KeyLockFlag == 0 && KEY == ULOC)
		{
			/*Up Button*/
			LastHIValue = HI;
			if (UpButton.PressCounter != lastCounter && UpButton.Effect == PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				HI = HI + 1;
				if (HI >= 9999)
					HI = 9999;
				SMG_DisplayModeONE_Detect_AREA_HI(1, HI, ADC_Display); /*��ʾ��ֵ*/
			}
			else if (UpButton.Status == Press && (UpButton.Effect == PressLong))
			{ /*�����Ű���������ʱ�䳬������ʱ��*/
				UpButton.PressCounter = 0;
				if (UpButton.PressTimer <= KEY_LEVEL_1)
				{
					if (UpButton.PressTimer % KEY_LEVEL_1_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						HI = HI + 1;
					}
				}
				else if (UpButton.PressTimer > KEY_LEVEL_1 && UpButton.PressTimer <= KEY_LEVEL_2)
				{
					if (UpButton.PressTimer % KEY_LEVEL_2_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						HI = HI + 2;
					}
				}
				else
				{
					if (UpButton.PressTimer % KEY_LEVEL_3_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						HI = HI + 5;
					}
				}
				if (HI >= 9999)
					HI = 9999;
				SMG_DisplayModeONE_Detect_AREA_HI(1, HI, ADC_Display); /*��ʾ��ֵ*/
			}
			//					else
			//					{
			//						UpButton.Effect = PressShort;
			//						SMG_DisplayModeONE_Detect_AREA_HI(timeflag,HI,ADC_Display); /*������ʾHI�뷧ֵ*/
			//					}
			/*Down Button*/
			else if (DownButton.PressCounter != lastCounter && DownButton.Effect == PressShort)
			{
				DownButton.PressCounter = 0;
				HI = HI - 1;
				if (HI <= 0)
					HI = 0;
				SMG_DisplayModeONE_Detect_AREA_HI(1, HI, ADC_Display); /*��ʾ��ֵ*/
			}
			else if (DownButton.Status == Press && (DownButton.Effect == PressLong))
			{
				DownButton.PressCounter = 0;
				if (DownButton.PressTimer < KEY_LEVEL_1)
				{
					if (DownButton.PressTimer % KEY_LEVEL_1_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						HI = HI - 1;
					}
				}
				else if (DownButton.PressTimer > KEY_LEVEL_1 && DownButton.PressTimer < KEY_LEVEL_2)
				{
					if (DownButton.PressTimer % KEY_LEVEL_2_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						HI = HI - 2;
					}
				}
				else
				{
					if (DownButton.PressTimer % KEY_LEVEL_3_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						HI = HI - 5;
					}
				}
				if (HI <= 0)
					HI = 0;
				SMG_DisplayModeONE_Detect_AREA_HI(1, HI, ADC_Display); /*��ʾ��ֵ*/
			}
			else
			{
				DownButton.Effect = PressShort;
				SMG_DisplayModeONE_Detect_AREA_HI(timeflag, HI, ADC_Display); /*������ʾHI�뷧ֵ*/
			}
			if (LastHIValue != HI && DownButton.Status == Release && UpButton.Status == Release)
			{
				//WriteFlash(HI_FLASH_DATA_ADDRESS,HI);
			}
		}
	}

	/*LO display mode*/
	else if (DisplayModeNo == 1)
	{
		if (ModeButton.Status == Release && KeyLockFlag == 0 && KEY == ULOC)
		{
			/*Up Button*/
			LastLOValue = LO;
			if (UpButton.PressCounter != lastCounter && UpButton.Effect == PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				LO = LO + 1;
				if (LO >= 9999)
					LO = 9999;
				SMG_DisplayModeONE_Detect_AREA_LO(1, LO, ADC_Display); /*��ʾ��ֵ*/
			}
			else if (UpButton.Status == Press && (UpButton.Effect == PressLong))
			{ /*�����Ű���������ʱ�䳬������ʱ��*/
				UpButton.PressCounter = 0;
				if (UpButton.PressTimer <= KEY_LEVEL_1)
				{
					if (UpButton.PressTimer % KEY_LEVEL_1_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						LO = LO + 1;
					}
				}
				else if (UpButton.PressTimer > KEY_LEVEL_1 && UpButton.PressTimer <= KEY_LEVEL_2)
				{
					if (UpButton.PressTimer % KEY_LEVEL_2_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						LO = LO + 2;
					}
				}
				else
				{
					if (UpButton.PressTimer % KEY_LEVEL_3_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						LO = LO + 5;
					}
				}
				if (LO >= 9999)
					LO = 9999;
				SMG_DisplayModeONE_Detect_AREA_LO(1, LO, ADC_Display); /*��ʾ��ֵ*/
			}
			//					else
			//					{
			//						UpButton.Effect = PressShort;
			//						SMG_DisplayModeONE_Detect_AREA_LO(timeflag,LO,ADC_Display);/*������ʾLO�뷧ֵ*/
			//					}
			/*Down Button*/
			else if (DownButton.PressCounter != lastCounter && DownButton.Effect == PressShort)
			{
				DownButton.PressCounter = 0;
				LO = LO - 1;
				if (LO <= 0)
					LO = 0;
				SMG_DisplayModeONE_Detect_AREA_LO(1, LO, ADC_Display); /*��ʾ��ֵ*/
			}
			else if (DownButton.Status == Press && (DownButton.Effect == PressLong))
			{
				DownButton.PressCounter = 0;
				if (DownButton.PressTimer < KEY_LEVEL_1)
				{
					if (DownButton.PressTimer % KEY_LEVEL_1_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						LO = LO - 1;
					}
				}
				else if (DownButton.PressTimer > KEY_LEVEL_1 && DownButton.PressTimer < KEY_LEVEL_2)
				{
					if (DownButton.PressTimer % KEY_LEVEL_2_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						LO = LO - 2;
					}
				}
				else
				{
					if (DownButton.PressTimer % KEY_LEVEL_3_SET == 0 && KeytempPress == 1)
					{
						KeytempPress = 0;
						LO = LO - 5;
					}
				}
				if (LO <= 0)
					LO = 0;
				SMG_DisplayModeONE_Detect_AREA_LO(1, LO, ADC_Display); /*��ʾ��ֵ*/
			}
			else
			{
				DownButton.Effect = PressShort;
				SMG_DisplayModeONE_Detect_AREA_LO(timeflag, LO, ADC_Display); /*������ʾLO�뷧ֵ*/
			}
			if (LastLOValue != LO && DownButton.Status == Release && UpButton.Status == Release)
			{
				//WriteFlash(LO_FLASH_DATA_ADDRESS,LO);
			}
		}
	}
}

/*******************************
*
*��ʾģʽ2
*
*******************************/
void DisplayModeTWO(void)
{
	static uint8_t lastCounter;
	/*�������ʾ*/
	SMG_DisplayModeTWO(RegisterB);

	/*Up Button*/
	if (UpButton.PressCounter != lastCounter && UpButton.Effect == PressShort)
	{
		lastCounter = UpButton.PressCounter;
		UpButton.PressCounter = 0;
		if (RegisterB == 0)
			RegisterB = 1;
		else
			RegisterB = 0;
		WriteFlash(RegisterB_FLASH_DATA_ADDRESS, RegisterB);
	}

	/*Down Button*/
	if (DownButton.PressCounter != lastCounter && DownButton.Effect == PressShort)
	{
		DownButton.PressCounter = 0;
		if (RegisterB == 0)
			RegisterB = 1;
		else
			RegisterB = 0;
		WriteFlash(RegisterB_FLASH_DATA_ADDRESS, RegisterB);
	}
}

/*******************************
*
*��ʾģʽ3
*
*******************************/
void DisplayModeTHIRD(void)
{
	/*�������ʾ*/
	SMG_DisplayModeTHIRD(CPV);
	/*����Ϊ���������������ֹ��Ӱ�쵽��ʾģʽ4*/
	DownButton.PressCounter = 0;
	UpButton.PressCounter = 0;
}

/*******************************
*
*��ʾģʽ4
*
*******************************/
void DisplayModeTotalCPV(void)
{
	/*�������ʾ*/
	SMG_DisplayModeTotalCPV(TotalCPV);
	/*����Ϊ���������������ֹ��Ӱ�쵽��ʾģʽ4*/
	DownButton.PressCounter = 0;
	UpButton.PressCounter = 0;
}

/*******************************
*
*Set RegisterA value
*
*******************************/
void SetRegisterA(uint32_t ADCTestValue)
{
	//	TX = 0; //debug
	//	DX = 0;
	if (displayModeONE_FLAG) /*AREA Mode*/
	{
		if (ADCTestValue >= LO + TX && ADCTestValue <= HI - TX - 80 - HI / 128)
			RegisterA = 1;
		else if (((ADCTestValue >= (HI + TX)) && (ADCTestValue <= 9999)) || (ADCTestValue <= (LO - TX - 80 - LO / 128))) //20180106
			RegisterA = 0;

		/*RegisterC*/
		if (ADCTestValue >= HI + TX)
			RegisterC = 1;
		else if (ADCTestValue <= HI - TX - HI / 128)
			RegisterC = 0;
	}
	else /*STD Mode*/
	{
		if (ADCTestValue >= Threshold + TX) //20171231
			RegisterA = 1;
		else if (ADCTestValue <= (Threshold - TX - 40 - Threshold / 256)) /*20171223*/ //2018-1-5 �޸ĳ�-80,//20180106 �ĳ�-40 /256
			RegisterA = 0;
	}
}

/*******************************
*
*�ж�OUT1�����״̬
*
*******************************/
uint8_t SHOTflag = 0;
uint8_t CPV_Status=0;
void SetOUT1Status(void)
{
	if (ShortCircuit != 1) /*���Ƕ�·����������²��ж�OUT1�����*/
	{
		/*ͬ������*/

		if (GPIO_ReadInputDataBit(BUTTON_SWITCH_GPIO_Port, BUTTON_SWITCH_Pin))
			RegisterB = 0;
		else
			RegisterB = 1;

		OUT1 = !(RegisterB ^ RegisterA);
		
		if (OUT1_Mode.DelayMode == TOFF)
		{
			//GPIOA->ODR ^= GPIO_Pin_9;
			if (OUT1 == 0)
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
				OUT1_Mode.DelayCounter = 0;
				CPV_Status = 1;
			}
			else
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
				OUT1_Mode.DelayCounter = 0;
			}
		}
		/*ON_D*/
		else if (OUT1_Mode.DelayMode == OFFD)
		{
			if (OUT1 == 0)
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
				OUT1_Mode.DelayCounter = 0;
				
			}
			else
			{
				if (OUT1_Mode.DelayCounter > (OUT1_Mode.DelayValue * DealyBaseTime))
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
					CPV_Status = 1;
				}
			}
		}
		/*OFFD*/
		else if (OUT1_Mode.DelayMode == ON_D)  //ZLJF�汾��OFF��NO���Ƿ��ˣ��������OFF delay
		{
			if (OUT1 == 0)
			{
				if (OUT1_Mode.DelayCounter > (OUT1_Mode.DelayValue * DealyBaseTime))
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
					CPV_Status = 1;
				}
			}
			else
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
				OUT1_Mode.DelayCounter = 0;
			}
		}
		/*SHOT*/
		else if (OUT1_Mode.DelayMode == SHOT)
		{
			if (OUT1 == 0)
			{
				if (OUT1_Mode.DelayCounter > (OUT1_Mode.DelayValue * DealyBaseTime))
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
					CPV_Status = 1;
				}
			}
			else
			{
				OUT1_Mode.DelayCounter = 0;
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
			}
		}
			if(LastOUT1 == 0 && OUT1 == 1 && CPV_Status == 1)
			{
				CPV++;
				CPV_Status = 0;
				TotalCPV++;
				if(TotalCPV>=999999) TotalCPV = 0;
				if (CPV >= CSV) /*����������ﵽԤ���趨��CSV�����㣬OUT2���һ���ߵ�ƽ*/
				{
					OUT2 = 1;
					CPV = 0;
				}
			}		
			LastOUT1 = OUT1;
	}
}
/*******************************
*
*�ж�OUT2�����״̬
*
*******************************/
void SetOUT2Status(void)
{
	if (ShortCircuit != 1) /*���Ƕ�·����������²��ж�OUT2�����*/
	{
		if (OUT2)
		{
			GPIO_WriteBit(OUT2_GPIO_Port, OUT2_Pin, Bit_SET); /*����*/
		}
		if (OUT2_TimerCounter >= 80)
		{
			OUT2 = 0;
			OUT2_TimerCounter = 0;								/*��ȡ��ǰʱ��*/
			GPIO_WriteBit(OUT2_GPIO_Port, OUT2_Pin, Bit_RESET); /*80ms������*/
		}
	}
}

/*******************************
*
*�ж�OUT3�����״̬
*
*******************************/
void SetOUT3Status(void)
{
	if (ShortCircuit != 1) /*���Ƕ�·����������²��ж�OUT2�����*/
	{
		if (OUT3)
		{
			//GPIO_WriteBit(OUT3_GPIO_Port,OUT3_Pin,Bit_SET);/*����*/
		}
		if (OUT3_TimerCounter >= 160)
		{
			OUT3 = 0;
			OUT3_TimerCounter = 0; /*��ȡ��ǰʱ��*/
								   //GPIO_WriteBit(OUT3_GPIO_Port,OUT3_Pin,Bit_RESET);/*80ms������*/
		}
	}
}

/*******************************
*
*��·����
*
*******************************/
void ShortCircuitProtection(void)
{
	uint8_t SCState;

	/*��ȡSC���ŵ�״̬*/
	if (ShortCircuit != 1)
	{
		//SCState = GPIO_ReadInputDataBit(SC_GPIO_Port ,SC_Pin);
		if ((BitAction)SCState == Bit_RESET)
		{
			/*����FB_SC*/
			ShortCircuit = 1;
		}
		else
		{
			ShortCircuit = 0;
			ConfirmShortCircuit = 0;
		}
	}
	if (ShortCircuit && ShortCircuitCounter >= 100)
	{
		ConfirmShortCircuit = 1;
		GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
		GPIO_WriteBit(OUT2_GPIO_Port, OUT2_Pin, Bit_RESET); /*��������OUT*/
		//GPIO_WriteBit(OUT3_GPIO_Port,OUT3_Pin,Bit_RESET);/*��������OUT*/
		ShortCircuitTimer = ShortCircuitLastTime;
	}
}

/*******************************
*
*��������
*
*******************************/

void ButtonMapping(void)
{
	/*��������*/
	if (KeyEnterFlag ==0&& ModeButton.PressTimer >= ModeButton.LongTime && ModeButton.Status == Press && DownButton.Effect == PressLong && DownButton.Status == Press &&UpButton.Status == Release )
	{
		KeyEnterFlag = 1;
		if (KEY == ULOC)
			KEY = LOC;
		else
			KEY = ULOC;
		ModeButton.PressCounter = 0;
		if (ModeButton.Effect == PressLong && ModeButton.Status == Press && DownButton.Effect == PressLong && DownButton.Status == Press &&UpButton.Status == Release )
		{
			ButtonMappingDisplay(1);
			KeyLockFlag = 1;
		}
		WriteFlash(KEY_FLASH_DATA_ADDRESS, KEY);
		ModeButton.Effect = PressShort;
		ModeButton.PressCounter = 0;
		DownButton.PressCounter = 0;
		//DownButton.Effect = PressNOEffect;
	}
	/*�����ʼ��*/
	else if (KEY == ULOC && KeyEnterFlag ==0&&ModeButton.PressTimer >= ModeButton.LongTime && ModeButton.Status == Press &&UpButton.Effect == PressLong && UpButton.Status == Press && DownButton.Effect == PressLong && DownButton.Status == Press)
	{
		KeyEnterFlag = 2;
		takeoffLED();
		if ((ReadButtonStatus(&ModeButton)) == Press && (ReadButtonStatus(&UpButton) == Press) && (ReadButtonStatus(&DownButton) == Press))
		{
			EraseFlash();
			if ((ReadButtonStatus(&ModeButton)) == Press && (ReadButtonStatus(&UpButton) == Press) && (ReadButtonStatus(&DownButton) == Press))
			{
				ButtonMappingDisplay(2);
				//testflag = 3;
			}
		}

		ResetParameter();

		ModeButton.PressCounter = 0;
		//ModeButton.Effect = PressNOEffect;
		DownButton.PressCounter = 0;
		//DownButton.Effect = PressNOEffect;
		UpButton.PressCounter = 0;
		//UpButton.Effect = PressNOEffect;
		Test_Delay(1000);
	}
	/*����������*/
	else if (KEY == ULOC && KeyEnterFlag ==0&&ModeButton.PressTimer >= ModeButton.LongTime && ModeButton.Status == Press && UpButton.Effect == PressLong && UpButton.Status == Press &&DownButton.Status == Release )
	{
		KeyEnterFlag = 3;
		if(DisplayCPVModeNo == 0)
		{
			CPV = 0;
			ModeButton.PressCounter = 1;
		}
		else if (DisplayCPVModeNo == 1)
		{
			TotalCPV = 0;
			ModeButton.PressCounter = 0;
		}
		if (ModeButton.Effect == PressLong && ModeButton.Status == Press && UpButton.Effect == PressLong && UpButton.Status == Press &&DownButton.Status == Release )
		{
			ButtonMappingDisplay(3);
			//testflag = 4;
		}
		//ModeButton.Effect = PressNOEffect;
		ModeButton.PressTimer = 0;
		
		UpButton.PressCounter = 0;
		//UpButton.Effect = PressNOEffect;
		Test_Delay(1000);
	}
	else if (ModeButton.Status == Release && DownButton.Status == Release && UpButton.Status == Release)
	{
		KeyEnterFlag = 0;
	}
	
	if(KeyEnterFlag==1) 
		ButtonMappingDisplay(1);
	else if(KeyEnterFlag==2)  
		ButtonMappingDisplay(2);
	else if(KeyEnterFlag==3)  
		ButtonMappingDisplay(3);
}

void Test_Delay(uint32_t ms)
{
	uint32_t i;

	/*��
		CPU��Ƶ168MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ5ʱ��SCLƵ�� = 1.78MHz (����ʱ: 92ms, ��д������������ʾ����̽ͷ���ϾͶ�дʧ�ܡ�ʱ��ӽ��ٽ�)
		ѭ������Ϊ10ʱ��SCLƵ�� = 1.1MHz (����ʱ: 138ms, ���ٶ�: 118724B/s)
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��1.0us��SCL�͵�ƽʱ��1.2us

		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us

		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	for (i = 0; i < ms * 100; i++)
		;
}
/*******************************
*
*��ȡEEPROM����
*
*******************************/

//	uint32_t _test_send_buf[30];
//	uint32_t _test_recv_buf[30];
//	short _test_size = 30;

//	char statusW=2;
//	char statusR=2;

uint32_t ProgramRUNcounter = 0;

void GetEEPROM(void)
{

	OUT1_Mode.DelayMode = ReadFlash(OUT1_Mode_FLASH_DATA_ADDRESS);
	OUT1_Mode.DelayValue = ReadFlash(OUT1_Value_FLASH_DATA_ADDRESS);
	CSV = ReadFlash(CSV_FLASH_DATA_ADDRESS);
	Threshold = ReadFlash(Threshold_FLASH_DATA_ADDRESS);
	DACOUT1 = ReadFlash(DACOUT1_FLASH_DATA_ADDRESS);
	KEY = ReadFlash(KEY_FLASH_DATA_ADDRESS);
	RegisterB = ReadFlash(RegisterB_FLASH_DATA_ADDRESS);
	DEL = ReadFlash(DEL_FLASH_DATA_ADDRESS);
	SA_MaxValue = ReadFlash(SA_MAX_FLASH_DATA_ADDRESS);
	SB_MaxValue = ReadFlash(SB_MAX_FLASH_DATA_ADDRESS);
	displayModeONE_FLAG = ReadFlash(DETECT_FLASH_DATA_ADDRESS);
	PERCENTAGE = ReadFlash(PERCENTAGE_FLASH_DATA_ADDRESS);
	S_SET = ReadFlash(S_SET_FLASH_DATA_ADDRESS);
	DSC = ReadFlash(DSC_FLASH_DATA_ADDRESS);

}

/*****************************
*
*��ʼ�����в���
*
****************************/
void ResetParameter(void)
{

	Threshold = 1000;
	KEY = ULOC;
	OUT1_Mode.DelayMode = TOFF;
	OUT1_Mode.DelayValue = 10;
	ATT100 = 100;
	DEL = 300;
	RegisterB = 1;
	HI = 1000;
	LO = 700;
	displayModeONE_FLAG = 0;
	PERCENTAGE = 1;
	DSC = 1;

	WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS, OUT1_Mode.DelayMode);
	Test_Delay(50);
	WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS, OUT1_Mode.DelayValue);
	Test_Delay(50);
	WriteFlash(CSV_FLASH_DATA_ADDRESS, CSV);
	Test_Delay(50);
	WriteFlash(Threshold_FLASH_DATA_ADDRESS, Threshold);
	Test_Delay(50);
	WriteFlash(DACOUT1_FLASH_DATA_ADDRESS, DACOUT1);
	Test_Delay(50);
	WriteFlash(KEY_FLASH_DATA_ADDRESS, KEY);
	Test_Delay(50);
	WriteFlash(RegisterB_FLASH_DATA_ADDRESS, RegisterB);
	Test_Delay(50);
	WriteFlash(DEL_FLASH_DATA_ADDRESS, DEL);
	Test_Delay(50);
	WriteFlash(SA_MAX_FLASH_DATA_ADDRESS, SA_MaxValue);
	Test_Delay(50);
	WriteFlash(SB_MAX_FLASH_DATA_ADDRESS, SB_MaxValue);
	Test_Delay(50);
	WriteFlash(DETECT_FLASH_DATA_ADDRESS, displayModeONE_FLAG);
	Test_Delay(50);
	WriteFlash(PERCENTAGE_FLASH_DATA_ADDRESS, PERCENTAGE);
	Test_Delay(50);
	WriteFlash(S_SET_FLASH_DATA_ADDRESS, S_SET);
	Test_Delay(50);
	WriteFlash(DSC_FLASH_DATA_ADDRESS, DSC);

	ModeButton.Effect = PressNOEffect;
	ModeButton.PressTimer = 0;
	ModeButton.PressCounter = 0;
	SetButton.Effect = PressNOEffect;
	SetButton.PressCounter = 0;
}
