#include "cli.h"
#include "obc_sci_io.h"
#include "obc_logging.h"
#include "obc_errors.h"
#include "obc_task_config.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <string.h>
#include <stdio.h>

#define CLI_INPUT_BUFF_LEN 64U
#define CLI_OUTPUT_BUFF_LEN 64U

static TaskHandle_t uartCliTaskHandle;
static StaticTask_t uartCliTaskBuffer;
static StackType_t uartCliTaskStack[UART_CLI_TASK_STACK_SIZE];

static void uartCliTask(void * pvParameters);

void initUartConsole(void) {
	ASSERT( (uartCliTaskStack != NULL) && (&uartCliTaskBuffer != NULL) );
	uartCliTaskHandle = xTaskCreateStatic(
		uartCliTask,
		UART_CLI_TASK_NAME,
		UART_CLI_TASK_STACK_SIZE,
		NULL,
		UART_CLI_TASK_PRIORITY,
		uartCliTaskStack,
		&uartCliTaskBuffer
	);
}

static void uartCliTask(void * pvParameters) {
	obc_error_code_t errCode;
	
	char inputBuffer[CLI_INPUT_BUFF_LEN] = {0};
	char outputBuffer[CLI_OUTPUT_BUFF_LEN] = {0};

	LOG_IF_ERROR_CODE(initCli(outputBuffer, CLI_OUTPUT_BUFF_LEN));
	sciPrintText((unsigned char *)outputBuffer, CLI_OUTPUT_BUFF_LEN);

	while (1) {
		vTaskDelay(pdMS_TO_TICKS(1000));

		LOG_IF_ERROR_CODE(sciPrintText((unsigned char *)"\r\n> ", strlen("\r\n> ")));
		if (errCode != OBC_ERR_CODE_SUCCESS) {
			continue;
		}

		LOG_IF_ERROR_CODE(sciRead((unsigned char *)inputBuffer, CLI_INPUT_BUFF_LEN));
		if (errCode != OBC_ERR_CODE_SUCCESS) {
			continue;
		}

		LOG_IF_ERROR_CODE(cliProcessCommand(inputBuffer, strlen(inputBuffer), outputBuffer, CLI_OUTPUT_BUFF_LEN));
		if (errCode != OBC_ERR_CODE_SUCCESS) {
			continue;
		}

		sciPrintText((unsigned char *)outputBuffer, CLI_OUTPUT_BUFF_LEN);
	}
}
