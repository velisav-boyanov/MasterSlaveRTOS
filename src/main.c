#include "Os_Task.h"

QueueHandle_t deviceQueue;
QueueHandle_t deviceQueueMasterToSlave;

int main(void)
{
    deviceQueue = xQueueCreate(10, sizeof(deviceBState));
    deviceQueueMasterToSlave = xQueueCreate(10, sizeof(deviceBState));

    //run test A
    #ifdef TEST2
    LOG_MSG("Running error test...\n");
    deviceAStart(deviceQueue, deviceQueueMasterToSlave);
    deviceBStart(deviceQueue, deviceQueueMasterToSlave, ERR_STATE);
    
    vTaskStartScheduler();
    #endif //TEST2

    //run test B
    #ifdef TEST1
    LOG_MSG("Running robustness test...\n");
    deviceAStart(deviceQueue, deviceQueueMasterToSlave);
    deviceBStart(deviceQueue, deviceQueueMasterToSlave, REG_STATE);

    vTaskStartScheduler();
    #endif //TEST1

    LOG_ERR("Scheduler stopped!\n");

    return 1;
}
