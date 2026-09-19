#include "Os_Task.h"

//local stuff
static deviceBState currentState = DEVICE_B_SLEEP;
static QueueHandle_t deviceQueue;
static QueueHandle_t deviceQueueMasterToSlave;
static int iteration = 0;

static void deviceBSetState(deviceBState newState)
{
    if ((currentState != newState) && IS_DEVICE_B_STATE(newState))
    {    
        currentState = newState;

        if (newState != DEVICE_B_FAULT)
        {
            LOG_MSG("[Device B] Switched to %s\n", DEVICE_B_STATE_STRING(newState));    
        }
        else
        {
            LOG_ERR("[Device B] Switched to %s\n", DEVICE_B_STATE_STRING(newState));
        }
        
        newState = DEVICE_B_FAULT;
        xQueueSend(
            deviceQueue,
            &newState,
            0
        );
    }
}

static void deviceBMainFunction(void *vpParams)
{
    unsigned int u32randomVal;
    deviceBState message;
    SemaphoreHandle_t printMutex;

    printMutex = xSemaphoreCreateMutex();

    if (printMutex == NULL)
    {
        LOG_ERR("Could not create print mutex\n");
        //log error
        return;
    }

    LOG_MSG("[Device B] Started in %s\n", DEVICE_B_STATE_STRING(currentState));

    do
    {
        iteration++;
        LOG_MSG("[Device B] iteration: %d\n", iteration);

        xSemaphoreTake(printMutex, portMAX_DELAY);

        xSemaphoreGive(printMutex);
        
        //polling the queue
        if (xQueueReceive(deviceQueueMasterToSlave, &message, 0) == pdPASS)
        {
            LOG_WRN("[Device B] Restarted by Device A\n");
            deviceBSetState(message);
        }

        switch(currentState)
        {
            case DEVICE_B_SLEEP:
                u32randomVal = rand() % 2;//change state to ACTIVE or stay in SLEEP
                if (u32randomVal) {deviceBSetState(DEVICE_B_ACTIVE);}
                break;
            case  DEVICE_B_ACTIVE:
                u32randomVal = rand() % 3;//66% to change state, split between FAULT and SLEEP
                if (u32randomVal == 1)     {deviceBSetState(DEVICE_B_FAULT);}
                else if(u32randomVal == 2) {deviceBSetState(DEVICE_B_SLEEP);}   
                break;
            case  DEVICE_B_FAULT:
                u32randomVal = rand() % 2;//change state to SLEEP or stay in FAULT
                if (u32randomVal) {deviceBSetState(DEVICE_B_SLEEP);}
                break;
        }

        /* Device B waits for 1.5 seconds */
        vTaskDelay(pdMS_TO_TICKS(CALC_DELAY(DEVICE_B_DELAY, u32randomVal)));
    }while(1);
}

//global functions
void deviceBStart(QueueHandle_t queue1, QueueHandle_t queue2)
{
    deviceQueue = queue1;
    deviceQueueMasterToSlave = queue2;

    xTaskCreate(
        deviceBMainFunction,
        "DeviceB",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        NULL
    );
}