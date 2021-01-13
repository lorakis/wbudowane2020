#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include "pid.h"

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem


//! \xrefitem todo "Todo" "Todo list"
#define K_P     20
//! \xrefitem todo "Todo" "Todo list"
#define K_I     5*0.5 //66*0.5
//! \xrefitem todo "Todo" "Todo list"
#define K_D     5*0.12 //66 *0.12

struct PID_DATA pidData;
// inicjalizacja UART
void uart_init()
{
  // ustaw baudrate
  UBRR0 = UBRR_VALUE;
  // włącz odbiornik i nadajnik
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);
  // ustaw format 8n1
  UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// transmisja jednego znaku
int uart_transmit(char data, FILE *stream)
{
  // czekaj aż transmiter gotowy
  while(!(UCSR0A & _BV(UDRE0)));
  UDR0 = data;
  return 0;
}

// odczyt jednego znaku
int uart_receive(FILE *stream)
{
  // czekaj aż znak dostępny
  while (!(UCSR0A & _BV(RXC0)));
  return UDR0;
}

// inicjalizacja ADC
void adc_init()
{
  ADMUX   = _BV(REFS0) | _BV(REFS1);; // referencja AVcc, wejście ADC0
  DIDR0   = _BV(ADC0D); // wyłącz wejście cyfrowe na ADC0
  // częstotliwość zegara ADC 125 kHz (16 MHz / 128)
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADEN); // włącz ADC
}

void timer_init()
{
  // ustaw tryb licznika
  // COM1A = 10   -- non-inverting mode
  // WGM1  = 1110 -- fast PWM top=ICR1
  // CS1   = 101  -- prescaler 1024
  // ICR1  = 15624
  // częstotliwość 16e6/(1024*(1+15624)) = 1 Hz
  // wzór: datasheet 20.12.3 str. 164
  ICR1 = 255;
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM12) | _BV(WGM13) | _BV(CS10) | _BV(CS12);
}

int16_t zmierz()
{
  ADCSRA |= _BV(ADSC); // wykonaj konwersję
  while (!(ADCSRA & _BV(ADIF))); // czekaj na wynik
  ADCSRA |= _BV(ADIF); // wyczyść bit ADIF (pisząc 1!)
  return ADC;
}


FILE uart_file;
int main()
{
  DDRB |= _BV(PB1);
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  pid_Init(K_P * SCALING_FACTOR, K_I * SCALING_FACTOR , K_D * SCALING_FACTOR , &pidData);
  adc_init();
  timer_init();
  int16_t pid_val = 0;
  float temp_docel = 35;
  int16_t temp_docel_adc = ((temp_docel * 0.01 + 0.5) * 1024.0)/1.1;
  while(1)
  {
    int16_t temp_tera_adc = zmierz();
    pid_val = pid_Controller(temp_docel_adc, temp_tera_adc, &pidData) / 128;

    if(pid_val < 0)
      pid_val = 0;
    else if(pid_val > 255)
      pid_val = 255;

    OCR1A = pid_val;
    printf("pid:%"PRIu16" temp_docelowa_adc:%"PRIu16" temp_tera_adc:%"PRIu16"     ", pid_val, temp_docel_adc, temp_tera_adc);
    printf("temp teraz:%f temp docelowa:%f\r\n", (((float)temp_tera_adc * (1.1 / 1024.0)) - 0.5)/0.01, temp_docel);
  }

}
