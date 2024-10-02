#include <FreeRTOSConfig.h>
#if (ENABLE_TASK_STATS_COLLECTOR == 1)

#include <rti.h>
#include <reg_rti.h>


static volatile rtiBASE_t* const rtiBase = rtiREG1;

#define RTI_GLCTRL_ENABLE_COUNTER1_MASK 2UL
#define RTI_RTOS_TICK_COMPARE_CNT (configCPU_CLOCK_HZ / configTICK_RATE_HZ)
#define RTI_RUNTIME_STATS_COMPARE_CNT (RTI_RTOS_TICK_COMPARE_CNT / 50)

__attribute__((weak)) void vConfigureRuntimeStats(void) {}
__attribute__((weak)) uint32_t ulSystemTickGet(void) {return 0;}

uint32_t rtiGetCounterOneTick() { 
  return rtiBase->CNT[1].FRCx; 
}

/**
 * @brief Resets all of RTI Counter 1. Disables the counter until enabled again 
 * using rtiStartCounter.
 * @param counter The counter to reset. Must be 1 for succesful reset.
 * @return A 32-bit value of 0 to indicate success.
 */
__attribute__((weak)) uint32_t rtiResetCounter(uint32_t counter) {
  if (counter != rtiCOUNTER_BLOCK1) return 1;

  // Disable the timer Counter 1
  rtiBase->GCTRL &= ~(RTI_GLCTRL_ENABLE_COUNTER1_MASK);

  // Preset Free Counting Register and Up Counter.
  rtiBase->CNT[1].FRCx = 0x00000000;
  rtiBase->CNT[1].UCx = 0x00000000;
  rtiBase->CNT[1].CPUCx = 0x00000000;

  // Clear current Counter 1 overflow interrupts
  rtiBase->INTFLAG= (1UL << 18);

  // Disable future Counter 1 overflow interrupts
  rtiBase->CLEARINTENA = (1UL << 18);
  return 0;
}

/**
 * @brief Resets all of RTI Counter 1 and sets the prescaler value to
 * a predefined macro.
 * @return Returns void
 */
__attribute__((weak)) void rtiInit(void) {
  rtiResetCounter(rtiCOUNTER_BLOCK1);

  // Set the prescaler for Counter Block 1
  rtiBase->CNT[1].CPUCx = RTI_RUNTIME_STATS_COMPARE_CNT;
}

/**
 * @brief Renables the counter.
 * @param counter The counter to start. Must be 1 for succesful enable.
 * @return Returns void
 */
__attribute__((weak)) void rtiStartCounter(uint32_t counter) {
  if (counter != rtiCOUNTER_BLOCK1) return;

  // Start the timer
  rtiBase->GCTRL |= RTI_GLCTRL_ENABLE_COUNTER1_MASK;
}

/**
 * @brief Disables the counter block RTI 1.
 * @param counter The counter to stop. Must be 1 for succesful disable
 * @return Returns void
 */
__attribute__((weak)) void rtiStopCounter(uint32_t counter) {
  if (counter != rtiCOUNTER_BLOCK1) return;

  // Disable the timer
  rtiBase->GCTRL &= ~(RTI_GLCTRL_ENABLE_COUNTER1_MASK);
}

#endif
