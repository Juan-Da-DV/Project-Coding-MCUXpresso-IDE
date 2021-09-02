/*
    Programmer's Block
    File Name: MCO556_Project_Daza
 	Student Name: Juan David Daza Velosa
	Student Number: 108722182
	Date: August 13, 2021
	Description: See the MCO556 Final Project document for details
 */

/* Standard includes. */
#include <assert.h>
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"

TimerHandle_t SwTimerHandle = NULL;

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The software timer period. */
#define SW_TIMER_PERIOD_MS (2000 / portTICK_PERIOD_MS) /*trigger a call back function every 2 seconds*/

/* Task priorities */
#define Task1_PRIORITY (tskIDLE_PRIORITY + 3)
#define Task2_PRIORITY (tskIDLE_PRIORITY + 3)
#define Task3_PRIORITY (tskIDLE_PRIORITY + 1)

/*Semaphores include*/
SemaphoreHandle_t xSemaphore_Task1;
SemaphoreHandle_t xSemaphore_Task2;
SemaphoreHandle_t xSemaphore_Task3;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void Task1(void *pvParameters);
static void Task2(void *pvParameters);
static void Task3(void *pvParameters);

/* The callback function. */
static void SwTimerCallback(TimerHandle_t xTimer);

void BOARD_PIT_1_0_IRQHANDLER(void) /*Interrupt Service Routine to process PIT interrupts*/
	{
		PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag); /*clear PIT interrupt status flag*/
		LED_RED_TOGGLE(); /*Toggles Red Led*/
	}

void BOARD_SW2_IRQHANDLER(void) /*Interrupt Service Routine for SW2*/
	{
		uint32_t flags = GPIO_PortGetInterruptFlags(BOARD_SW2_GPIO); /*get triggered interrupt flag*/
		GPIO_PortClearInterruptFlags(BOARD_SW2_GPIO, flags); /*clear triggered interrupt flag*/
		BaseType_t xHigherPriorityTaskWoken = pdFALSE; /*set to pdFALSE*/
		xTimerStopFromISR( SwTimerHandle, &xHigherPriorityTaskWoken );		/*stop the software timer*/
		PRINTF("\nSW2 ISR - EMERGENCY - all operations are suspended!\n\n");/*prints message*/
		xSemaphoreGiveFromISR(xSemaphore_Task3, &xHigherPriorityTaskWoken);	/* give semaphore to Task3 so it can run before timer stops*/
	}

void BOARD_SW3_IRQHANDLER(void) /*Interrupt Service Routine for SW3*/
	{
		uint32_t flags = GPIO_PortGetInterruptFlags(BOARD_SW3_GPIO); /*get triggered interrupt flag*/
		GPIO_PortClearInterruptFlags(BOARD_SW3_GPIO, flags); /*clear triggered interrupt flag*/
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;/*set to pdFALSE*/
		PRINTF("\nSW3 - NORMAL system operation is resuming...\n\n");/*prints message*/
		LED_RED_OFF();/*Turns Red led OFF*/
		PIT_StopTimer(BOARD_PIT_1_PERIPHERAL, kPIT_Chnl_0); /*stop PIT timer-channel 0*/
		LED_GREEN_ON();/*Turns Green led ON*/
	    GPIO_PinWrite(GPIOC, 16U, 0U); /*make pin PTC16 low using pin number 16 and logic 0 (Normal condition)*/
		xTimerStartFromISR( SwTimerHandle, &xHigherPriorityTaskWoken );/* restart the software timer again to resume normal operation */
	}


int main(void) {

	 /* Init board hardware. */
	    BOARD_InitBootPins();
	    BOARD_InitBootClocks();
	    BOARD_InitBootPeripherals();
	    SystemCoreClockUpdate();

	    /* binary semaphores */
	    xSemaphore_Task1 = xSemaphoreCreateBinary();
	    xSemaphore_Task2 = xSemaphoreCreateBinary();
	    xSemaphore_Task3 = xSemaphoreCreateBinary();

	    /* Create the software timer. */
	    SwTimerHandle = xTimerCreate("SwTimer",          /* Text name. */
	                                 SW_TIMER_PERIOD_MS, /* Timer period. */
	                                 pdTRUE,             /* Enable auto reload. */
	                                 0,                  /* ID is not used. */
	                                 SwTimerCallback);   /* The callback function. */

	    if (xTaskCreate(Task1, "TASK1", configMINIMAL_STACK_SIZE + 128, NULL, Task1_PRIORITY, NULL) != pdPASS)
	       {
	           PRINTF("Task creation failed!.\r\n");
	           while (1)
	               ;
	       }

	    if (xTaskCreate(Task2, "TASK2", configMINIMAL_STACK_SIZE + 128, NULL, Task2_PRIORITY, NULL) != pdPASS)
	       {
	           PRINTF("Task creation failed!.\r\n");
	           while (1)
	               ;
	       }

	   	if (xTaskCreate(Task3, "TASK3", configMINIMAL_STACK_SIZE + 128, NULL, Task3_PRIORITY, NULL) != pdPASS)
	   	   {
	   	       PRINTF("Task creation failed!.\r\n");
	   	        while (1)
	   	           ;
	       }

	    /* Start timer. */
	    xTimerStart(SwTimerHandle, 0);
	    /* Start scheduling. */
	    vTaskStartScheduler();
	    for (;;)
	        ;
	}

	/*!
	 * @brief Software timer callback.
	 */
	static void SwTimerCallback(TimerHandle_t xTimer)
		{
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;/*set to pdFALSE*/
			LED_RED_OFF();		/*Turns OFF Red Led initially*/
			GPIO_PinWrite(GPIOC, 16U, 0U);	/*PTC16 OFF initially*/
			LED_GREEN_ON();		/*Turn ON Green Led initially*/
			PRINTF("Software Timer ISR - sensors are read\r\n");/*prints message*/
			xSemaphoreGiveFromISR(xSemaphore_Task1, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		}

	/* Task 1 */
	void Task1(void *pvParameters)
		{
			for(;;)
			{
			xSemaphoreTake(xSemaphore_Task1, portMAX_DELAY);
			PRINTF("Task1 - sensor data is processed\r\n");/*prints message*/
			xSemaphoreGive(xSemaphore_Task2); /*Task 1 code “give” the second semaphore to Task2*/
			}
		}

	/* Task 2 */
	void Task2(void *pvParameters)
		{
			for(;;)
			{
			xSemaphoreTake(xSemaphore_Task2, portMAX_DELAY); /*Task 2 code, “take” the second semaphore*/
			PRINTF("Task2 - display is updated\r\n");/*prints message*/
			}
		}

	/* Task 3 */
	void Task3(void *pvParameters)
		{
			for(;;)
			{
				xSemaphoreTake(xSemaphore_Task3, portMAX_DELAY);
				PIT_StartTimer(BOARD_PIT_1_PERIPHERAL, kPIT_Chnl_0); /*start PIT timer-channel 0 in order to make the onboard RED LED Blinking*/
				LED_GREEN_OFF();/*Green Led if OFF*/
				GPIO_PinWrite(GPIOC, 16U, 1U);/*make pin PTC16 High using pin number 16 and logic 1*/
				PRINTF("Task3 - EMERGENCY protocol is being executed...\n");/*prints message*/
				PRINTF("        RED LIGHT and SOUND ALARM are ON\n");/*prints message*/
				PRINTF("        Press button SW3 to clear the system and reboot...\n");/*prints message*/
			}
		}
