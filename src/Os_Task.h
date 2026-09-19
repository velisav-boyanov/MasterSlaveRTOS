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

//user param macros
#define DEBUG          1u
#define DEVICE_A_DELAY 500u
#define DEVICE_B_DELAY 1000u

//user func macros
#define STRING_GEN(STRING)             #STRING
#define CALC_DELAY(INIT_DELAY, PARAM)  (((int)INIT_DELAY)*(((int)PARAM)+1))

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
    #define LOG_MSG(fmt, ...) \
        printf("[INFO]    " fmt, ##__VA_ARGS__)    

    #define LOG_ERR(fmt, ...) \
        printf("[ERROR]   " fmt, ##__VA_ARGS__) 

    #define LOG_WRN(fmt, ...) \
        printf("[WARNING] " fmt, ##__VA_ARGS__)     
#else
    #define LOG_MSG(fmt, ...) ((void)0)
    #define LOG_WRN(fmt, ...) ((void)0)    
    #define LOG_ERR(fmt, ...) ((void)0)    
#endif    

//user funcs
void deviceAStart(QueueHandle_t queue1, QueueHandle_t queue2);
void deviceBStart(QueueHandle_t queue1, QueueHandle_t queue2);

#endif