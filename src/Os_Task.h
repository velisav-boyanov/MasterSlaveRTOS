#ifndef OS_TASK_H
#define OS_TASK_H

//std includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>    

//user includes
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

//user types
typedef unsigned char uint8;

typedef enum
{
    DEVICE_A_IDLE=0,
    DEVICE_A_PROCESSING,
    DEVICE_A_ERROR
} deviceAState;

typedef enum
{
    DEVICE_B_SLEEP=0,
    DEVICE_B_ACTIVE,
    DEVICE_B_FAULT
} deviceBState;

typedef enum
{
    REG_STATE=0,//regular working state used for robustness test
    ERR_STATE//causes device B to always be in error state
} testState;

//user param macros
#define TEST1
//#define TEST2
#define DEBUG          1u
#define DEVICE_A_DELAY 500u
#define DEVICE_B_DELAY 1000u

//user func macros
#define STRING_GEN(STRING)             #STRING

#define DEVICE_B_STATE_STRING(DV_B_ENUM) \
    ((DV_B_ENUM) == DEVICE_B_SLEEP  ? "DEVICE_B_SLEEP" : \
     (DV_B_ENUM) == DEVICE_B_ACTIVE ? "DEVICE_B_ACTIVE" : \
                               "DEVICE_B_FAULT")

#define DEVICE_A_STATE_STRING(DV_A_ENUM) \
    ((DV_A_ENUM) == DEVICE_A_IDLE  ? "DEVICE_A_IDLE" : \
     (DV_A_ENUM) == DEVICE_A_PROCESSING ? "DEVICE_A_PROCESSING" : \
                               "DEVICE_A_ERROR")

#define IS_DEVICE_B_STATE(newState) \
    (((newState) == DEVICE_B_SLEEP) || ((newState) == DEVICE_B_ACTIVE) || ((newState) == DEVICE_B_FAULT))

#define IS_DEVICE_A_STATE(newState) \
    (((newState) == DEVICE_A_IDLE) || ((newState) == DEVICE_A_PROCESSING) || ((newState) == DEVICE_A_ERROR))

//logging macros
#if DEBUG
    #define LOG_PRINT(mutex, prefix, fmt, ...)       \
        do {                                         \
            xSemaphoreTake((mutex), portMAX_DELAY);  \
            printf(prefix fmt, ##__VA_ARGS__);       \
            xSemaphoreGive((mutex));                \
        } while (0)

    #define LOG_MSG(mutex, fmt, ...) \
        LOG_PRINT(mutex, "[INFO]    ", fmt, ##__VA_ARGS__)

    #define LOG_ERR(mutex, fmt, ...) \
        LOG_PRINT(mutex, "[ERROR]   ", fmt, ##__VA_ARGS__)

    #define LOG_WRN(mutex, fmt, ...) \
        LOG_PRINT(mutex, "[WARNING] ", fmt, ##__VA_ARGS__)   
#else
    #define LOG_MSG(fmt, ...) ((void)0)
    #define LOG_WRN(fmt, ...) ((void)0)    
    #define LOG_ERR(fmt, ...) ((void)0)    
#endif    

//user funcs
void deviceAStart(QueueHandle_t queue1, QueueHandle_t queue2);
void deviceBStart(QueueHandle_t queue1, QueueHandle_t queue2, testState deviceBTest);

#endif