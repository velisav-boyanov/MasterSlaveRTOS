#include "Os_Task.h"

//local stuff
static deviceBState currentState = DEVICE_B_SLEEP;
static QueueHandle_t deviceQueue;
static QueueHandle_t deviceQueueMasterToSlave;
static uint8 restarts = 0;
static testState deviceUnderTest = REG_STATE;
static SemaphoreHandle_t printMutex;

static void deviceBSetState(deviceBState newState)
{
    char mssg[15] = ""; 

    if (IS_DEVICE_B_STATE(newState))
    {    
        if(currentState != newState)
        {    
            strcpy(mssg, "Switched to");
        }
        else
        {
            strcpy(mssg, "Stays in");
        }

        if (newState != DEVICE_B_FAULT)
        {
            LOG_MSG(printMutex, "[Device B] %s %s\n",mssg, DEVICE_B_STATE_STRING(newState));    
        }
        else
        {
            LOG_ERR(printMutex, "[Device B] %s %s\n",mssg, DEVICE_B_STATE_STRING(newState));
        }

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

    printMutex = xSemaphoreCreateMutex();

    if (printMutex == NULL)
    {
        LOG_ERR(printMutex, "Could not create print mutex\n");
        //log error
        return;
    }

    LOG_MSG(printMutex, "[Device B] Started in %s\n", DEVICE_B_STATE_STRING(currentState));

    do
    {
        //polling the queue
        if (xQueueReceive(deviceQueueMasterToSlave, &message, 0) == pdPASS)
        {
            LOG_WRN(printMutex, "[Device B] Restarted by Device A\n");
            deviceBSetState(message);
            if (deviceUnderTest == ERR_STATE)
            {
                restarts++;
                if (restarts >= 4)//B has been restared by A enough time, stoping simulation
                {   
                    LOG_MSG(printMutex, "[Device B] Stopped error handling test\n\n\n");
                    vTaskEndScheduler();
                    vTaskDelete(NULL);
                }
            }
        }

        if (deviceUnderTest == ERR_STATE) {deviceBSetState(DEVICE_B_FAULT);}
        else
        {    
            switch(currentState)
            {
                case DEVICE_B_SLEEP:
                    //change state to ACTIVE or stay in SLEEP
                    if (rand() % 2) {deviceBSetState(DEVICE_B_ACTIVE);}
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
        }
        vTaskDelay(pdMS_TO_TICKS(DEVICE_B_DELAY));
    }while(1);
}

//global functions
void deviceBStart(QueueHandle_t queue1, QueueHandle_t queue2, testState deviceBTest)
{
    deviceQueue = queue1;
    deviceQueueMasterToSlave = queue2;
    deviceUnderTest = deviceBTest;

    xTaskCreate(
        deviceBMainFunction,
        "DeviceB",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        NULL
    );
}