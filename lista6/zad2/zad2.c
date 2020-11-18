#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem
#define BUFFER_SIZE 5

FILE uart_file;
char in[BUFFER_SIZE];
char out[BUFFER_SIZE];
uint8_t pointer_in = 0;
uint8_t transmit_in = 0;
uint8_t in_counter = 0;
uint8_t pointer_out = 0;
uint8_t transmit_out = 0;
uint8_t out_counter = 0;

// inicjalizacja UART
void uart_init()
{
  	// ustaw baudrate
  	UBRR0 = UBRR_VALUE;
  	// wyczyść rejestr UCSR0A
  	UCSR0A = 0;
  	// włącz odbiornik i nadajnik oraz obsluge przerwan
  	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0) | _BV(UDRIE0);
  	// ustaw format 8n1
  	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// transmisja jednego znaku
int uart_transmit(char data, FILE *stream)
{
	if (out_counter == BUFFER_SIZE) {
		UCSR0B &= ~_BV(UDRIE0);
		while(!(UCSR0A & _BV(UDRE0)));
		UDR0 = buffer_out[transmit_out++];
		if (transmit_out == BUFFER_SIZE)
			transmit_out = 0;
		buffer_out[pointer_out++] = data;
		if (pointer_out == BUFFER_SIZE)
			pointer_out = 0;
		UCSR0B |= _BV(UDRIE0);
	} else {
		buffer_out[pointer_out++] = data;
		if (pointer_out == BUFFER_SIZE) {
			pointer_out = 0;
		}
		out_counter++;
	}
	return 0;
}

// odczyt jednego znaku
int uart_receive(FILE *stream)
{
	if (in_counter == 0) {
		UCSR0B &= ~_BV(RXCIE0);
  		while (!(UCSR0A & _BV(RXC0)));
		char return_val = UDR0;
		UCSR0B |= _BV(RXCIE0);
		return return_val;
	} else {
		//UCSR0B &= ~_BV(RXCIE0);
		char return_val = buffer_in[transmit_in++];
		if (transmit_in == BUFFER_SIZE)
			transmit_in = 0;
		in_counter--;
		//UCSR0B |= _BV(RXCIE0);
		return return_val;
	}
}

ISR(USART_RX_vect)
{
	in[pointer_in] = UDR0;
	pointer_in++;
	if (pointer_in == BUFFER_SIZE)
		pointer_in = 0;
	in_counter++;
}

ISR(USART_UDRE_vect) {
	if (out_counter > 0) {
		UDR0 = buffer_out[transmit_out++];
		out_counter--;
		if (transmit_out == BUFFER_SIZE) {
			transmit_out = 0;
		}
	}
}

int main()
{
  	// zainicjalizuj UART
  	uart_init();
  	// skonfiguruj strumienie wejścia/wyjścia
  	fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
  	stdin = stdout = stderr = &uart_file;
  	// program testowy
	sei();
  	printf("Hello world!\r\n");
	_delay_ms(10);
  	while(1) {
    	int16_t a = 1, b = 0;
    	scanf("%"SCNd16, &a);
		//scanf("%"SCNd16, &b);
    	printf("Odczytano: %"PRId16"\r\n", a);
		printf("Hello world!\r\n");
		_delay_ms(10);
  	}
}
