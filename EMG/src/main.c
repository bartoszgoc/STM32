#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include "arm_math.h"
#include <math.h>
#include "stm32f4xx_rcc.h"
#include "main.h"
#include "stm32_ub_uart.h"
#include "stdbool.h"

void CheckOutCLK() {
	u16 StartUpCounter = 0;
	uint32_t HSEStatus;

	RCC_HSEConfig(RCC_HSE_ON);

	do {
		StartUpCounter++;
	} while ((RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET));

	if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == SET) {
		HSEStatus = SUCCESS;
	} else {
		HSEStatus = ERROR;
	}
}

void KonfiguracjaADC() {

	CheckOutCLK();

	ADC_InitTypeDef ADC_Init_Structure1, ADC_Init_Structure2;
	GPIO_InitTypeDef GPIO_Init_Structure1, GPIO_Init_Structure2;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);

	GPIO_Init_Structure1.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2 | GPIO_Pin_1;
	GPIO_Init_Structure1.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init_Structure1.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_Init_Structure1);

	ADC_Init_Structure1.ADC_DataAlign = ADC_DataAlign_Right;

	ADC_Init_Structure1.ADC_Resolution = ADC_Resolution_12b;

	ADC_Init_Structure1.ADC_ContinuousConvMode = ENABLE;
	ADC_Init_Structure1.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;

	ADC_Init_Structure1.ADC_ExternalTrigConvEdge =
	ADC_ExternalTrigConvEdge_None;

	ADC_Init_Structure1.ADC_NbrOfConversion = 1;

	ADC_Init_Structure1.ADC_ScanConvMode = ENABLE;

	ADC_Init(ADC1, &ADC_Init_Structure1);
	ADC_Init(ADC2, &ADC_Init_Structure1);
	ADC_Init(ADC3, &ADC_Init_Structure1);


	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 1, ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 1, ADC_SampleTime_144Cycles);

	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
	ADC_Cmd(ADC3, ENABLE);

}

void Display_Init(void) {
	LCD_Init();

	LCD_Clear(0xFFFF);

	LCD_LayerInit();
	LTDC_Cmd(ENABLE);
	LCD_SetLayer(LCD_FOREGROUND_LAYER);

	LCD_SetFont(&Font8x12);
	LCD_DisplayStringLine(LCD_LINE_9, "0 50 100    200            450");
	LCD_DisplayStringLine(LCD_LINE_17, "0 50 100    200            450");
	LCD_DisplayStringLine(LCD_LINE_25, "0 50 100    200            450");

}

void DrawFFT() {
	int j = 0;
	for (j = 0; j < 255; j++) {
		LCD_SetTextColor(0xFFFF);
		LCD_DrawUniLine(j, 0 + 100, j + 1,
				0 + (100 - buffer_output_mag_copy[0][j + 1]));
		LCD_DrawUniLine(j, 100 + 100, j + 1,
				100 + (100 - buffer_output_mag_copy[1][j + 1]));
		LCD_DrawUniLine(j, 200 + 100, j + 1,
				200 + (100 - buffer_output_mag_copy[2][j + 1]));
	}
	for (j = 0; j < 255; j++) {
		LCD_SetTextColor(0x001F);
		LCD_DrawUniLine(j, 0 + 100, j + 1,
				0 + (100 - buffer_output_mag[0][j + 1]));
		LCD_DrawUniLine(j, 100 + 100, j + 1,
				100 + (100 - buffer_output_mag[1][j + 1]));
		LCD_DrawUniLine(j, 200 + 100, j + 1,
		200 + (100 - buffer_output_mag[2][j + 1]));
	}
}


void FFT_Calculate() {
	int i = 0, ii = 0;

	for (i = 0; i < 3; i++) {
		arm_rfft_f32(&S, buffer_input[i], buffer_output[i]);
		arm_cmplx_mag_f32(buffer_output[i], buffer_output_mag[i], 512);
		arm_max_f32(buffer_output_mag[i], 512, &maxvalue[i], &maxvalueindex[i]);

		for (ii = 0; ii < 512; ii++) {
			buffer_output_mag[i][ii] =  buffer_output_mag[i][ii] / 100; //by³o 100
			FFT[i][ii] = buffer_output_mag[i][ii];
		}
	}
}

int KonwersjaADC(ADC_TypeDef* ADCx) {

	ADC_SoftwareStartConv(ADCx);

	while (!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC))
		;

	return ADC_GetConversionValue(ADCx);
}

uint32_t ReadAdc(ADC_TypeDef* ADCx) {
	return KonwersjaADC(ADCx) * 3300 / 0xFFF;
}

void Collect() {
	uint16_t Value_Forearm = (FFT[0][30] + FFT[0][40] + FFT[0][45]) / 3; 
	uint16_t Value_Biceps = (FFT[1][30] + FFT[1][35]) / 2;
	uint16_t Value_Shoulder = (FFT[2][30] + FFT[2][35] + FFT[2][40] + FFT[2][50]
			+ FFT[2][60]) / 5;

	uint16_t Value_Finger = (FFT[0][20] + FFT[0][22] + FFT[0][24] + FFT[0][26]
			+ FFT[0][27] + FFT[0][28]) / 6;

	uint16_t Value_Finger_Range = (FFT[0][45] + FFT[0][60] + FFT[0][70]) / 3;

	uint16_t Value_ChangeSing = (FFT[0][40] + FFT[0][41] + FFT[0][42]
			+ FFT[0][43] + FFT[0][44] + FFT[0][45]) / 6;

	uint8_t Used = 0;
	static uint8_t WaitUntilMoved = 0;

	static uint16_t Counter_MoveDetected_Forearm = 0;
	static uint16_t Counter_MoveDetected_Biceps = 0;
	static uint16_t Counter_MoveDetected_Shoulder = 0;
	static uint16_t Counter_ChangeSing = 0;
	static uint16_t Actual_Sing = 0;
	static int Sign = 2;

	if (Value_Forearm > 10) {
		Counter_MoveDetected_Forearm++;
		Used = 1;
	} else {
		Flag_ActionDetected_Forearm = Counter_MoveDetected_Forearm;
		if (Flag_ActionDetected_Forearm > 0) {
			sprintf((char*) Buffer, "mov0 %d\13\10",
					Flag_ActionDetected_Forearm * 10 * Sign);
			UB_Uart_SendString(COM1, Buffer);
			Counter_MoveDetected_Forearm = 0;
			Flag_ActionDetected_Forearm = 0;

			WaitUntilMoved = 1;
		}
	}

	if (Value_Biceps > 10) {
		Counter_MoveDetected_Biceps++;
		Used = 1;
	} else {
		Flag_ActionDetected_Biceps = Counter_MoveDetected_Biceps;
		if (Flag_ActionDetected_Biceps > 0) {
			sprintf((char*) Buffer2, "mov1 %d\13\10",
					Flag_ActionDetected_Biceps * 10 * Sign);
			UB_Uart_SendString(COM1, Buffer2);
			Counter_MoveDetected_Biceps = 0;
			Flag_ActionDetected_Biceps = 0;
			;
			WaitUntilMoved = 1;
		}
	}

	if (Value_Shoulder > 10) {
		Counter_MoveDetected_Shoulder++;
		Used = 1;
	} else {
		Flag_ActionDetected_Shoulder = Counter_MoveDetected_Shoulder;
		if (Flag_ActionDetected_Shoulder > 0) {
			sprintf((char*) Buffer3, "mov2 %d\13\10",
					Flag_ActionDetected_Shoulder * 10 * Sign);

			UB_Uart_SendString(COM1, Buffer3);
			Counter_MoveDetected_Shoulder = 0;
			Flag_ActionDetected_Shoulder = 0;

			WaitUntilMoved = 1;
		}
	}

	if (Value_Finger > 2 && Value_Forearm < 4
			&& Counter_MoveDetected_Forearm == 0 && Value_Finger_Range < 2
			&& Used == 0) {
		UB_Uart_SendString(COM1, "grab \13\10");

	}

	if (Value_ChangeSing > 2 && Value_Forearm < 4
			&& Counter_MoveDetected_Forearm == 0 && Value_Finger < 4
			&& Used == 0 && WaitUntilMoved == 1) {
		Counter_ChangeSing++;
		UB_Uart_SendString(COM1, "change \13\10");
		Actual_Sing = Counter_ChangeSing % 2;
		WaitUntilMoved = 0;
		if (Actual_Sing == 1) {
			Sign = 1;
		} else {
			Sign = -1;
		}
	}
}

void Delay(__IO uint32_t nTime) {
	TimingDelay = nTime;

	while (TimingDelay != 0)
		;
}

void TimingDelay_Decrement(void) {
	if (TimingDelay != 0x00) {
		TimingDelay--;
	}
}

void Clear(void) {
	memset(buffer_input_copy[0], 0, sizeof(buffer_input[0]));
	memset(buffer_output_mag_copy[0], 0, sizeof(buffer_output_mag_copy[0]));
	memset(buffer_input_copy[1], 0, sizeof(buffer_input[1]));
	memset(buffer_output_mag_copy[1], 0, sizeof(buffer_output_mag_copy[1]));
	memset(buffer_input_copy[2], 0, sizeof(buffer_input[2]));
	memset(buffer_output_mag_copy[2], 0, sizeof(buffer_output_mag_copy[2]));

}

void Overwrite(void) {
	int i = 0;
	for (i = 0; i < 256; ++i) {
		buffer_input_copy[0][i] = buffer_input[0][i];
		buffer_output_mag_copy[0][i] = buffer_output_mag[0][i];

		buffer_input_copy[1][i] = buffer_input[1][i];
		buffer_output_mag_copy[1][i] = buffer_output_mag[1][i];

		buffer_input_copy[2][i] = buffer_input[2][i];
		buffer_output_mag_copy[2][i] = buffer_output_mag[2][i];
	}
}

void SysTick_Handler(void) {
	flag_SysTick = 1;
}

int main(void) {

	SystemCoreClockUpdate();
	KonfiguracjaADC();
	Display_Init();
	SysTick_Config(SystemCoreClock / 1024);
	UB_Uart_Init();
	uint16_t Counter_ADC = 0;
	arm_rfft_init_f32(&S, &S_CFFT, 512, 0, 1);

	while (1) {
		if (flag_SysTick == 1) {
			flag_SysTick = 0;
			voltage_Pomiar[0] = ReadAdc(ADC1);
			voltage_Pomiar[1] = ReadAdc(ADC2);
			voltage_Pomiar[2] = ReadAdc(ADC3);

			buffer_input[0][Counter_ADC] = voltage_Pomiar[0];
			buffer_input[1][Counter_ADC] = voltage_Pomiar[1];
			buffer_input[2][Counter_ADC] = voltage_Pomiar[2];

			Counter_ADC++;
			if (Counter_ADC == 512) {

				FFT_Calculate();
				DrawFFT();
				Collect();
				Clear();
				Overwrite();
				Counter_ADC = 0;
			}

		}

	}
}
