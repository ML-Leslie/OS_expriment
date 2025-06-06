#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

void third_thread(void *arg) {
    printf("pid %d name \"%s\": Hello World! arrivalTime: %d \n", programManager.running->pid, programManager.running->name, programManager.running->arrivalTime);
}
void second_thread(void *arg) {
    printf("pid %d name \"%s\": Hello World! arrivalTime: %d \n", programManager.running->pid, programManager.running->name, programManager.running->arrivalTime);
}
void first_thread(void *arg)
{
    // 第1个线程不可以返回
    printf("pid %d name \"%s\": Hello World! arrivalTime: %d \n", programManager.running->pid, programManager.running->name, programManager.running->arrivalTime);

    if (!programManager.running->pid)
    {
        for(int i = 0; i < 100000000; i++);
        programManager.executeThread(second_thread, nullptr, "second thread", 2);
        for(int i = 0; i < 100000000; i++);
        programManager.executeThread(third_thread, nullptr, "third thread", 3);

        programManager.schedulePriority();
    }
    asm_halt();
}

extern "C" void setup_kernel()
{

    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 启用中断，让times开始计数
    interruptManager.enableInterrupt();
    
    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
