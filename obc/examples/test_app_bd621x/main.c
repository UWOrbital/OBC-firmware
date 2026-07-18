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

/* 50 us -> 20 kHz, the minimum PWM frequency the BD621x allows (20-100 kHz).
 * The HET loop resolution is ~1.16 us, so a 50 us period gives ~43 duty steps
 * (~2.3 % resolution). Raising the frequency reduces duty resolution further. */
#define MTQ_PWM_PERIOD_US 50.0

#define STEP_HOLD_MS 10000U

static StaticTask_t taskBuffer;
static StackType_t taskStack[1024];

static void setAxis(const char *name, const DC_motor_t *mtq, float speed) {
  obc_error_code_t errCode = setMotorSpeed(mtq, speed, MTQ_PWM_PERIOD_US);
  sciPrintf("MTQ %s: duty %d%% -> %s (err %d)\r\n", name, (int32_t)(speed * 100.0f),
            (errCode == OBC_ERR_CODE_SUCCESS) ? "OK" : "FAILED", (int32_t)errCode);
}

static void testAxis(const char *name, const DC_motor_t *mtq) {
  static const float dutySteps[] = {0.25f, 0.5f, 0.75f, 1.0f};
  const uint32_t numSteps = sizeof(dutySteps) / sizeof(dutySteps[0]);

  sciPrintf("---- Axis %s: forward sweep ----\r\n", name);
  for (uint32_t i = 0; i < numSteps; i++) {
    setAxis(name, mtq, dutySteps[i]);
    vTaskDelay(pdMS_TO_TICKS(STEP_HOLD_MS));
  }

  // sciPrintf("---- Axis %s: reverse sweep ----\r\n", name);
  // for (uint32_t i = 0; i < numSteps; i++) {
  //   setAxis(name, mtq, -dutySteps[i]);
  //   vTaskDelay(pdMS_TO_TICKS(STEP_HOLD_MS));
  // }

  // sciPrintf("---- Axis %s: brake (FIN and RIN high) ----\r\n", name);
  // brakeMotor(mtq);
  // vTaskDelay(pdMS_TO_TICKS(STEP_HOLD_MS));

  // sciPrintf("---- Axis %s: coast (FIN and RIN low) ----\r\n", name);
  // setAxis(name, mtq, 0.0f);
  // vTaskDelay(pdMS_TO_TICKS(STEP_HOLD_MS));
}

static void vMtqTestTask(void *pvParameters) {
  // const DC_motor_t mtqX = {.hetBase = hetRAM1, .finPwm = pwm0, .rinPwm = pwm1, .maxSpeed = 1.0f};
  const DC_motor_t mtqY = {.hetBase = hetRAM1, .finPwm = pwm2, .rinPwm = pwm3, .maxSpeed = 1.0f};
  // const DC_motor_t mtqZ = {.hetBase = hetRAM1, .finPwm = pwm4, .rinPwm = pwm6, .maxSpeed = 1.0f};

  sciPrintf("\r\n==== BD621x magnetorquer PWM bench test ====\r\n");
  sciPrintf("PWM period: 50 us (20 kHz)\r\n");

  obc_error_code_t errCode;
  // errCode = startMotor(&mtqX);
  // sciPrintf("startMotor X: err %d\r\n", (int32_t)errCode);
  errCode = startMotor(&mtqY);
  sciPrintf("startMotor Y: err %d\r\n", (int32_t)errCode);
  // errCode = startMotor(&mtqZ);
  // sciPrintf("startMotor Z: err %d\r\n", (int32_t)errCode);

  while (1) {
    // testAxis("X", &mtqX);
    testAxis("Y", &mtqY);
    // testAxis("Z", &mtqZ);

    // sciPrintf("---- All axes: 50%% forward simultaneously ----\r\n");
    // setAxis("X", &mtqX, 0.5f);
    // setAxis("Y", &mtqY, 0.5f);
    // setAxis("Z", &mtqZ, 0.5f);
    // vTaskDelay(pdMS_TO_TICKS(2U * STEP_HOLD_MS));

    // setAxis("X", &mtqX, 0.0f);
    // setAxis("Y", &mtqY, 0.0f);
    // setAxis("Z", &mtqZ, 0.0f);

    sciPrintf("==== Cycle complete, restarting ====\r\n");
  }
}

int main(void) {
  gioInit();
  sciInit();
  hetInit();

  initSciPrint();

  /* hetInit() leaves every N2HET1 pin configured as an input (hetREG1->DIR == 0),
   * so the PWM instructions would run but never drive the pins. Force the six
   * PWM pins used here to output mode. */
  hetREG1->DIR |= (1U << 8) | (1U << 10) | (1U << 12) | (1U << 14) | (1U << 16) | (1U << 18);

  xTaskCreateStatic(vMtqTestTask, "mtq_test", 1024, NULL, 1, taskStack, &taskBuffer);

  vTaskStartScheduler();

  while (1)
    ;
}
