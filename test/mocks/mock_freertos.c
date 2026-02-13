#include <stddef.h>

void* MPU_xQueueCreateMutexStatic(void* pxStaticQueue) { return NULL; }
void* MPU_xQueueGenericCreateStatic(unsigned portBaseType, unsigned portBaseType2, unsigned char ucQueueType,
                                    void* pxStaticQueue, void* pxStaticBuffer) {
  return NULL;
}
int MPU_xQueueGenericReceive(void* xQueue, void* pvBuffer, unsigned int xTicksToWait, unsigned int xJustPeeking) {
  return 0;
}
int MPU_xQueueGenericSend(void* xQueue, const void* pvItemToQueue, unsigned int xTicksToWait,
                          unsigned int xCopyPosition) {
  return 0;
}
void xQueueGiveFromISR(void* xQueue, int* pxHigherPriorityTaskWoken) {}
