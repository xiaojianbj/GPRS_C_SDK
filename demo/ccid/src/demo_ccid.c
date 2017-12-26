
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "api_os.h"
#include "api_debug.h"
#include "api_event.h"
#include "api_sim.h"



#define MAIN_TASK_STACK_SIZE    (2048 * 2)
#define MAIN_TASK_PRIORITY      0
#define MAIN_TASK_NAME          "CCID Test Task"

static HANDLE mainTaskHandle = NULL;
static HANDLE ccidTaskHandle = NULL;



void CcidRead(void* param)
{
    uint8_t ccid[21];
    while(1)
    {
        memset(ccid,0,sizeof(ccid));
        SIM_GetICCID(ccid);
        Trace(1,"CCID:%s",ccid);
        OS_Sleep(3000);
    }
}


void EventDispatch(API_Event_t* pEvent)
{
    switch(pEvent->id)
    {
        case API_EVENT_ID_NO_SIMCARD:
            Trace(10,"!!NO SIM CARD%d!!!!",pEvent->param1);
            break;

        case API_EVENT_ID_NETWORK_REGISTERED_HOME:
        case API_EVENT_ID_NETWORK_REGISTERED_ROAMING:
            break;
        default:
            break;
    }
}


void ccidTest(void *pData)
{
    API_Event_t* event=NULL;

    ccidTaskHandle = OS_CreateTask(CcidRead,
        NULL, NULL, MAIN_TASK_STACK_SIZE, MAIN_TASK_PRIORITY, 0, 0, MAIN_TASK_NAME);

    while(1)
    {
        if(OS_WaitEvent(mainTaskHandle, &event, OS_TIME_OUT_WAIT_FOREVER))
        {
            EventDispatch(event);
            OS_Free(event->pParam1);
            OS_Free(event);
        }
    }
}

void ccid_Main()
{
    mainTaskHandle = OS_CreateTask(ccidTest,
        NULL, NULL, MAIN_TASK_STACK_SIZE, MAIN_TASK_PRIORITY, 0, 0, MAIN_TASK_NAME);
    OS_SetUserMainHandle(&mainTaskHandle);
}


