/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/

#include "FreeRTOS.h"
#include "task.h"

#include <avr/io.h>
#include <stdbool.h>

#include <stdio.h>
#include "uart.h"

/******************************************************************************
 * Private macro definitions.
 ******************************************************************************/

#define mainLED_TASK_PRIORITY   2

#define mainSERIAL_TASK_PRIORITY 1

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB

#define BTN PB4
#define BTN_PIN PINB
#define BTN_PORT PORTB

#define LED_DDR1 DDRD
#define LED_PORT1 PORTD

/******************************************************************************
 * Private function prototypes.
 ******************************************************************************/

static void vBlinkLed(void* pvParameters);

static void vSerial(void* pvParameters);

/******************************************************************************
 * Public function definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn int main(void)
 *
 * \brief Main function.
 *
 * \return
 ******************************************************************************/
bool tab[102];
int main(void)
{
    // Create task.
    xTaskHandle blink_handle;
    xTaskHandle serial_handle;

    xTaskCreate
        (
         vBlinkLed,
         "blink",
         configMINIMAL_STACK_SIZE,
         NULL,
         mainLED_TASK_PRIORITY,
         &blink_handle
        );

    xTaskCreate
        (
         vSerial,
         "serial",
         configMINIMAL_STACK_SIZE,
         NULL,
         mainSERIAL_TASK_PRIORITY,
         &serial_handle
        );

    // Start scheduler.
    vTaskStartScheduler();

    return 0;
}

/**************************************************************************//**
 * \fn static vApplicationIdleHook(void)
 *
 * \brief
 ******************************************************************************/
void vApplicationIdleHook(void)
{

}

/******************************************************************************
 * Private function definitions.
 ******************************************************************************/

/**************************************************************************//**
 * \fn static void vBlinkLed(void* pvParameters)
 *
 * \brief
 *
 * \param[in]   pvParameters
 ******************************************************************************/
static void vBlinkLed(void* pvParameters)
{
  UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
  LED_DDR1 |= 0b11111111;
  LED_PORT1 |= 0b00000001;
  while(1)
  {
    for(int i = 0; i < 7; i++)
    {
      vTaskDelay(200 / portTICK_PERIOD_MS);
      LED_PORT1 <<= 1;
    }
    for(int i = 0; i < 7; i++)
    {
      vTaskDelay(200 / portTICK_PERIOD_MS);
      LED_PORT1 >>= 1;
    }
  }
}


/**************************************************************************//**
 * \fn static void vSerial(void* pvParameters)
 *
 * \brief
 *
 * \param[in]   pvParameters
 ******************************************************************************/
static void vSerial(void* pvParameters)
{
  BTN_PORT |= _BV(BTN);
  LED_DDR |= _BV(LED);
  int i = 0;
  while(1)
  {

    if( tab[(i+1) % 101] == true)
      LED_PORT &= ~_BV(LED);
    else
      LED_PORT |= _BV(LED);

    if (BTN_PIN & _BV(BTN))
      tab[i] = true;
    else
      tab[i] = false;
    i++;
    if( i >= 101 )
    {
      i = 0;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

}
