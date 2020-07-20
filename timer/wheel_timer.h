#ifndef WHEEL_TIMER_H
#define WHEEL_TIMER_H

#include<stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

const uint16_t MAX_TIMER_DAYS=356;
typedef void(*timer_cb_t)(void*);

struct wheel_timer_node
{
    uint32_t node_id;
    uint32_t seconds;
    uint32_t milliseconds;
    uint32_t rotation;
    struct timeval timeout_tv; //定时器过期的时间
    struct timeval create_tv; //创建定时器的时间
    void* data; //参数值
    timer_cb_t callback; //回调函数
    struct wheel_timer_node* next;
};

typedef struct wheel_timer_node* timer_list_t;

class wheel_timer_manager
{
public:
    void init();
    void add_timer_node(void* data,timer_cb_t cb,uint32_t later_seconds,uint32_t later_milliseconds);
    void delete_timer_node(wheel_timer_node* timer_node);
    void show_timer_info();
    void deal_timeout();
    wheel_timer_manager(/* args */);
    ~wheel_timer_manager();
private:
    //过期的任务链表
    timer_list_t m_timeout_list; 

    //毫秒、秒、分、小时、天（链表）
    timer_list_t m_millisecond_timers[100];
    timer_list_t m_second_timers[60];
    timer_list_t m_minute_timers[60];
    timer_list_t m_hour_timers[24];
    timer_list_t m_day_timers[MAX_TIMER_DAYS];

    uint32_t round;
};

wheel_timer_manager::wheel_timer_manager(/* args */)
{
}

wheel_timer_manager::~wheel_timer_manager()
{
}


#endif