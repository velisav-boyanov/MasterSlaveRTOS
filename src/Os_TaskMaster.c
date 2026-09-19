#include "Os_Task.h"

//local stuff
static deviceAState currentState = DEVICE_A_IDLE;
static QueueHandle_t deviceQueue;
static QueueHandle_t deviceQueueMasterToSlave;
static SemaphoreHandle_t printMutex;

static void deviceASetState(deviceAState newState)
{
    if ((currentState != newState) && IS_DEVICE_A_STATE(newState))
    {    
        currentState = newState;

        if(newState != DEVICE_A_ERROR)
        {
            LOG_MSG(printMutex, "[Device A] Switched to %s\n", DEVICE_A_STATE_STRING(newState));    
        }
        else
        {
            LOG_ERR(printMutex, "[Device A] Switched to %s\n", DEVICE_A_STATE_STRING(newState));
        }
    }
}

static void deviceAMainFunction(void *vpParams)
{
    unsigned int u32randomVal;
    deviceBState message;
    const deviceBState setDeviceBState = DEVICE_B_SLEEP;

    printMutex = xSemaphoreCreateMutex();

    if (printMutex == NULL)
    {
        LOG_ERR(printMutex, "Could not create print mutex\n");
        //log error
        return;
    }

    LOG_MSG(printMutex, "[Device A] Started in %s\n", DEVICE_A_STATE_STRING(currentState));

    do
    {
        //polling the queue
        if (xQueueReceive(deviceQueue, &message, 0) == pdPASS)
        {
            LOG_MSG(printMutex, 
                "[Device A] Received B state: %s\n",
                DEVICE_B_STATE_STRING(message)
            );

            //recover if needed
            if (currentState == DEVICE_A_ERROR)
            {
                u32randomVal = rand() % 5;//0, 1, 2, 3, 4 possible values
                if (u32randomVal != 0)//20% to fail recovery
                {
                    deviceASetState(DEVICE_A_IDLE);
                    LOG_MSG(printMutex,"[Device A] Recovered\n");        
                }
                else
                {
                    LOG_ERR(printMutex,"[Device A] failed to recover\n");//log error
                }

                if (message == DEVICE_B_FAULT)
                {
                    xQueueSend(
                        deviceQueueMasterToSlave,
                        &setDeviceBState,
                        0
                    );
                }
            }

            if (currentState != DEVICE_A_ERROR)
            {
                switch(message)
                {
                    case DEVICE_B_SLEEP://try to switch to IDLE, if already in IDLE nothing will change
                        if (currentState == DEVICE_A_PROCESSING)
                        {
                            LOG_MSG(printMutex,"[Device A] finished succesful operation\n");//log info successful operation
                        }
                        deviceASetState(DEVICE_A_IDLE);
                        break;
                    case DEVICE_B_ACTIVE://in case of ACTIVE on B 
                        if (currentState == DEVICE_A_PROCESSING)
                        {
                            LOG_WRN(printMutex,"[Device A] continues running operation\n");//log info continues operation
                        }
                        deviceASetState(DEVICE_A_PROCESSING);
                        break;
                    case DEVICE_B_FAULT://switch to error, then log
                        if (currentState == DEVICE_A_PROCESSING)
                        {
                            LOG_ERR(printMutex,"[Device A] failed operation\n");//log error failed operation
                        }
                        deviceASetState(DEVICE_A_ERROR);
                        break;
                }
            }
        }
        /* Device A waits for 1 second */
        vTaskDelay(pdMS_TO_TICKS(DEVICE_A_DELAY));
    }while(1);
}

void deviceAStart(QueueHandle_t queue1, QueueHandle_t queue2)
{
    deviceQueue = queue1;
    deviceQueueMasterToSlave = queue2;

    xTaskCreate(
        deviceAMainFunction,
        "DeviceA",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        NULL
    );
}