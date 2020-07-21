#ifndef WHEEL_TIMER_H
#define WHEEL_TIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

//定义两级大小
const uint16_t TVN_BITS = 6;
const uint16_t TVR_BITS = 8;
const uint32_t TVN_SIZE = 1 << TVN_BITS; //二三四五级时间轮的大小
const uint32_t TVR_SIZE = 1 << TVR_BITS; //一级时间轮的大小
const uint32_t TVN_MASK = TVN_SIZE - 1;
const uint32_t TVR_MASK = TVR_SIZE - 1;
typedef void (*timer_cb_t)(void *);

typedef struct LIST_TIMER
{
    struct LIST_TIMER *next;
    struct LIST_TIMER *prev;
} LISTTIMER;

typedef struct WHEEL_TIMER_NODE
{
    LISTTIMER listTImer; //双向链表的头部
    uint32_t timeout_tv; //定时器过期的时间
    uint32_t period_tv;  //触发几次
    void *data;          //参数值
    timer_cb_t callback; //回调函数

} TIMERNODE;

class WHEEL_TIMER_MANAGER
{
public:
    void InitListTimer(LISTTIMER *listtimer, uint32_t size);   //初始化一级时间轮
    void DeleteListTimer(LISTTIMER *listtimer, uint32_t size); //删除一级时间轮
    void ListTimerReplace(LISTTIMER *pOld, LISTTIMER *pNew);   //替换pOld
    void AddTimerNode(TIMERNODE timer_node);
    void CreateTimer(timer_cb_t callback, void *param, uint32_t DuringTime, uint32_t PeridTime);
    void delete_timer_node(WHEEL_TIMER_NODE *timer_node);
    void show_timer_info();
    void deal_timeout();
    uint32_t GetBaseTimerOld();
    uint32_t GetBaseTime(); //获取基准时间
    void ListTimerInsert(LISTTIMER *pNew, LISTTIMER *pPrev, LISTTIMER *pNext);
    void ListTimerInsertHead(LISTTIMER *pNew, LISTTIMER *pHead);
    void ListTimerInsertTail(LISTTIMER *pNew, LISTTIMER *pTail);
    WHEEL_TIMER_MANAGER(/* args */);
    ~WHEEL_TIMER_MANAGER(); //析构函数这边需要删除整个时间管理器

private:
    //仿照Linux实现五个级别的时间轮，表示更大的时间
    LISTTIMER listTimer1[TVR_SIZE]; //一级时间轮，2^8=256毫秒
    LISTTIMER listTimer2[TVN_SIZE]; //二级时间轮，256～256*64-1毫秒
    LISTTIMER listTimer3[TVN_SIZE]; //三级时间轮，256*64 ~ 256*64*64-1
    LISTTIMER listTimer4[TVN_SIZE]; //四级时间轮，256*64*64 ~ 256*64*64*64-1
    LISTTIMER listTimer5[TVN_SIZE]; //五级时间轮，256*64*64*64 ~ 256*64*64*64*64-1

    uint32_t base_time; //基准时间
};

WHEEL_TIMER_MANAGER::WHEEL_TIMER_MANAGER(/* args */)
{
}

WHEEL_TIMER_MANAGER::~WHEEL_TIMER_MANAGER()
{
}

#endif