//--------------------------------------------------------------
// File     : stm32_ub_uart.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_UART_H
#define __STM32F4_UB_UART_H


//--------------------------------------------------------------
// Inkludy
//--------------------------------------------------------------
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"



//--------------------------------------------------------------

//--------------------------------------------------------------
typedef enum
{
  COM1 = 0   // COM1 (TX=PA9, RX=PA10)
}UART_NAME_t;

#define  UART_ANZ   1 //uart 1



//--------------------------------------------------------------
// status bufora
//--------------------------------------------------------------
typedef enum {
  RX_EMPTY = 0,  // pusty
  RX_READY,      // gotowy
  RX_FULL        // pe≥ny
}UART_RXSTATUS_t;


//--------------------------------------------------------------
// UART_pin
//--------------------------------------------------------------
typedef struct {
  GPIO_TypeDef* PORT;     // Port
  const uint16_t PIN;     // Pin
  const uint32_t CLK;     // Clock
  const uint8_t SOURCE;   // Source
}UART_PIN_t;

//--------------------------------------------------------------
// UART-parametry
//--------------------------------------------------------------
typedef struct {
  UART_NAME_t UART_NAME;    // Name
  const uint32_t CLK;       // Clock
  const uint8_t AF;         // AF
  USART_TypeDef* UART;      // UART
  const uint32_t BAUD;      // Baudrate
  const uint8_t INT;        // Interrupt
  UART_PIN_t TX;            // TX-Pin
  UART_PIN_t RX;            // RX-Pin
}UART_t;


//--------------------------------------------------------------
// bufor i znaczniki
//--------------------------------------------------------------
#define  RX_BUF_SIZE   50    // wielkoc bufora odbiorczego
#define  RX_FIRST_CHR  0x20
#define  RX_LAST_CHR   0x7E
#define  RX_END_CHR    0x0D


//--------------------------------------------------------------
// UART_RX
//--------------------------------------------------------------
typedef struct {
  char rx_buffer[RX_BUF_SIZE]; // RX-bufor
  uint8_t wr_ptr;              // wskaünik zapisu
  UART_RXSTATUS_t status;      // RX-Status
}UART_RX_t;
UART_RX_t UART_RX[1];


//--------------------------------------------------------------
// Funkcje
//--------------------------------------------------------------
void UB_Uart_Init(void);
void UB_Uart_SendByte(UART_NAME_t uart, uint16_t wert);
void UB_Uart_SendString(UART_NAME_t uart, char *ptr);
UART_RXSTATUS_t UB_Uart_ReceiveString(UART_NAME_t uart, char *ptr);



//--------------------------------------------------------------
#endif // __STM32F4_UB_UART_H
