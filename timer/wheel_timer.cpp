#include "wheel_timer.h"
uint32_t WHEEL_TIMER_MANAGER::GetBaseTimerOld()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
uint32_t WHEEL_TIMER_MANAGER::GetBaseTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000); // 返回毫秒时间
}
//将pNew插入到pPrev和pNext之间
void WHEEL_TIMER_MANAGER::ListTimerInsert(LISTTIMER *pNew, LISTTIMER *pPrev, LISTTIMER *pNext)
{
    pNext->prev = pNew;
    pNew->next = pNext;
    pPrev->next = pNew;
    pNew->prev = pPrev;
}
void WHEEL_TIMER_MANAGER::ListTimerInsertHead(LISTTIMER *pNew, LISTTIMER *pHead)
{
    ListTimerInsert(pNew, pHead, pHead->next);
}
void WHEEL_TIMER_MANAGER::ListTimerInsertTail(LISTTIMER *pNew, LISTTIMER *pHead)
{
    ListTimerInsert(pNew, pHead->prev, pHead);
}
void WHEEL_TIMER_MANAGER::InitListTimer(LISTTIMER *listtimer, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        listtimer[i].next = &listtimer[i];
        listtimer[i].prev = &listtimer[i];
    }
}
void WHEEL_TIMER_MANAGER::ListTimerReplace(LISTTIMER *pOld, LISTTIMER *pNew)
{
    pNew->next = pOld->next;
    pNew->prev = pOld->prev;
    pNew->next->prev = pNew;
    pNew->prev->next = pNew;
    pOld->next = pOld;
    pOld->prev = pOld;
}
void WHEEL_TIMER_MANAGER::DeleteListTimer(LISTTIMER *listtimer, uint32_t size)
{
    LISTTIMER listTmr, *nextTmr;
    TIMERNODE *timer_node;
    for (uint32_t i = 0; i < size; i++)
    {
        ListTimerReplace(&listtimer[i], &listTmr);
        while (nextTmr != &listTmr)
        {
            //释放节点
            timer_node = (TIMERNODE *)(nextTmr);
            nextTmr = nextTmr->next;
            free(timer_node);
        }
    }
}

void WHEEL_TIMER_MANAGER::AddTimerNode(TIMERNODE timer_node)
{
    LISTTIMER *pHead;
    uint32_t index;
    uint32_t time_out = timer_node.timeout_tv;
    uint32_t time_during = time_out - base_time;
    //一级时间轮
    if (time_during < TVR_SIZE)
    { 
        index = time_out & TVR_MASK;
        pHead = &listTimer1[index];
    }
    //二级时间轮
    else if (time_during < (1 << (TVR_BITS + TVN_BITS)))
    { 
        index=(time_out>>TVR_BITS)&TVN_MASK;
        pHead=&listTimer2[index];
    }
    //三级时间轮
    else if(time_during<(1<<(TVR_BITS+2*TVN_BITS))){
        index=(time_out>>(TVR_BITS+TVN_BITS))&TVN_MASK;
        pHead=&listTimer3[index];
    }
    //四级时间轮
    else if(time_during<(1<<(TVR_BITS+3*TVN_BITS))){
        index=(time_out>>(TVR_BITS+2*TVN_BITS))&TVN_MASK;
        pHead=&listTimer4[index];
    }
    //五级时间轮
    else if(time_during<(1<<(TVR_BITS+4*TVN_BITS))){
        index=(time_out>>(TVR_BITS+3*TVN_BITS))&TVN_MASK;
        pHead=&listTimer5[index];
    }
    else if(time_during<0){
        //如果过期时间是基准时间之前，那就以当前时间为准
        pHead=&listTimer1[(base_time&TVN_MASK)];
    }
    else{
        //如果过期时间超过了限制
        if(time_during>0xffffffffUL){
            time_during=0xffffffffUL;
            time_out=time_during+base_time;
        }
        index=(time_out>>(TVR_BITS+3*TVN_BITS))&TVN_MASK;
        pHead=&listTimer5[index];
    }
    ListTimerInsertTail(&timer_node.listTImer,pHead); //尾插法插入
}
WHEEL_TIMER_MANAGER::WHEEL_TIMER_MANAGER(){
    base_time=GetBaseTime();
    InitListTimer(listTimer1,sizeof(listTimer1)/sizeof(listTimer1[0]));
    InitListTimer(listTimer1,sizeof(listTimer2)/sizeof(listTimer2[0]));
    InitListTimer(listTimer1,sizeof(listTimer3)/sizeof(listTimer3[0]));
    InitListTimer(listTimer1,sizeof(listTimer4)/sizeof(listTimer4[0]));
    InitListTimer(listTimer1,sizeof(listTimer5)/sizeof(listTimer5[0]));
}
WHEEL_TIMER_MANAGER::~WHEEL_TIMER_MANAGER(){
    DeleteListTimer(listTimer1,sizeof(listTimer1)/sizeof(listTimer1[0]));
    DeleteListTimer(listTimer1,sizeof(listTimer2)/sizeof(listTimer2[0]));
    DeleteListTimer(listTimer1,sizeof(listTimer3)/sizeof(listTimer3[0]));
    DeleteListTimer(listTimer1,sizeof(listTimer4)/sizeof(listTimer4[0]));
    DeleteListTimer(listTimer1,sizeof(listTimer5)/sizeof(listTimer5[0]));
}
