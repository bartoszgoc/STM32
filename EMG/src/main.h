static __IO uint32_t TimingDelay;
__IO uint16_t voltage_Pomiar[3];

volatile uint8_t flag_SysTick, Flag_ActionDetected_Forearm,Flag_ActionDetected_Biceps,Flag_ActionDetected_Shoulder,Flag_ActionDetected_Finger;
volatile uint16_t licznik_1,licznik_2,licznik_3 = 0;

float32_t FFT[3][512];
float32_t buffer_input[3][1024];
float32_t buffer_input_copy[3][256];
float32_t buffer_output[3][1024];
float32_t buffer_output_mag[3][1024];
float32_t buffer_output_mag_copy[3][256];
float32_t maxvalue[3];


float32_t fft, caleFFT,ulamekFFT;
uint32_t Liczba_UART_1,Liczba_UART_2,Liczba_UART_3 = 0;
uint32_t Cale_UART_1,Cale_UART_2,Cale_UART_3 = 0;
uint32_t Ulamek_UART_1,Ulamek_UART_2,Ulamek_UART_3 = 0;
float32_t Buffor_UART_1[512],Buffor_UART_2[512],Buffor_UART_3[512];
int32_t Buffer[256],Buffer2[256],Buffer3[256];

uint8_t Buffor_LCD_1[512],Buffor_LCD_2[512],Buffor_LCD_3[512];
uint32_t Liczba_LCD_1,Liczba_LCD_2,Liczba_LCD_3 = 0;
uint32_t Cale_LCD_1,Cale_LCD_2,Cale_LCD_3 = 0;
uint32_t Ulamek_LCD_1,Ulamek_LCD_2,Ulamek_LCD_3 = 0;

uint32_t Buffor_Collect[5];

uint32_t maxvalueindex[3];

arm_rfft_instance_f32 S;
arm_cfft_radix4_instance_f32 S_CFFT;
