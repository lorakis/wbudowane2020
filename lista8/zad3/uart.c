#include "FreeRTOS.h"
#include "task.h"
#include <avr/io.h>
#include <stdio.h>
#include "uart.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#ifndef BAUD
#define BAUD 9600
#endif
#include <util/setbaud.h>

int uart_transmit(char c, FILE *stream);
int uart_receive(FILE *stream);

FILE uart_file = FDEV_SETUP_STREAM(uart_transmit, uart_receive, _FDEV_SETUP_RW);

void uart_init() {
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= _BV(U2X0);
#else
  UCSR0A &= ~(_BV(U2X0));
#endif
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0) | _BV(UDRIE0); /* Enable RX and TX i włączamy interupty na RX i interupty na UDRE */
}

int uart_transmit(char c, FILE *stream)
{
  /*
  if(uxQueueSpacesAvailable(out) == 0)
    while (!(UCSR0A & _BV(UDRE0))) taskYIELD();
  */
  xQueueSend(out, (void*)&c, (TickType_t)portMAX_DELAY);
  USCR0B |= _BV(UDRIE0);
  return 0;
}

int uart_receive(FILE *stream)
{
  uint8_t data;
  /*
  while(xQueueReceive(in, (void*)&data, (TickType_t)0) == pdFALSE) // xQueueReceiveFromISR useless
    vTaskDelay(10 / portTICK_PERIOD_MS);
    */
  xQueueReceive(in, (void*)&data, (TickType_t)portMAX_DELAY);
  return (char)data;
}
