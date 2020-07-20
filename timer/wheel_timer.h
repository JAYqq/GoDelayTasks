#ifndef WHEEL_TIMER_H
#define WHEEL_TIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

//定义两级大小
const uint16_t TVN_BITS = 6;
const uint16_t TVR_BITS = 8;
const uint32_t TVN_SIZE = 1 << TVN_BITS;
const uint32_t TVR_SIZE = 1 << TVR_BITS;
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
    LISTTIMER listTImer; //双向链表
    uint32_t timeout_tv; //定时器过期的时间
    void *data;          //参数值
    timer_cb_t callback; //回调函数

} TIMERNODE;

class WHEEL_TIMER_MANAGER
{
public:
    void init();
    void add_timer_node(void *data, timer_cb_t cb, uint32_t later_seconds, uint32_t later_milliseconds);
    void delete_timer_node(WHEEL_TIMER_NODE *timer_node);
    void show_timer_info();
    void deal_timeout();
    WHEEL_TIMER_MANAGER(/* args */);
    ~WHEEL_TIMER_MANAGER();//析构函数这边需要删除整个时间管理器

private:
    //仿照Linux实现五个级别的时间轮，表示更大的时间
    LISTTIMER listTimer1[TVR_SIZE]; //一级时间轮，2^8=256毫秒
    LISTTIMER listTimer2[TVN_SIZE]; //二级时间轮，256～256*64-1毫秒
    LISTTIMER listtimer3[TVN_SIZE]; //三级时间轮，256*64 ~ 256*64*64-1
    LISTTIMER listtimer4[TVN_SIZE]; //四级时间轮，256*64*64 ~ 256*64*64*64-1
    LISTTIMER listtimer5[TVN_SIZE]; //五级时间轮，256*64*64*64 ~ 256*64*64*64*64-1

    uint32_t base_time; //基准时间
};

WHEEL_TIMER_MANAGER::WHEEL_TIMER_MANAGER(/* args */)
{
}

WHEEL_TIMER_MANAGER::~WHEEL_TIMER_MANAGER()
{
}

#endif