#ifndef _UART_H
#define _UART_H

#include <avr/interrupt.h>
#include "queue.h"
void uart_init(void);

FILE uart_file;
QueueHandle_t in;
QueueHandle_t out;

#endif
