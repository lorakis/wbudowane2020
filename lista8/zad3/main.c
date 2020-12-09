/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/

#include "FreeRTOS.h"
#include "task.h"

#include <avr/io.h>


#include <stdio.h>
#include "uart.h"

/******************************************************************************
 * Private macro definitions.
 ******************************************************************************/

#define mainLED_TASK_PRIORITY   1

#define mainSERIAL_TASK_PRIORITY 2

#define LED PB5
#define LED_DDR DDRB
#define LED_PORT PORTB
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
int main(void)
{
    /* Create task. w uart.h
    QueueHandle_t in;
    QueueHandle_t out; */
    xTaskHandle blink_handle;
    xTaskHandle serial_handle;

    in = xQueueCreate(10, sizeof(char));
    out = xQueueCreate(10, sizeof(char));

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
    DDRB |= _BV(PB5);

    for ( ;; )
    {
        LED_PORT &= ~_BV(LED);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        LED_PORT |= _BV(LED);
        vTaskDelay(500 / portTICK_PERIOD_MS);
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
    uart_init();
    stdin = stdout = stderr = &uart_file;

    char input;

    for ( ;; )
    {
      
      input = getchar;
      for(int i =0; i<10; i++)
        //printf("%d\r\n", i);
        putchar('0'+i);

    }
}

ISR(USART_UDRE_vect)
{
  if(xQueueIsQueueEmptyFromISR(out) == pdFALSE)
    xQueueReceiveFromISR(out, (void*)&UDR0, NULL);
  else
    USCR0B &= ~_BV(UDRIE0);
}

ISR(USART_RX_vect)
{
  xQueueSendFromISR(in, (void*)&UDR0, NULL);
}
