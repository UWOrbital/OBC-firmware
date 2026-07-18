/**
 * Bench test for the ADCS Rev 1 magnetorquer H-bridges (BD6210F-E2), driven
 * from an RM46 LaunchPad through the bd621x magnetorquer driver.
 *
 * X axis wiring (pwm2/pwm3 reach LaunchPad header J4; see bd621x.h for how
 * the pwm channel -> N2HET pin mapping is derived):
 *   FIN : pwm2 -> N2HET1[12] -> J4 pin 6 -> ADCS H1 pin 17 (FIN)
 *   RIN : pwm3 -> N2HET1[14] -> J4 pin 5 -> ADCS H1 pin 18 (RIN)
 *   5V  : bench supply       -> ADCS H1 pin 38
 *   GND : LaunchPad J5 pin 1 + bench supply return -> ADCS H1 pin 20
 *
 * Sequence per cycle: forward 25/50/75/100 % duty, reverse the same, then
 * standby. Watch FIN/RIN on the scope, or the BD6210F outputs (MAG_X+/- on
 * the ADCS J1 connector) once the ADCS board is connected and powered.
 *
 * Debug output: 115200 baud, XDS110 "Class Application/User UART" COM port.
 * LED3 (GIOB[2]) toggles once per completed cycle.
 */

#include "obc_errors.h"
#include "obc_print.h"
#include "bd621x.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <sys_common.h>
#include <sys_core.h>
#include <gio.h>
#include <sci.h>
#include <het.h>

#define STEP_HOLD_MS 100000000U

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

static void setOutput(const char *name, const mtq_t *mtq, int32_t dutyPercent) {
  obc_error_code_t errCode = mtqSetOutput(mtq, dutyPercent);
  sciPrintf("MTQ %s: duty %ld%% -> %s (err %d)\r\n", name, dutyPercent,
            (errCode == OBC_ERR_CODE_SUCCESS) ? "OK" : "FAILED", (int32_t)errCode);
}

static void testAxis(const char *name, const mtq_t *mtq) {
  //   static const int32_t dutySteps[] = {25, 50, 75, 100};
  //   sciPrintf("Axis %s: forward sweep (PWM on FIN, RIN low)\r\n", name);
  //     setOutput(name, mtq, 50);
  sciPrintf("---- Axis %s: reverse sweep (PWM on RIN, FIN low) ----\r\n", name);
  setOutput(name, mtq, 50);

  vTaskDelay(pdMS_TO_TICKS(STEP_HOLD_MS));

  sciPrintf("---- Axis %s: reverse sweep (PWM on RIN, FIN low) ----\r\n", name);
  setOutput(name, mtq, -50);
}

static void vMtqTestTask(void *pvParameters) {
  const mtq_t mtqX = {
      .hetRam = hetRAM1, .hetReg = hetREG1, .finPwm = pwm2, .rinPwm = pwm3, .finPin = 12U, .rinPin = 14U};

  sciPrintf("\r\nBD621x magnetorquer PWM bench test\r\n");
  //   sciPrintf("PWM: 50 us / 20 kHz, X axis on J4-6 (FIN) / J4-5 (RIN)\r\n");

  obc_error_code_t errCode = mtqInit(&mtqX);
  sciPrintf("mtqInit X: err %d\r\n", (int32_t)errCode);

  uint32_t led = 0;
  while (1) {
    testAxis("X", &mtqX);

    led ^= 1U;
    gioSetBit(gioPORTB, 2, led);
    sciPrintf("==== Cycle complete, restarting ====\r\n");
  }
}

int main(void) {
  gioInit();
  sciInit();
  hetInit();

  initSciPrint();

  xTaskCreateStatic(vMtqTestTask, "mtq_test", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;
}
