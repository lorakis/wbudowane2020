#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem


#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define pom 20

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

float zmierz()
{
  ADCSRA |= _BV(ADSC); // wykonaj konwersję
  while (!(ADCSRA & _BV(ADIF))); // czekaj na wynik
  ADCSRA |= _BV(ADIF); // wyczyść bit ADIF (pisząc 1!)
  return (((float)ADC * (1.1 / 1024.0)) - 0.5)/0.01;
}


void nagrzewaj(float do_temp)
{
  float teraz_temp = zmierz();
  PORTB |= _BV(PB5);
  while(teraz_temp < do_temp)
  {
    _delay_ms(100);
    teraz_temp = zmierz();
    printf("Nagrzewanie: %f°C/%f°C \r\n", teraz_temp, do_temp);
  }
  PORTB &= ~_BV(PB5);
}

void studz(float do_temp)
{
  float teraz_temp = zmierz();
  while(teraz_temp >= do_temp - 1)
  {
    _delay_ms(100);
    teraz_temp = zmierz();
    printf("Studzienie: %f°C/%f°C \r\n", teraz_temp, do_temp-1);
  }
}


FILE uart_file;
int main()
{
  DDRB |= _BV(PB5);
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  // zainicjalizuj ADC
  adc_init();
  float cel = 29;
  while(1)
  {
    float temp = zmierz();
    if(temp < cel)
      nagrzewaj(cel);
    else
      studz(cel);
  }

}
