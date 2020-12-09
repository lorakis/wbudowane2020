#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
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

void timer1_init()
{
  // ustaw tryb licznika
  // WGM1  = 0000 -- normal
  // CS1   = 001  -- prescaler 1
  TCCR1B = _BV(CS10);
}
FILE uart_file;

int main()
{
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  // zainicjalizuj licznik
  timer1_init();
  // program testowy
  volatile int8_t a1, a2, a3;
  volatile int16_t b1, b2, b3;
  volatile int32_t c1, c2, c3;
  volatile int64_t d1, d2, d3;
  volatile float e1, e2, e3;

  uint16_t a = TCNT1; // wartość licznika przed czekaniem
  a3 = a1+a2;
  uint16_t b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dodawania int8_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  a3 = a1-a2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas odejmowania int8_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  a3 = a1*a2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas mnożenia int8_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  a3 = a1/a2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dzielenia int8_t: %"PRIu16" cykli\r\n", b - a);



  a = TCNT1; // wartość licznika przed czekaniem
  b3 = b1+b2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dodawania int16_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  b3 = b1-b2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas odejmowania int16_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  b3 = b1*b2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas mnożenia int16_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  b3 = b1/b2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dzielenia int16_t: %"PRIu16" cykli\r\n", b - a);



  a = TCNT1; // wartość licznika przed czekaniem
  c3 = c1+c2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dodawania int32_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  c3 = c1-c2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas odejmowania int32_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  c3 = c1*c2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas mnożenia int32_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  c3 = c1/c2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dzielenia int32_t: %"PRIu16" cykli\r\n", b - a);



  a = TCNT1; // wartość licznika przed czekaniem
  d3 = d1+d2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dodawania int64_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  d3 = d1-d2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas odejmowania int64_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  d3 = d1*d2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas mnożenia int64_t: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  d3 = d1/d2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dzielenia int64_t: %"PRIu16" cykli\r\n", b - a);


  a = TCNT1; // wartość licznika przed czekaniem
  e3 = e1+e2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dodawania float: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  e3 = e1-e2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas odejmowania float: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  e3 = e1*e2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas mnożenia float: %"PRIu16" cykli\r\n", b - a);

  a = TCNT1; // wartość licznika przed czekaniem
  e3 = e1/e2;
  b = TCNT1; // wartość licznika po czekaniu
  printf("Czas dzielenia float: %"PRIu16" cykli\r\n", b - a);


  while(1);

}
