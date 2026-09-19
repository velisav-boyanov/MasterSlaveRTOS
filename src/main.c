#include "Os_Task.h"

QueueHandle_t deviceQueue;
QueueHandle_t deviceQueueMasterToSlave;

int main(void)
{
    #ifdef DEBUG
    
    #endif

    deviceQueue = xQueueCreate(10, sizeof(deviceBState));
    deviceQueueMasterToSlave = xQueueCreate(10, sizeof(deviceBState));

    //run test A

    //run test B

    LOG_MSG("Running robustness test...\n");
    deviceAStart(deviceQueue, deviceQueueMasterToSlave);
    deviceBStart(deviceQueue, deviceQueueMasterToSlave);

    vTaskStartScheduler();

    /*
     * We should never get here because the scheduler
     * normally does not return.
     */
    LOG_MSG("ERROR: Scheduler stopped!\n");

    return 1;
}
