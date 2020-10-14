#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

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

FILE uart_file;

void kropka()
{
  LED_PORT |= _BV(LED); // on
  _delay_ms(500);
  LED_PORT &= ~_BV(LED); // off
  _delay_ms(200);

}

void kreska()
{
  LED_PORT |= _BV(LED); // on
  _delay_ms(1500);
  LED_PORT &= ~_BV(LED); // off
  _delay_ms(200);

}

void literaNaMorsea(char a)
{
  switch(a)
  {
    case 'a': // ._
      kropka();
      kreska();
      break;
    case 'b': //_...
      kreska();
      kropka();
      kropka();
      kropka();
      break;
    case 'c': //_._.
      kreska();
      kropka();
      kreska();
      kropka();
      break;
    case 'd': //_..
      kreska();
      kropka();
      kropka();
      break;
    case 'e': //.
      kropka();
      break;
    case 'f': //.._.
      kropka();
      kropka();
      kreska();
      kropka();
      break;
    case 'g': //_ _.
      kreska();
      kreska();
      kropka();
      break;
    case 'h': //....
      kropka();
      kropka();
      kropka();
      kropka();
      break;
    case 'i': //..
      kropka();
      kropka();
      break;
    case 'j': //._ _ _
      kropka();
      kreska();
      kreska();
      kreska();
      break;
    case 'k': //_._
      kreska();
      kropka();
      kreska();
      break;
    case 'l': //._..
      kropka();
      kreska();
      kropka();
      kropka();
      break;
    case 'm': //_ _
      kreska();
      kreska();
      break;
    case 'n': //_.
      kreska();
      kropka();
      break;
    case 'o': //_ _ _
      kreska();
      kreska();
      kreska();
      break;
    case 'p': //._ _ .
      kropka();
      kreska();
      kreska();
      kropka();
      break;
    case 'q': //_ _ . _
      kreska();
      kreska();
      kropka();
      kreska();
      break;
    case 'r': //._.
      kropka();
      kreska();
      kropka();
      break;
    case 's': //...
      kropka();
      kropka();
      kropka();
      break;
    case 't': //_
      kreska();
      break;
    case 'u': //.._
      kropka();
      kropka();
      kreska();
      break;
    case 'v': //..._
      kropka();
      kropka();
      kropka();
      kreska();
      break;
    case 'w': //._ _
      kropka();
      kreska();
      kreska();
      break;
    case 'x': //_.._
      kreska();
      kropka();
      kropka();
      kreska();
      break;
    case 'y': //_._ _
      kreska();
      kropka();
      kreska();
      kreska();
      break;
    case 'z': //_ _ ..
      kreska();
      kreska();
      kropka();
      kropka();
      break;
    case ' ':
      _delay_ms(600);
      break;
  }
}


int main() {
  // zainicjalizuj UART
  uart_init();
  // skonfiguruj strumienie wejścia/wyjścia
  fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  stdin = stdout = stderr = &uart_file;
  while(1)
  {
    printf("Wpisz napis:\r\n" );
    char napis[30];
    scanf("%s", napis);
    printf("%s\r\n", napis );
    int dl = strlen(napis);
    for(int i = 0; i < dl; i++)
    {
      literaNaMorsea(napis[i]);
      _delay_ms(200);
    }
  }
}
