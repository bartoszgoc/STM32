
//
// Pinologia UART”W
//            UART1 : TX:[PA9,PB6] RX:[PA10,PB7]
//            UART2 : TX:[PA2,PD5] RX:[PA3,PD6]
//            UART3 : TX:[PB10,PC10,PD8] RX:[PB11,PC11,PD9]
//            UART4 : TX:[PA0,PC10] RX:[PA1,PC11]
//            UART5 : TX:[PC12] RX:[PD2]
//            UART6 : TX:[PC6,PG14] RX:[PC7,PG9]
//            UART7 : TX:[PE8,PF7] RX:[PE7,PF6]
//            UART8 : TX:[PE1] RX:[PE0]
//
//------------------------


//--------------------------------------------------------------
// Inkludy
//--------------------------------------------------------------
#include "stm32_ub_uart.h"




//--------------------------------------------------------------

//--------------------------------------------------------------
UART_t UART[] = {
// Name, Clock               , AF-UART      ,UART  , Baud , Interrupt
  {COM1,RCC_APB2Periph_USART1,GPIO_AF_USART1,USART1,115200,USART1_IRQn, // UART1 z 115200 Baud
// PORT , PIN      , Clock              , Source
  {GPIOA,GPIO_Pin_9 ,RCC_AHB1Periph_GPIOA,GPIO_PinSource9 },  // TX an PA9
  {GPIOA,GPIO_Pin_10,RCC_AHB1Periph_GPIOA,GPIO_PinSource10}}, // RX an PA10
};

//--------------------------------------------------------------
// inicjalizacja UARTA1
//--------------------------------------------------------------
void UB_Uart_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  UART_NAME_t nr;



    // Clock enable  TX and RX Pins
    RCC_AHB1PeriphClockCmd(UART[0].TX.CLK, ENABLE);
    RCC_AHB1PeriphClockCmd(UART[0].RX.CLK, ENABLE);

    // Clock enable  UART
    if((UART[0].UART==USART1) || (UART[0].UART==USART6)) {
      RCC_APB2PeriphClockCmd(UART[0].CLK, ENABLE);
    }
    else {
      RCC_APB1PeriphClockCmd(UART[0].CLK, ENABLE);
    }

    // UART
    GPIO_PinAFConfig(UART[0].TX.PORT,UART[0].TX.SOURCE,UART[0].AF);
    GPIO_PinAFConfig(UART[0].RX.PORT,UART[0].RX.SOURCE,UART[0].AF);

    // UART - alternatywne funkcje pinu
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    // TX-Pin
    GPIO_InitStructure.GPIO_Pin = UART[0].TX.PIN;
    GPIO_Init(UART[0].TX.PORT, &GPIO_InitStructure);
    // RX-Pin
    GPIO_InitStructure.GPIO_Pin =  UART[0].RX.PIN;
    GPIO_Init(UART[0].RX.PORT, &GPIO_InitStructure);

    // Oversampling
    USART_OverSampling8Cmd(UART[0].UART, ENABLE);

    // Baudrate, 8Databits, 1Stopbit, *8N1
    USART_InitStructure.USART_BaudRate = UART[0].BAUD;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART[0].UART, &USART_InitStructure);

    // UART enable
    USART_Cmd(UART[0].UART, ENABLE);

    // RX-Interrupt enable
    USART_ITConfig(UART[0].UART, USART_IT_RXNE, ENABLE);

    // enable UART Interrupt-Vector
    NVIC_InitStructure.NVIC_IRQChannel = UART[0].INT;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Ustawienie bufora
    UART_RX[0].rx_buffer[0]=RX_END_CHR;
    UART_RX[0].wr_ptr=0;
    UART_RX[0].status=RX_EMPTY;
  //}
}

//--------------------------------------------------------------
// Wys≥anie bajtu przez UART
//--------------------------------------------------------------
void UB_Uart_SendByte(UART_NAME_t uart, uint16_t wert)
{
  // odczekiwanie na wys≥anie
  while (USART_GetFlagStatus(UART[uart].UART, USART_FLAG_TXE) == RESET);
  USART_SendData(UART[uart].UART, wert);
}

//--------------------------------------------------------------
// wys≥anie stringa przez UART
//--------------------------------------------------------------
void UB_Uart_SendString(UART_NAME_t uart, char *ptr)
{
  // czekanie na wys≥anie
  while (*ptr != 0) {	//sprawdzanie czy wskünik nie jest null
    UB_Uart_SendByte(uart,*ptr);	//jeli nie to wysy≥anie po jedym bajcie
    ptr++;							//zwiÍkszenie wskaünika
  }
}

//--------------------------------------------------------------
	//odbieranie danych przez UART
//--------------------------------------------------------------
UART_RXSTATUS_t UB_Uart_ReceiveString(UART_NAME_t uart, char *ptr)
{
  UART_RXSTATUS_t ret_wartosc=RX_EMPTY;
  uint8_t n,wartosc;

  if(UART_RX[uart].status==RX_READY) {
	  ret_wartosc=RX_READY;
    // kopiowanie bufora
    n=0;
    do {
    	wartosc=UART_RX[uart].rx_buffer[n];
      if(wartosc!=RX_END_CHR) {
        ptr[n]=wartosc;
        n++;
      }
    }while(wartosc!=RX_END_CHR);
    // zerowanie eskaünika
    ptr[n]=0x00;
    // czyszczenie bufora
    UART_RX[uart].rx_buffer[0]=RX_END_CHR;
    UART_RX[uart].wr_ptr=0;
    UART_RX[uart].status=RX_EMPTY;
  }
  else if(UART_RX[uart].status==RX_FULL) {
    ret_wartosc=RX_FULL;
    // czyczczenie bufora
    UART_RX[uart].rx_buffer[0]=RX_END_CHR;
    UART_RX[uart].wr_ptr=0;
    UART_RX[uart].status=RX_EMPTY;
  }

  return(ret_wartosc);
}


//--------------------------------------------------------------

//--------------------------------------------------------------
void P_UART_Receive(UART_NAME_t uart, uint16_t wertosc)
{
  if(UART_RX[uart].wr_ptr<RX_BUF_SIZE) {
    // jeli jest miejsce w buforze
    if(UART_RX[uart].status==RX_EMPTY) {
      // jeli identyfikator koÒca stringu nie zosta≥ odebrany
      if((wertosc>=RX_FIRST_CHR) && (wertosc<=RX_LAST_CHR)) {
        // zapisyjemy w buforze
        UART_RX[uart].rx_buffer[UART_RX[uart].wr_ptr]=wertosc;
        UART_RX[uart].wr_ptr++;
      }
      if(wertosc==RX_END_CHR) {
        // jeli znacznik koÒca stringu siÍ pojawi zapisujemy do bufora
        UART_RX[uart].rx_buffer[UART_RX[uart].wr_ptr]=wertosc;
        UART_RX[uart].status=RX_READY;
      }
    }
  }
  else {
    // bufor pe≥ny
    UART_RX[uart].status=RX_FULL;
  }
}


//--------------------------------------------------------------

//--------------------------------------------------------------
void P_UART_RX_INT(uint8_t int_nr, uint16_t wert)
{
  UART_NAME_t nr;

  for(nr=0;nr<UART_ANZ;nr++) {
    if(UART[nr].INT==int_nr) {
      P_UART_Receive(nr,wert);
      break;
    }
  }
}



//--------------------------------------------------------------
// UART1-Interrupt
//--------------------------------------------------------------
void USART1_IRQHandler(void) {
  uint16_t wartosc;

  if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
    // gdy mamy bajt w buforze odbiorczym
	  wartosc=USART_ReceiveData(USART1);
    // zapis bajtu
    P_UART_RX_INT(USART1_IRQn,wartosc);
  }
}

