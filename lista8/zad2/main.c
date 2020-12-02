/******************************************************************************
 * Header file inclusions.
 ******************************************************************************/

#include "FreeRTOS.h"
#include "task.h"

#include <avr/io.h>
#include "queue.h"

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
    // Create task.
    xTaskHandle blink_handle;
    xTaskHandle serial_handle;
    QueueHandle_t xQueue = xQueueCreate(100, sizeof(uint16_t));

    xTaskCreate
    (
      vSerial,
      "serial",
      configMINIMAL_STACK_SIZE,
      (void *)xQueue,
      mainSERIAL_TASK_PRIORITY,
      &serial_handle
    );
    xTaskCreate
    (
     vBlinkLed,
     "blink",
     configMINIMAL_STACK_SIZE,
     (void *)xQueue,
     mainLED_TASK_PRIORITY,
     &blink_handle
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
    QueueHandle_t xQueue = (QueueHandle_t)pvParameters;
    LED_DDR |= _BV(LED);
    uint16_t value;
    for ( ;; )
    {
        if (xQueueReceive(xQueue, &value, (TickType_t)10) == pdTRUE)
        {
          printf("%"PRIu16"\r\n", value);
          LED_PORT |= _BV(LED);
          vTaskDelay(value / portTICK_PERIOD_MS);
          LED_PORT &= ~_BV(LED);
          vTaskDelay(1000 / portTICK_PERIOD_MS);
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
    uart_init();
    stdin = stdout = stderr = &uart_file;
    QueueHandle_t xQueue = (QueueHandle_t)pvParameters;
    char input;
    uint16_t value;
    for ( ;; )
    {
        value = 0;
        input = getchar();

        while (input >= '0' && input <= '9')
        {
          printf("%"PRIu16"", input - '0');
          value = value * 10 + (input - '0');
          input = getchar();
        }
        printf("\r\n");
        if( xQueueSend( xQueue,
                       ( void * ) &value,
                       ( TickType_t ) 0 ) != pdPASS )
        {
            printf("blad\r\n");
        }
    }
}
