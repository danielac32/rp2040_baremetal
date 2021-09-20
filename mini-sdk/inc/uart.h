#pragma once
#include "lib.h"
#include "rp2040.h"

void uart_init(uint id, uint baudrate);
uint uart_read(uint id, char *buff, uint len);
uint uart_write(uint id, char *buff, uint len);
void uart_intr_enable(uint id, uint rx, uint tx);
void uart_flush(uint id);
void uart_deinit(uint id);
#define UART_RX0_BUFFER_SIZE 64

#define UART_RX0_BUFFER_MASK (UART_RX0_BUFFER_SIZE - 1)
#define UART_FRAME_ERROR 0x0800
#define UART_OVERRUN_ERROR 0x0400
#define UART_BUFFER_OVERFLOW 0x0200
#define UART_NO_DATA 0x0100

extern uint8_t UART_RxBuf[UART_RX0_BUFFER_SIZE];

extern uint8_t UART_TxHead;
extern uint8_t UART_TxTail;
extern uint8_t UART_RxHead;
extern uint8_t UART_RxTail;
extern uint8_t UART_LastRxError;
uint16_t uart0_available(void);
uint16_t uart0_getc(void);
void irq_uart0();
uint8_t readBytes(char *buffer, uint8_t length);
