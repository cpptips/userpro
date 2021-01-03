#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

int init_timer(struct timer_struct *p, int cycle_time) {
    if (p == NULL) {
        return -1;
    }

    p->epfd = epoll_create(1);  //这里里面维护了一个epfd
    p->cycle_time = cycle_time;
    p->isstop = 0;                //定时器是否停止
    queue_init(&p->timer_queue);  //初始化队列

    return 0;
}

//添加到定时器当中
int add_timer(struct timer_struct *p, int cycle, void (*call_back)(void *),
              void *arg) {
    if (p == NULL) {
        return -1;
    }

    struct timer_data *data = calloc(1, sizeof(struct timer_data));
    if (data == NULL) {
        return -2;
    }

    data->arg = arg;
    data->call_back = call_back;
    data->cycle = cycle;
    data->time = cycle;

    queue_push(&p->timer_queue, &data->qnode);

    return 0;
}

// 运行定时器
int run_timer(struct timer_struct *p) {
    if (p == NULL) {
        return -1;
    }

    queue_node *current_node = NULL;
    struct timer_data *current = NULL;

    struct epoll_event evs[1];
    while (!p->isstop) {
        epoll_wait(p->epfd, evs, 1, p->cycle_time);  // cycle_time是超时时间

        current_node = p->timer_queue.head;
        current = GET_STRUCT_START_ADDR(struct timer_data, qnode, current_node);
        if (queue_isempty(&p->timer_queue)) {
            continue;
        }

        while (current != NULL) {
            current->time -= p->cycle_time;

            if (current->time <= 0) {
                // 时间到了，那么执行任务
                current->call_back(current->arg);
                current->time = current->cycle;
            }
            //接着这个桶中其他节点有没有到时了
            current_node = current->qnode.next;
            if (current_node == NULL) {
                break;
            }
            current =
                GET_STRUCT_START_ADDR(struct timer_data, qnode, current_node);
        }
    }

    return 0;
}

int stop_timer(struct timer_struct *p) {
    if (p == NULL) {
        return -1;
    }

    p->isstop = 1;
    return 0;
}

int uninit_timer(struct timer_struct *p) {
    if (p == NULL) {
        return -1;
    }

    close(p->epfd);
    p->isstop = 1;

    while (!queue_isempty(&p->timer_queue)) {
        queue_pop(&p->timer_queue, NULL);
    }

    queue_uninit(&p->timer_queue);

    return 0;
}
