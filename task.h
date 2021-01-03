#ifndef __TASK_H__
#define __TASK_H__

#include "queue.h"
#include "rbtree_node.h"

#define TASK_RUN 1
#define TASK_END 2

/*
//协程上下文
%rax 作为函数返回值使用。
%rsp 栈指针寄存器，指向栈顶
%rdi，%rsi，%rdx，%rcx
用作函数参数，依次对应第1参数，第2参数。。。
%rbx，%rbp 用作数据存储
%rip 下一条指令
一个函数在执行的时候，某一时刻的状态，就主要靠这些寄存器来描述。
同理，如果想恢复一个函数此前的状态，恢复这些寄存器即可。
这里使用e开头，应该为了后续兼容32位，按说只支持64位的话，用r前缀更合适
*/
struct task_ctx {
    unsigned long eax;
    unsigned long ebx;
    unsigned long ecx;
    unsigned long edx;
    unsigned long edi;
    unsigned long esi;
    unsigned long esp;
    unsigned long ebp;
    unsigned long eip;
};

// 协程
struct task {
    int state;  //运行状态
    int pid;
    void *start_addr;
    rbtree_node pid_node;
    queue_node ready_node;
    queue_node end_node;
    struct task_ctx ctx;            //上下文
    void (*start_routine)(void *);  //回调函数
    void *arg;                      //回调参数
};

int task_init();
int task_switch(struct task *src, struct task *obj);
int task_create(void (*start_routine)(void *), void *arg);
void task_exit();
int task_kill(int pid);
int task_uninit();

#endif
