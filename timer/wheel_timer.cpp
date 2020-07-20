#include "wheel_timer.h"
#include "ntx_time.h"
void wheel_timer_manager::init()
{
    round = 0;
    for (int i = 0; i < 100; ++i)
    {
        m_millisecond_timers[i] = nullptr;
    }
    for (int i = 0; i < 60; ++i)
    {
        m_second_timers[i] = nullptr;
    }
    for (int i = 0; i < 60; i++)
    {
        m_minute_timers[i] = nullptr;
    }
    for (int i = 0; i < 24; i++)
    {
        m_hour_timers[i] = nullptr;
    }
    for (int i = 0; i < 365; i++)
    {
        m_day_timers[i] = nullptr;
    }
    m_timeout_list = new wheel_timer_node;
    m_timeout_list->next = NULL;
}
void wheel_timer_manager::add_timer_node(void *data, timer_cb_t cb, uint32_t later_seconds, uint32_t later_milli_seconds)
{
    if (later_seconds == 0 && later_milli_seconds == 0)
    {
        printf("ERROR: invalid args addtimer sec:%d millisec:%d\n", later_seconds, later_milli_seconds);
        return;
    }
    struct timeval now_tv;
    gettimeofday(&now_tv, NULL);
    wheel_timer_node *new_tm_node = new wheel_timer_node;
    new_tm_node->data = data;
    new_tm_node->callback = cb;
    new_tm_node->seconds = later_seconds;
    new_tm_node->milliseconds = later_milli_seconds;
    new_tm_node->create_tv = now_tv;
    new_tm_node->timeout_tv = ntx_later_time(&now_tv, later_seconds, later_milli_seconds);
    new_tm_node->next = NULL;
    //处理每一个定时器链表
    wheel_timer_node *idx_node;
    if (later_milli_seconds > 0 && later_seconds == 0)
    {
        //毫秒链表
        if (later_milli_seconds > 1000)
            return;
        uint32_t idx = later_milli_seconds / 10;
        //如果毫秒链表中idx位置已经有任务了
        if (m_millisecond_timers[idx])
        {
            //这边采用头插法，因为过期时间都是一样的
            idx_node = m_millisecond_timers[idx];
            if (!idx_node->next)
            {
                idx_node->next = new_tm_node;
            }
            else
            {
                new_tm_node->next = idx_node->next;
                idx_node->next = new_tm_node;
            }
        }
        else
        {
            m_millisecond_timers[idx] = new_tm_node;
        }
    }
    else if (later_seconds < 60 && later_seconds > 0)
    {
        //秒链表
        uint32_t idx = later_seconds - 1;
        if (m_second_timers[idx])
        {
            //这边采用头插法，因为过期时间都是一样的
            idx_node = m_second_timers[idx];
            if (!idx_node->next)
            {
                idx_node->next = new_tm_node;
            }
            else
            {
                new_tm_node->next = idx_node->next;
                idx_node->next = new_tm_node;
            }
        }
        else
        {
            m_second_timers[idx] = new_tm_node;
        }
    }
    else if (later_seconds >= 60 && later_seconds < 3600)
    {
        //分钟链表
        uint32_t idx = later_seconds / 60 - 1;
        if (m_minute_timers[idx])
        {
            //这边采用头插法，因为过期时间都是一样的
            idx_node = m_minute_timers[idx];
            if (!idx_node->next)
            {
                idx_node->next = new_tm_node;
            }
            else
            {
                new_tm_node->next = idx_node->next;
                idx_node->next = new_tm_node;
            }
        }
        else
        {
            m_minute_timers[idx] = new_tm_node;
        }
    }
    else if (later_seconds >= 3600 && later_seconds < 3600 * 24)
    {
        uint32_t idx = later_seconds / 3600 - 1;
        if (m_hour_timers[idx])
        {
            //这边采用头插法，因为过期时间都是一样的
            idx_node = m_hour_timers[idx];
            if (!idx_node->next)
            {
                idx_node->next = new_tm_node;
            }
            else
            {
                new_tm_node->next = idx_node->next;
                idx_node->next = new_tm_node;
            }
        }
        else
        {
            m_hour_timers[idx] = new_tm_node;
        }
    }
    else if (later_seconds >= 3600 * 24 && later_seconds < 3600 * 24 * MAX_TIMER_DAYS)
    {
        uint32_t idx = later_seconds / (3600 * 24) - 1;
        if (m_day_timers[idx])
        {
            //这边采用头插法，因为过期时间都是一样的
            idx_node = m_day_timers[idx];
            if (!idx_node->next)
            {
                idx_node->next = new_tm_node;
            }
            else
            {
                new_tm_node->next = idx_node->next;
                idx_node->next = new_tm_node;
            }
        }
        else
        {
            m_day_timers[idx] = new_tm_node;
        }
    }
    else if (later_seconds >= 3600 * 24 * MAX_TIMER_DAYS)
    {
        printf("Timer's time overflow!");
    }
}

void wheel_timer_manager::deal_timeout()
{
    show_timer_info();
    std::string now_time_string = timestamp_tostring(now_time());
    //处理每个到点的任务
    wheel_timer_node *timeout_node = m_timeout_list->next;
    while (true)
    {
        if (timeout_node == NULL)
            break;
        //暂时采用单线程处理，后续采用线程池处理
        wheel_timer_node *cur_node = timeout_node;
        std::string create_time_string = timestamp_tostring(cur_node->create_tv.tv_sec);
        std::string timeout_time_string = timestamp_tostring(cur_node->timeout_tv.tv_sec);
        printf("This task will be finished at sec:%d usec:%d round:%d, nowtm:%s timer_timeout_tm:%s, create_tm:%s\n",
               timeout_node->seconds, timeout_node->milliseconds, round, now_time_string.c_str(), timeout_time_string.c_str(), create_time_string.c_str());
        cur_node->callback(cur_node->data);
        timeout_node = timeout_node->next;
        delete_timer_node(cur_node);
    }
    m_timeout_list->next = NULL;
    wheel_timer_node* milli_node=m_millisecond_timers[0]->next;
    while(true){
        if(milli_node==NULL){
            break;
        }
        wheel_timer_node* next_milli_node=milli_node->next;
        m_millisecond_timers[0]->next=milli_node->next;
    }
}
void wheel_timer_manager::delete_timer_node(wheel_timer_node *timer_node)
{
    delete timer_node;
    timer_node = NULL;
}