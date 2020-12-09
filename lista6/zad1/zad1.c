#include <avr/io.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem

// inicjalizacja UART
void uart_init()
{
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
  // wyczyść rejestr UCSR0A
  UCSR0A = 0;
  // włącz odbiornik i nadajnik i przerwania
  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(TXCIE0);
  // ustaw format 8n1
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

volatile int data = 0;

// odczyt jednego znaku
ISR(USART_RX_vect)
{

  UCSR0B &= ~_BV(RXCIE0); // wyłączamy przerwanie czytania
  data = UDR0; // odczyt
  UDR0 = data; // transmitujemy
}

// włączenie odczytu
ISR(USART_TX_vect)
{
  UCSR0B |= _BV(RXCIE0); // włączamy przerwanie odczytu
}


int main()
{
  // zainicjalizuj UART
  uart_init();
  sei();
  set_sleep_mode(SLEEP_MODE_IDLE);
  UCSR0B |= _BV(RXCIE0);
  UCSR0B |= _BV(TXCIE0);
  while(1)
  {
    sleep_mode();
  }
}
