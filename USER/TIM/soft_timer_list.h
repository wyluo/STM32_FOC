#ifndef SOFT_TIMER_LIST_H
#define SOFT_TIMER_LIST_H

#include "stm32f10x.h"
#include <stdbool.h>

/**
 * 硬件中断tick
 */
#define TIMER_HARD_TICK     100U//ms,硬件tick取决于硬件定时中断时间
#define TIMER_200MS_TICK    (200U / TIMER_HARD_TICK)//TIMER_HARD_TICK * (2) = 100mS
#define TIMER_SEC_TICK      (1000U / TIMER_HARD_TICK)//TIMER_HARD_TICK * (20) = 1S

/**
 * 定时器模式选择
 */
typedef enum
{
    ONCE_MODE,							/* 单次定时模式，即超时后自动关闭定时器 */
	CONTINUE_MODE,						/* 持续定时模式，只要开启除非手动关闭否则永不停歇 */
	DEFINE_NUM_MODE,					/* 定义次数的模式，运行指定的次数后关闭定时器 */
}TimerTimingModeType;

/**
 * 定时超时后运行的回调函数可以选择在中断直接运行或者挂起任务轮询执行
 * 只要在定时需求准确的时候才建议选择中断模式执行，类似无磁传感器脉冲测量
 * 像一些超时判断类的应用以轮询的方式进行执行
 * 中断执行模式越多，其他定时器越不准,毕竟中断允许占时间，查询其他定时器时
 * 会有延时
 */
typedef enum
{
    RUN_IN_LOOP_MODE,                      /* 轮询执行模式 */
    RUN_IN_INTERRUPT_MODE,                 /* 中断实时执行模式 */
}TimerRunModeType;

/**
 * 软件定时器基本类型
 */
typedef struct SoftTimer
{
    unsigned long counter;                /* 计数              */
    unsigned long duration;               /* 定时时长          */
    unsigned long run_num;                /* 自定义的定时次数  */
    bool start_flag;                      /* 启动标志          */
    bool loop_flag;                       /* 轮询标志          */
    TimerRunModeType run_mode;
    TimerTimingModeType timing_mode;
    void (*callback_function)(void);      /* 回调函数          */
    struct SoftTimer *next;
}SoftTimer;

/*
 * 初始化软件定时器的硬件tick
 */
extern void soft_timer_tick_init(void);

/*
 * 创建一个只运行一次的软件定时器并立刻开始计时
 * 参数表：p: 定时器结构体指针，由用户创建
 *        mode: 选择运行模式，可选定时器到了之后是直接在tick中断内执行还是置起标志在while循环内轮询执行
 *        duration: 要计时的时长，单位为硬件中断的tick
 *        timeout_handler: 定时到了之后要执行的函数指针
 * return：无
 */
extern void creat_single_soft_timer(SoftTimer *p, TimerRunModeType mode, unsigned long duration, void(*timeout_handler)(void));

/*
 * 创建永远运行的软件定时器并立刻开始计时
 * 参数表：p: 定时器结构体指针，由用户创建
 *        mode: 选择运行模式，可选定时器到了之后是直接在tick中断内执行还是置起标志在while循环内轮询执行
 *        duration: 要计时的时长，单位为硬件中断的tick
 *        timeout_handler: 定时到了之后要执行的函数指针
 * return：无
 */
extern void creat_continue_soft_timer(SoftTimer *p, TimerRunModeType mode, unsigned long duration, void(*timeout_handler)(void));

/*
 * 创建指定次数运行的软件定时器并立刻开始计时
 * 参数表：p: 定时器结构体指针，由用户创建
 *        mode: 选择运行模式，可选定时器到了之后是直接在tick中断内执行还是置起标志在while循环内轮询执行
 *        run_num：要定时的次数，比如1就是定时1次，5就是定时5次以后自动关闭定时器
 *        duration: 要计时的时长，单位为硬件中断的tick
 *        timeout_handler: 定时到了之后要执行的函数指针
 * return：无
 */
extern void creat_limit_num_soft_timer(SoftTimer *p, TimerRunModeType mode, \
										unsigned long run_num, unsigned long duration, void(*timeout_handler)(void));

/*
 * 重启指定的单次软件定时器
 * 参数表：p: 定时器结构体指针，由用户创建
 *        mode: 选择运行模式，可选定时器到了之后是直接在tick中断内执行还是置起标志在while循环内轮询执行
 *        duration: 要计时的时长，单位为硬件中断的tick
 *        timeout_handler: 定时到了之后要执行的函数指针
 * return：无
 */
extern void restart_single_soft_timer(SoftTimer *p, TimerRunModeType mode, unsigned long duration, void(*timeout_handler)(void));

/**
 * 删除一个软件定时器
 */
extern void stop_timer(SoftTimer *p);

/**
 * 系统main循环进程，用于执行轮询模式的回调函数
 */
extern void soft_timer_main_loop(void);

/**
 * 此函数为tick中断服务函数，需要挂载在外部硬件定时器上
 * 因此软件定时器的定时精度由此函数挂载的硬件定时时间决定，
 * 比如此函数挂载在定时50ms的外部定时器上，那么定时dutation
 * 为20时定时时间就是20*50ms=1S
 */
extern void system_tick_IrqHandler(void);


#endif
