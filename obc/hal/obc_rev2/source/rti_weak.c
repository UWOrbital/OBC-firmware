#include <FreeRTOSConfig.h>
#if (configGENERATE_RUN_TIME_STATS == 1)

#include <rti.h>


#define RTI_BASE_MEMORY_ADDRESS 0xFFFFFC00u
#define RTI_GLCTRL_ENABLE_COUNTER1_MASK 0x00000002u

#define RTI_RTOS_TICK_COMPARE_CNT (configCPU_CLOCK_HZ / configTICK_RATE_HZ)
#define RTI_RUNTIME_STATS_COMPARE_CNT (RTI_RTOS_TICK_COMPARE_CNT / 50)

#define RTIFRC1 *((volatile uint32_t*)(RTI_BASE_MEMORY_ADDRESS + 0x00000030u))
#define RTIUC1 *((volatile uint32_t*)(RTI_BASE_MEMORY_ADDRESS + 0x00000034u))
#define RTICPUC1 *((volatile uint32_t*)(RTI_BASE_MEMORY_ADDRESS + 0x00000038u))

#define RTI_GCTRL_REG *((volatile uint32_t*)0xFFFFFC00)    // Used to disable/enable the counter and timer
#define RTI_SETINTENA_REG *((volatile uint32_t*)0xFFFFFC80)
#define RTI_CLEARINTENA_REG *((volatile uint32_t*)0xFFFFFC84)
#define RTI_INTFLAG_REG *((volatile uint32_t*)0xFFFFFC88)

uint32_t rtiGetCounterTick() { return RTIFRC1; }

/**
 * @brief Resets all of RTI Counter 1. Disables the counter until enabled again 
 * using rtiStartCounter.
 * @param counter The counter to reset. Must be 1 for succesful reset.
 * @return A 32-bit value of 0 to indicate success.
 */
__attribute__((weak)) uint32_t rtiResetCounter(uint32_t counter) {
  if (counter != rtiCOUNTER_BLOCK1) return 1;

  // Disable the timer Counter 1
  RTI_GCTRL_REG &= ~(RTI_GLCTRL_ENABLE_COUNTER1_MASK);

  // Preset Free Counting Register and Up Counter.
  RTIFRC1 = 0x00000000;
  RTIUC1 = 0x00000000;
  RTICPUC1 = 0x00000000;

  // Clear current Counter 1 overflow interrupts
  RTI_INTFLAG_REG = (0x00000001 << 18);

  // Disable future Counter 1 overflow interrupts
  RTI_CLEARINTENA_REG = (0x00000001 << 18);
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
  RTICPUC1 = RTI_RUNTIME_STATS_COMPARE_CNT;
}

/**
 * @brief Renables the counter.
 * @param counter The counter to start. Must be 1 for succesful enable.
 * @return Returns void
 */
__attribute__((weak)) void rtiStartCounter(uint32_t counter) {
  if (counter != rtiCOUNTER_BLOCK1) return;

  // Start the timer
  RTI_GCTRL_REG |= RTI_GLCTRL_ENABLE_COUNTER1_MASK;
}

/**
 * @brief Disables the counter block RTI 1.
 * @param counter The counter to stop. Must be 1 for succesful disable
 * @return Returns void
 */
__attribute__((weak)) void rtiStopCounter(uint32_t counter) {
  if (counter != rtiCOUNTER_BLOCK1) return;

  // Disable the timer
  RTI_GCTRL_REG &= ~(RTI_GLCTRL_ENABLE_COUNTER1_MASK);
}

#endif
