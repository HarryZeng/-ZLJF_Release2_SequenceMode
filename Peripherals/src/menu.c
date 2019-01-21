///**
//  ********************************  STM32F0x1  *********************************
//  * @�ļ���     �� menu.c
//  * @����       �� HarryZeng
//  * @��汾     �� V1.5.0
//  * @�ļ��汾   �� V1.0.0
//  * @����       �� 2017��05��15��
//  * @ժҪ       �� ���ݴ���
//  ******************************************************************************/
///*----------------------------------------------------------------------------
//  ������־:
//  2017-05-15 V1.0.0:��ʼ�汾
//  ----------------------------------------------------------------------------*/
///* ������ͷ�ļ� --------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "project.h"
#include "menu.h"
#include "key.h"
#include "display.h"
#include "flash.h"
#include "selfstudy.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_iwdg.h"

extern uint16_t DEL;
int32_t CSV = 1000;
uint16_t FSV = 1000;	/*FINAL SET VALUE*/
int16_t ATT100=100;
int8_t PERCENTAGE=1;
int8_t DSC=1;

#define TimerDisplayIndex   4

OUT1_DELAY_MODE_STRUCT OUT1_Mode={TOFF,10};
int8_t DispalyNo=0;
/*CSV�˵�*/
void MenuOne_CSV(void);
/*DSC  �˵�*/
void Menu_DSC(void);
/*ATT100�趨�˵�*/
void MenuOne_ATT100(void);
/*PERCENTAGE�趨�˵�*/
void Menu_PERCENTAGE(void);
/*FSV COUNTER�趨�˵�*/
void MenuOne_FSV(void);
void MenuTwo_DEL(void);
//void MenuOne_SV(void);
void MenuTwo_OUT1_DelaySET(void);
void MenuTwo_OUT1_TOFF(void);
void MenuTwo_OUT1_OFFD(void);
void MenuTwo_OUT1_ON_D(void);	
void MenuTwo_OUT1_SHOT(void);
extern uint8_t  displayModeONE_FLAG;
extern uint32_t KeytempPress;
extern uint8_t DX_Flag;
extern int16_t 	DX;
extern uint32_t DACOriginalValue;
uint8_t MenuFlashflag = 0;

uint32_t key_time=0;

void menu(void)
{
	static uint8_t lastCounter;
	/*Mode������һ�ΰ���,��������ʱ��*/
	if(ModeButton.Status ==Press &&(DownButton.Status==Press||UpButton.Status==Press))
	{
		
	}
	else
	{
		while((ModeButton.Status==Press&&(ModeButton.PressTimer>=ModeButton.LongTime)) && UpButton.Status == Release && DownButton.Status==Release)
		{
				DX_Flag = 0;
				DX = 0;
				ModeButton.PressCounter = 1;
				UpButton.PressCounter = 0;
				MenuTwo_DEL();
				   
				while(ModeButton.PressCounter==1)
				{
					MenuTwo_DEL();
				}	
//				
				while(ModeButton.PressCounter==2)
				{
					MenuOne_CSV();
				}
				
				/*��ʾ��ǰ��ʱ��ѡ���˵�*/
				if(ModeButton.PressCounter==3 && ModeButton.Effect==PressShort)
				{
					if(OUT1_Mode.DelayMode == TOFF)
						DispalyNo = 0;
					else if(OUT1_Mode.DelayMode == OFFD)
						DispalyNo = 3;
					else if(OUT1_Mode.DelayMode == ON_D)
						DispalyNo = 1;
					else if(OUT1_Mode.DelayMode == SHOT)
						DispalyNo = 2;
				}
				while(ModeButton.Effect==PressShort && ModeButton.PressCounter==3)
				{
						MenuTwo_OUT1_DelaySET();
				}
				while(ModeButton.Effect==PressShort && ModeButton.PressCounter<=7 && ModeButton.PressCounter>=6)
				{
						END_Display();
						DX_Flag = 1;
						/*�ٶ̰�MODE�����˳��˵�*/
						if(ModeButton.Effect==PressShort && ModeButton.PressCounter>=7) 
						{
							ModeButton.PressCounter = 0;
							ModeButton.Status = Release;
							WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS,OUT1_Mode.DelayMode);
							WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS,OUT1_Mode.DelayValue);
							break;
						}
						/*��END_Display();���ˣ�����ʱ����2us*/
						key_time++;
						if(key_time % 500 == 0)					// 1,000/2 = 500  1ms
						{
							SMG_Diplay();  /*ˢ�������*/
						}
						if (key_time % 4000 == 0) // 8000/2 = 4000  8ms
						{
							Key_Scan(); //��ʱɨ�谴��
							KeytempPress = 1;
							IWDG_ReloadCounter();
						}
						if (key_time >= 250000)  // 500,000/2 = 250,000  500ms
						{
							EventFlag = EventFlag | Blink500msFlag;
							key_time = 0;
						}
				}
		}
	}
}
/*CSV�趨�˵�*/
void MenuOne_CSV(void)
{
	static uint8_t lastCounter;
	
	//GPIOA->BSRR = 0x0080;
	SMG_DisplaCSV(CSV);
	/*Up Button*/
	if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
	{
		lastCounter = UpButton.PressCounter;
		UpButton.PressCounter = 0;
		CSV = CSV+1;
		MenuFlashflag = 1;
	}
	else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		UpButton.PressCounter = 0;
		if(UpButton.PressTimer<KEY_LEVEL_1)
		{
			if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				CSV = CSV+1;
				MenuFlashflag = 1;
			}
		}
		else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
		{
			if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				CSV = CSV+2;
				MenuFlashflag = 1;
			}
		}
		else 
		{
			if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				CSV = CSV+10;
				MenuFlashflag = 1;
			}
		}
	}	
	else
	{
		UpButton.Effect = PressShort;
	}
	
	/*Down Button*/
	if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
	{
		DownButton.PressCounter = 0;
		CSV = CSV-1;
		MenuFlashflag = 1;
	}
	else 	if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		DownButton.PressCounter = 0;
		if(DownButton.PressTimer<KEY_LEVEL_1)
		{
			if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				CSV = CSV-1;
				MenuFlashflag = 1;
			}
		}
		else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
		{
			if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				CSV = CSV-2;
				MenuFlashflag = 1;
			}
		}
		else 
		{
			if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				CSV = CSV-10;
				MenuFlashflag = 1;
			}
		}
	}
	else
	{
		DownButton.Effect = PressShort;
		if(EventFlag&Blink500msFlag && MenuFlashflag==1)
		{
			MenuFlashflag = 0;
			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
			WriteFlash(CSV_FLASH_DATA_ADDRESS,CSV);
		}
	}
	
	if(CSV<=1)
			CSV =1;
	else if(CSV>=999999)
			CSV =999999;

	/*��SMG_DisplayMenuTwo_DEL_SET(DEL,0)���ˣ�����ʱ����9us*/
		//GPIOA->BRR = 0x00080;
		key_time++;
		if(key_time % 112 == 0)					// 1,000/9 = 111.111  1ms
		{
			SMG_Diplay();  /*ˢ�������*/
		}
		if (key_time % 888 == 0) // 8000/9 = 888.888  8ms
		{
			Key_Scan(); //��ʱɨ�谴��
			KeytempPress = 1;
			IWDG_ReloadCounter();
		}
		if (key_time >= 55555)  // 500,000/9 = 55555.555  500ms
		{
			EventFlag = EventFlag | Blink500msFlag;
			key_time = 0;
		}	
	
}

/*ATT100�趨�˵�*/
extern uint32_t ADC_Display;//ADC��ʾ
void MenuOne_ATT100(void)
{
	static uint8_t lastCounter;
	uint8_t Flashflag=0;
	
	SMG_DisplayATT100(ATT100,ADC_Display);
	
	/*Up Button*/
	if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
	{
		lastCounter = UpButton.PressCounter;
		UpButton.PressCounter = 0;
		ATT100 = ATT100+1;
		Flashflag = 1;
	}
	else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		UpButton.PressCounter = 0;
		if(UpButton.PressTimer<KEY_LEVEL_1)
		{
			if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				ATT100 = ATT100+1;
				Flashflag = 1;
			}
		}
		else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
		{
			if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				ATT100 = ATT100+2;
				Flashflag = 1;
			}
		}
		else 
		{
			if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				ATT100 = ATT100+5;
				Flashflag = 1;
			}
		}
	}	
	else
	{
		UpButton.Effect = PressShort;
	}
	
	/*Down Button*/
	if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
	{
		DownButton.PressCounter = 0;
		ATT100 = ATT100-1;
		Flashflag = 1;
	}
	else 	if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		DownButton.PressCounter = 0;
		if(DownButton.PressTimer<KEY_LEVEL_1)
		{
			if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				ATT100 = ATT100-1;
				Flashflag = 1;
			}
		}
		else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
		{
			if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				ATT100 = ATT100-2;
				Flashflag = 1;
			}
		}
		else 
		{
			if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				ATT100 = ATT100-5;
				Flashflag = 1;
			}
		}
	}
	else
	{
		DownButton.Effect = PressShort;
	}
	
	if(ATT100<=1)
			ATT100 =1;
	else if(ATT100>=100)
			ATT100 =100;
	
		if(EventFlag&Blink500msFlag && Flashflag==1) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
			//WriteFlash(ATT100_FLASH_DATA_ADDRESS,ATT100);
		}
		
		//ATTSet(ATT100);/*����ATT�����*/
	
}


void Menu_PERCENTAGE(void)
{
	static uint8_t lastCounter;
	uint8_t Flashflag=0;
	
	
	
	/*Up Button*/
	if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
	{
		lastCounter = UpButton.PressCounter;
		UpButton.PressCounter = 0;
		PERCENTAGE = PERCENTAGE+1;
		Flashflag = 1;
	}
	else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		UpButton.PressCounter = 0;
		if(UpButton.PressTimer<KEY_LEVEL_1)
		{
			if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				PERCENTAGE = PERCENTAGE+1;
				Flashflag = 1;
			}
		}
		else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
		{
			if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				PERCENTAGE = PERCENTAGE+2;
				Flashflag = 1;
			}
		}
		else 
		{
			if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				PERCENTAGE = PERCENTAGE+5;
				Flashflag = 1;
			}
		}
	}	
	else
	{
		UpButton.Effect = PressShort;
	}
	
	/*Down Button*/
	if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
	{
		DownButton.PressCounter = 0;
		PERCENTAGE = PERCENTAGE-1;
		Flashflag = 1;
	}
	else 	if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		DownButton.PressCounter = 0;
		if(DownButton.PressTimer<KEY_LEVEL_1)
		{
			if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				PERCENTAGE = PERCENTAGE-1;
				Flashflag = 1;
			}
		}
		else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
		{
			if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				PERCENTAGE = PERCENTAGE-2;
				Flashflag = 1;
			}
		}
		else 
		{
			if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				PERCENTAGE = PERCENTAGE-5;
				Flashflag = 1;
			}
		}
	}
	else
	{
		DownButton.Effect = PressShort;
	}
	
	if(PERCENTAGE<=0)
			PERCENTAGE =0;
	else if(PERCENTAGE>=50)
			PERCENTAGE =50;
	SMG_DisplayPERCENTAGE(PERCENTAGE); //��ʾ
		if(EventFlag&Blink500msFlag && Flashflag==1) 
		{
			WriteFlash(PERCENTAGE_FLASH_DATA_ADDRESS,PERCENTAGE);
			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
		}
}

/*******************************
*
*�˵���ʾ DSC
*
*******************************/
void Menu_DSC(void)
{
	static uint8_t lastCounter;
		/*�������ʾ*/
		SMG_DisplayMenuDSC(DSC);
	
		/*Up Button*/
		if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
		{
			lastCounter = UpButton.PressCounter;
			UpButton.PressCounter = 0;
			if(DSC==0)
				DSC = 1;
			else 
				DSC=0;
			WriteFlash(DSC_FLASH_DATA_ADDRESS,DSC);
		}

		/*Down Button*/
		if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
		{
			DownButton.PressCounter = 0;
			if(DSC==0)
				DSC = 1;
			else 
				DSC=0;
			WriteFlash(DSC_FLASH_DATA_ADDRESS,DSC);
		}
}



///*SV COUNTER�趨�˵�*/
//void MenuOne_SV(void)
//{
////	static uint8_t lastCounter;
////	uint8_t Flashflag=0;
////	SMG_DisplaSV(SV);
////	
////	/*Up Button*/
////	if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
////	{
////		lastCounter = UpButton.PressCounter;
////		UpButton.PressCounter = 0;
////		SV = SV+1;
////		Flashflag = 1;
////	}
////	else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
////	{				/*�����Ű���������ʱ�䳬������ʱ��*/
////		UpButton.PressCounter = 0;
////		if(UpButton.PressTimer<KEY_LEVEL_1)
////		{
////			if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
////			{
////				KeytempPress = 0;	
////				SV = SV+1;
////				Flashflag = 1;
////			}
////		}
////		else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
////		{
////			if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
////			{
////				KeytempPress = 0;	
////				SV = SV+2;
////				Flashflag = 1;
////			}
////		}
////		else 
////		{
////			if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
////			{
////				KeytempPress = 0;	
////				SV = SV+5;
////				Flashflag = 1;
////			}
////		}
////	}	
////	else
////	{
////		UpButton.Effect = PressShort;
////	}
////	
////	/*Down Button*/
////	if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
////	{
////		DownButton.PressCounter = 0;
////		SV = SV-1;
////		Flashflag = 1;
////	}
////	else 	if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
////	{				/*�����Ű���������ʱ�䳬������ʱ��*/
////		DownButton.PressCounter = 0;
////		if(DownButton.PressTimer<KEY_LEVEL_1)
////		{
////			if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
////			{
////				KeytempPress = 0;	
////				SV = SV-1;
////				Flashflag = 1;
////			}
////		}
////		else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
////		{
////			if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
////			{
////				KeytempPress = 0;	
////				SV = SV-2;
////				Flashflag = 1;
////			}
////		}
////		else 
////		{
////			if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
////			{
////				KeytempPress = 0;	
////				SV = SV-5;
////				Flashflag = 1;
////			}
////		}
////	}	
////	else
////	{
////		DownButton.Effect = PressShort;
////	}
////	
////	if(SV<=1)
////			SV =1;
////	else if(SV>=65535)
////			SV =65535;
////	
////		if(EventFlag&Blink500msFlag && Flashflag==1) 
////		{
////			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
////			//WriteFlash(SV_FLASH_DATA_ADDRESS,SV);
////		}
//}


/*FSV COUNTER�趨�˵�*/
void MenuOne_FSV(void)
{
	static uint8_t lastCounter;
	uint8_t Flashflag=0;
	SMG_DisplayFSV(FSV);
	
	/*Up Button*/
	if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
	{
		lastCounter = UpButton.PressCounter;
		UpButton.PressCounter = 0;
		FSV = FSV+1;
		Flashflag = 1;
	}
	else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		UpButton.PressCounter = 0;
		if(UpButton.PressTimer<KEY_LEVEL_1)
		{
			if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;	
				FSV = FSV+1;
				Flashflag = 1;
			}
		}
		else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
		{
			if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;	
				FSV = FSV+2;
				Flashflag = 1;
			}
		}
		else 
		{
			if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;	
				FSV = FSV+5;
				Flashflag = 1;
			}
		}
	}	
	else
	{
		UpButton.Effect = PressShort;
	}
	
	/*Down Button*/
	if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
	{
		DownButton.PressCounter = 0;
		FSV = FSV-1;
		Flashflag = 1;
	}
	else 	if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		DownButton.PressCounter = 0;
		if(DownButton.PressTimer<KEY_LEVEL_1)
		{
			if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;	
				FSV = FSV-1;
				Flashflag = 1;
			}
		}
		else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
		{
			if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;	
				FSV = FSV-2;
				Flashflag = 1;
			}
		}
		else 
		{
			if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;	
				FSV = FSV-5;
				Flashflag = 1;
			}
		}
	}	
	else
	{
		DownButton.Effect = PressShort;
	}
	
	if(FSV<=1)
			FSV =1;
	else if(FSV>=65535)
			FSV =65535;
	
		if(EventFlag&Blink500msFlag && Flashflag==1) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
			//WriteFlash(FSV_FLASH_DATA_ADDRESS,FSV);
		}
	
}

/*OUT1 DELAY�趨�˵�*/

void MenuTwo_OUT1_DelaySET(void)
{
		static uint8_t lastCounter;
	
		/*TOFF Mode*/
		while(DispalyNo==0)
		{
			
			MenuTwo_OUT1_TOFF();
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				DispalyNo++;
				if(DispalyNo>2)
					DispalyNo = 0;
			}
			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
			{
				//lastCounter = DownButton.PressCounter;
				DownButton.PressCounter = 0;
				DispalyNo--;
				if(DispalyNo<0)
					DispalyNo = 2;
			}
			if(ModeButton.PressCounter>=(TimerDisplayIndex)) 
			{
				ModeButton.PressCounter = (TimerDisplayIndex+2); //��������END�˵�
				break;
			}
			/*��MenuTwo_OUT1_TOFF();���ˣ�����ʱ����2us*/
			key_time++;
			if(key_time % 300 == 0)					// 1,000/2 = 500  1ms
			{
				SMG_Diplay();  /*ˢ�������*/
			}
			if (key_time % 3000 == 0) // 8000/2 = 4000  8ms
			{
				Key_Scan(); //��ʱɨ�谴��
				KeytempPress = 1;
				IWDG_ReloadCounter();
			}
			if (key_time >= 120000)  // 500,000/2 = 250,000  500ms
			{
				EventFlag = EventFlag | Blink500msFlag;
				key_time = 0;
			}
		}
		
		/*OFFD mode*/
		//while(DispalyNo==3)
		while(0)
		{
				MenuTwo_OUT1_OFFD();
				if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
				{
					//lastCounter = UpButton.PressCounter;
					UpButton.PressCounter = 0;
					DispalyNo++;
					if(DispalyNo>3)
						DispalyNo = 0;
				}
				if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
				{
					//lastCounter = DownButton.PressCounter;
					DownButton.PressCounter = 0;
					DispalyNo--;
					if(DispalyNo<0)
						DispalyNo = 3;
				}
				if(ModeButton.PressCounter>=(TimerDisplayIndex+2)) break;
		}
		/*ON_D mode*/
		while(DispalyNo==1)
		{
				
				MenuTwo_OUT1_ON_D();
				if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
				{
					//lastCounter = UpButton.PressCounter;
					UpButton.PressCounter = 0;
					DispalyNo++;
					if(DispalyNo>2)
						DispalyNo = 0;
				}
				if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
				{
					//lastCounter = DownButton.PressCounter;
					DownButton.PressCounter = 0;
					DispalyNo--;
					if(DispalyNo<0)
						DispalyNo = 2;
				}
				if(ModeButton.PressCounter>=(TimerDisplayIndex))
				{
					ModeButton.PressCounter = (TimerDisplayIndex+2); //��������END�˵�
					break;
				}
			/*��MenuTwo_OUT1_ON_D();���ˣ�����ʱ����2us*/
			key_time++;
			if(key_time % 300 == 0)					// 1,000/2 = 500  1ms
			{
				SMG_Diplay();  /*ˢ�������*/
			}
			if (key_time % 3000 == 0) // 8000/2 = 4000  8ms
			{
				Key_Scan(); //��ʱɨ�谴��
				KeytempPress = 1;
				IWDG_ReloadCounter();
			}
			if (key_time >= 120000)  // 500,000/2 = 250,000  500ms
			{
				EventFlag = EventFlag | Blink500msFlag;
				key_time = 0;
			}
		}
		while(DispalyNo==2)
		{
				MenuTwo_OUT1_SHOT();
				if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
				{
					//lastCounter = UpButton.PressCounter;
					UpButton.PressCounter = 0;
					DispalyNo++;
					if(DispalyNo>2)
						DispalyNo = 0;
				}
				if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
				{
					//lastCounter = DownButton.PressCounter;
					DownButton.PressCounter = 0;
					DispalyNo--;
					if(DispalyNo<0)
						DispalyNo = 2;
				}
				if(ModeButton.PressCounter>=(TimerDisplayIndex)) 
				{
					ModeButton.PressCounter = (TimerDisplayIndex+2); //��������END�˵�
					break;
				}
				/*��MenuTwo_OUT1_ON_D();���ˣ�����ʱ����2us*/
				key_time++;
				if(key_time % 300 == 0)					// 1,000/2 = 500  1ms
				{
					SMG_Diplay();  /*ˢ�������*/
				}
				if (key_time % 3000 == 0) // 8000/2 = 4000  8ms
				{
					Key_Scan(); //��ʱɨ�谴��
					KeytempPress = 1;
					IWDG_ReloadCounter();
				}
				if (key_time >= 120000)  // 500,000/2 = 250,000  500ms
				{
					EventFlag = EventFlag | Blink500msFlag;
					key_time = 0;
				}
		}
		
//		while(DispalyNo==4)
//		{
//				MenuTwo_DEL();
//				if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
//				{
//					//lastCounter = UpButton.PressCounter;
//					UpButton.PressCounter = 0;
//					DispalyNo++;
//					if(DispalyNo>4)
//						DispalyNo = 0;
//				}
//				if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
//				{
//					//lastCounter = DownButton.PressCounter;
//					DownButton.PressCounter = 0;
//					DispalyNo--;
//					if(DispalyNo<0)
//						DispalyNo = 4;
//				}
//				if(ModeButton.PressCounter>4) break;
//		}

		
}


/*******************************
*
*OUT1_TOFF
*
*******************************/
void MenuTwo_OUT1_TOFF(void)
{
		if(EventFlag&Blink500msFlag) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
			SMG_DisplayMenuTwo_TOFF();
		}
		//if(ModeButton.Effect==PressShort && ModeButton.PressCounter==4)
			OUT1_Mode.DelayMode = TOFF;
}

/*******************************
*
*OUT1_OFFD
*
*******************************/
void MenuTwo_OUT1_OFFD(void)
{
		static uint8_t lastCounter;

		if(EventFlag&Blink500msFlag) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
			/*��ʾOFFD,��˸*/
			SMG_DisplayMenuTwo_OFFD();
		}
		/*�̰�MODE�󣬽���OFFD�������Ӳ˵�*/
		while(ModeButton.Effect==PressShort && ModeButton.PressCounter==TimerDisplayIndex)
		{
			OUT1_Mode.DelayMode = OFFD;
			/*��ʾOFFD value*/
			if(UpButton.Status==Release&&DownButton.Status==Release)
			{
				if(EventFlag&Blink500msFlag) 
				{
					EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
					/*��ʾOFFD,��˸*/
					SMG_DisplayMenuTwo_OFFD_SET(OUT1_Mode.DelayValue,1);
				}
			}
			else
			{
				EventFlag = EventFlag &(~Blink500msFlag);
				SMG_DisplayMenuTwo_OFFD_SET(OUT1_Mode.DelayValue,0);
			}
			/*Up Button*/
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				MenuFlashflag = 1;
			}
			else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				UpButton.PressCounter = 0;
				if(UpButton.PressTimer<KEY_LEVEL_1)
				{
					if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
						MenuFlashflag = 1;
						KeytempPress = 0;
					}
				}
				else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
				{
					if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
						MenuFlashflag = 1;
						KeytempPress = 0;
					}
				}
				else 
				{
					if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
						MenuFlashflag = 1;
						KeytempPress = 0;
					}
				}
			}	
			else
			{
				UpButton.Effect = PressShort;
			}
				/*Down Button*/
			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
			{
				DownButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				MenuFlashflag = 1;
			}
			else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				DownButton.PressCounter = 0;
				if(DownButton.PressTimer<KEY_LEVEL_1)
				{
					if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
						MenuFlashflag = 1;
						KeytempPress = 0;
					}
				}
				else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
				{
					if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
						MenuFlashflag = 1;
						KeytempPress = 0;
					}
				}
				else 
				{
					if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
						MenuFlashflag = 1;
						KeytempPress = 0;
					}
				}
			}	
			else
			{
				DownButton.Effect = PressShort;
				if((EventFlag&Blink500msFlag)&&(MenuFlashflag==1)) 
				{
					MenuFlashflag = 0;
					EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
					WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS,OUT1_Mode.DelayMode);
					WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS,OUT1_Mode.DelayValue);
				}
			}
			if(OUT1_Mode.DelayValue>=9999)
				OUT1_Mode.DelayValue = 9999;
			else if(OUT1_Mode.DelayValue<=0)
					OUT1_Mode.DelayValue = 0;			
		}
}

/*******************************
*
*OUT1_ON_D
*
*******************************/
void MenuTwo_OUT1_ON_D(void)
{
		static uint8_t lastCounter;
		uint8_t Flashflag=0;
		if(EventFlag&Blink500msFlag) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
			
			SMG_DisplayMenuTwo_ON_Delay();
		}
		/*�̰�MODE�󣬽���ON_D�������Ӳ˵�*/
		while(ModeButton.Effect==PressShort && ModeButton.PressCounter==TimerDisplayIndex)
		{
			//GPIOA->BSRR = 0x0080;
			OUT1_Mode.DelayMode = ON_D;
			/*��ʾON_D value*/
			if(UpButton.Status==Release&&DownButton.Status==Release)
			{
				if(EventFlag&Blink500msFlag) 
				{
					EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
					/*��ʾON_D,��˸*/
					SMG_DisplayMenuTwo_ON_Delay_SET(OUT1_Mode.DelayValue,1);
				}
			}
			else
			{
				EventFlag = EventFlag &(~Blink100msFlag);
				SMG_DisplayMenuTwo_ON_Delay_SET(OUT1_Mode.DelayValue,0);
			}
			
			/*Up Button*/
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				Flashflag = 1;
			}
			else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				UpButton.PressCounter = 0;
				if(UpButton.PressTimer<KEY_LEVEL_1)
				{
					if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
				else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
				{
					if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
				else 
				{
					if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
			}	
			else
			{
				UpButton.Effect = PressShort;
				if(EventFlag&Blink500msFlag && Flashflag==1) 
				{
					Flashflag = 0;
					EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
					WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS,OUT1_Mode.DelayMode);
					WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS,OUT1_Mode.DelayValue);
				}
			}	
				/*Down Button*/
			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
			{
				DownButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				Flashflag = 1;
			}
			else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				DownButton.PressCounter = 0;
				if(DownButton.PressTimer<KEY_LEVEL_1)
				{
					if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
				else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
				{
					if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
				else 
				{
					if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
			}	
			else
			{
				DownButton.Effect = PressShort;
				if(EventFlag&Blink500msFlag && Flashflag==1) 
				{
					Flashflag = 0;
					EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
					WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS,OUT1_Mode.DelayMode);
					WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS,OUT1_Mode.DelayValue);
				}
			}
			
			if(OUT1_Mode.DelayValue>=9999)
				OUT1_Mode.DelayValue = 9999;
			else if(OUT1_Mode.DelayValue<=1)
					OUT1_Mode.DelayValue = 1;
			/*��while(ModeButton.Effect==PressShort && ModeButton.PressCounter==TimerDisplayIndex)���ˣ�����ʱ����3.3us*/
			//GPIOA->BRR = 0x00080;
			key_time++;
			if(key_time % 200 == 0)					// 1,000/3.3 = 333.33  1ms
			{
				SMG_Diplay();  /*ˢ�������*/
			}
			if (key_time % 800 == 0) // 8000/3.3 = 2424.2424  8ms
			{
				Key_Scan(); //��ʱɨ�谴��
				KeytempPress = 1;
				IWDG_ReloadCounter();
			}
			if (key_time >= 150000)  // 500,000/3.3 = 151515.151515  500ms
			{
				EventFlag = EventFlag | Blink500msFlag;
				key_time = 0;
			}

		}
}


/*******************************
*
*OUT1_SHOT
*
*******************************/
void MenuTwo_OUT1_SHOT(void)
{
		static uint8_t lastCounter;
		uint8_t Flashflag=0;
		if(EventFlag&Blink500msFlag) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
			
			SMG_DisplayMenuTwo_SHOT();
		}
		/*�̰�MODE�󣬽���SHOT�������Ӳ˵�*/
		while(ModeButton.Effect==PressShort && ModeButton.PressCounter==TimerDisplayIndex)
		{
//			GPIOA->BSRR = 0x0080;
			OUT1_Mode.DelayMode = SHOT;
			/*��ʾSHOT value*/			
			if(UpButton.Status==Release&&DownButton.Status==Release)
			{
				if(EventFlag&Blink500msFlag) 
				{
					EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
					/*��ʾSHOT,��˸*/
					SMG_DisplayMenuTwo_SHOT_SET(OUT1_Mode.DelayValue,1);
				}
			}
			else
			{
				EventFlag = EventFlag &(~Blink500msFlag);
				SMG_DisplayMenuTwo_SHOT_SET(OUT1_Mode.DelayValue,0);
			}
			
			/*Up Button*/
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				Flashflag = 1;
			}
			else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				UpButton.PressCounter = 0;
				if(UpButton.PressTimer<KEY_LEVEL_1)
				{
					if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
				else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
				{
					if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
				else 
				{
					if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
			}	
			else
			{
				UpButton.Effect = PressShort;
				if(EventFlag&Blink500msFlag && Flashflag==1) 
				{
					Flashflag = 0;
					EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
					WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS,OUT1_Mode.DelayMode);
					WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS,OUT1_Mode.DelayValue);
				}
			}	
				/*Down Button*/
			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
			{
				DownButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				Flashflag = 1;
			}
			else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				DownButton.PressCounter = 0;
				if(DownButton.PressTimer<KEY_LEVEL_1)
				{
					if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
				else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
				{
					if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
				else 
				{
					if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
					{
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
						Flashflag = 1;
						KeytempPress = 0;
					}
				}
			}	
			else
			{
				DownButton.Effect = PressShort;
				if(EventFlag&Blink500msFlag && Flashflag==1) 
				{
					Flashflag = 0;
					EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
					WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS,OUT1_Mode.DelayMode);
					WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS,OUT1_Mode.DelayValue);
				}
			}
			
			if(OUT1_Mode.DelayValue>=9999)
				OUT1_Mode.DelayValue = 9999;
			else if(OUT1_Mode.DelayValue<=1)
					OUT1_Mode.DelayValue = 1;

			/*��while(ModeButton.Effect==PressShort && ModeButton.PressCounter==TimerDisplayIndex)���ˣ�����ʱ����3.3us*/
			//GPIOA->BRR = 0x00080;
			key_time++;
			if(key_time % 200 == 0)					// 1,000/3.3 = 333.33  1ms
			{
				SMG_Diplay();  /*ˢ�������*/
			}
			if (key_time % 800 == 0) // 8000/3.3 = 2424.2424  8ms
			{
				Key_Scan(); //��ʱɨ�谴��
				KeytempPress = 1;
				IWDG_ReloadCounter();
			}
			if (key_time >= 150000)  // 500,000/3.3 = 151515.151515  500ms
			{
				EventFlag = EventFlag | Blink500msFlag;
				key_time = 0;
			}
			
		}
}



/*******************************
*
*OUT1_DEL Ӧ��ֵ����
*
*******************************/

void MenuTwo_DEL(void)
{
		static uint8_t lastCounter;
		GPIOA->BRR = 0x00080;
		SMG_DisplayMenuTwo_DEL_SET(DEL,0);
			
	/*Up Button*/
	if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
	{
		lastCounter = UpButton.PressCounter;
		UpButton.PressCounter = 0;
		DEL = DEL+1;
		MenuFlashflag = 1;
	}
	else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		UpButton.PressCounter = 0;
		if(UpButton.PressTimer<KEY_LEVEL_1)
		{
			if(UpButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				DEL = DEL+1;
				MenuFlashflag = 1;
			}
		}
		else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
		{
			if(UpButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				DEL = DEL+2;
				MenuFlashflag = 1;
			}
		}
		else 
		{
			if(UpButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				DEL = DEL+5;
				MenuFlashflag = 1;
			}
		}
	}	
	
//	{
//		UpButton.Effect = PressShort;
//	}
	
	/*Down Button*/
	else if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
	{
		DownButton.PressCounter = 0;
		DEL = DEL-1;
		MenuFlashflag = 1;
	}
	else 	if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
	{				/*�����Ű���������ʱ�䳬������ʱ��*/
		DownButton.PressCounter = 0;
		if(DownButton.PressTimer<KEY_LEVEL_1)
		{
			if(DownButton.PressTimer%KEY_LEVEL_1_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				DEL = DEL-1;
				MenuFlashflag = 1;
			}
		}
		else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
		{
			if(DownButton.PressTimer%KEY_LEVEL_2_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				DEL = DEL-2;
				MenuFlashflag = 1;
			}
		}
		else 
		{
			if(DownButton.PressTimer%KEY_LEVEL_3_SET==0&&KeytempPress == 1)
			{
				KeytempPress = 0;
				DEL = DEL-5;
				MenuFlashflag = 1;
			}
		}
	}
	else
	{
		DownButton.Effect = PressShort;
		UpButton.Effect = PressShort;
		if((EventFlag&Blink500msFlag)&& (MenuFlashflag==1)) 
		{
			MenuFlashflag = 0;
			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
			WriteFlash(DEL_FLASH_DATA_ADDRESS,DEL);
		}
	}
	
	if(DEL<=80)
			DEL =80;
	else if(DEL>=3000)
			DEL =3000;
	/*��SMG_DisplayMenuTwo_DEL_SET(DEL,0)���ˣ�����ʱ����9us*/
	//GPIOA->BSRR = 0x0080;
		key_time++;
		if(key_time % 112 == 0)					// 1,000/9 = 111.111  1ms
		{
			SMG_Diplay();  /*ˢ�������*/
		}
		if (key_time % 888 == 0) // 8000/9 = 888.888  8ms
		{
			Key_Scan(); //��ʱɨ�谴��
			IWDG_ReloadCounter();
			KeytempPress = 1;
		}
		if (key_time >= 55555)  // 500,000/9 = 55555.555  500ms
		{
			EventFlag = EventFlag | Blink500msFlag;
			DACOUT1 = DACOriginalValue + DEL;
			DAC_SetChannel1Data(DAC_Align_12b_R, (uint16_t)DACOUT1);	
			key_time = 0;
		}
}



