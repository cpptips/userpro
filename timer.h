#ifndef __TIMER_H__
#define __TIMER_H__

#include "queue.h"

// 定时器
struct timer_struct {
    int epfd;
    int cycle_time;
    int isstop;
    queue timer_queue;  //节点队列
};

struct timer_data {
    int cycle;
    int time;
    queue_node qnode;
    void (*call_back)(void *arg);  //回调函数
    void *arg;                     //回调参数
};

int init_timer(struct timer_struct *p, int cycle_time);
int add_timer(struct timer_struct *p, int cycle, void (*call_back)(void *),
              void *arg);
int run_timer(struct timer_struct *p);
int stop_timer(struct timer_struct *p);
int uninit_timer(struct timer_struct *p);

#endif
