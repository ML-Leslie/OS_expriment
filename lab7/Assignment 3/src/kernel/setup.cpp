// filepath: /home/leslie/OS_expriment/lab7/Assignment 3/src/kernel/setup.cpp
#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    // 在同个地方两次分配
    printf("=======Test 1=======\n");
    char *pages_0 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 128);
    printf("Allocated 128 pages for pages_0, starting at 0x%x.\n", pages_0);

    memoryManager.releasePages(AddressPoolType::KERNEL, int(pages_0), 128);
    printf("Released 128 pages for pages_0.\n");

    char *pages_0_ = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 128);
    printf("Allocated 128 pages again, starting at 0x%x.\n", pages_0_);
    
    memoryManager.releasePages(AddressPoolType::KERNEL, int(pages_0_), 128); // 释放内存进入下一个测试
    printf("Released 128 pages for pages_0_.\n");

    // 释放中间内存，再分别申请小于这个中间空位和大于这个中间空位的内存
    printf("=======Test 2=======\n");
    char *pages_1 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 64);
    printf("Allocated 64 pages for pages_1, starting at 0x%x.\n", pages_1);

    char *pages_2 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 16);
    printf("Allocated 16 pages for pages_2, starting at 0x%x.\n", pages_2);

    char *pages_3 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 8);
    printf("Allocated 8 pages for pages_3, starting at 0x%x.\n", pages_3);

    memoryManager.releasePages(AddressPoolType::KERNEL, int(pages_2), 16); // 释放中间内存
    printf("Released 16 pages for pages_2.\n");

    char *pages_4 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 8); // 申请小于中间空位的内存
    printf("Allocated 8 pages for pages_4, starting at 0x%x.\n", pages_4);
    memoryManager.releasePages(AddressPoolType::KERNEL, int(pages_4), 8);
    printf("Released 8 pages for pages_4.\n");

    char *pages_5 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 64); // 申请大于中间空位的内存
    printf("Allocated 64 pages for pages_5, starting at 0x%x.\n", pages_5);

    memoryManager.releasePages(AddressPoolType::KERNEL, int(pages_1), 64); // 释放内存进入下一个测试
    memoryManager.releasePages(AddressPoolType::KERNEL, int(pages_3), 8);
    memoryManager.releasePages(AddressPoolType::KERNEL, int(pages_5), 64);
    printf("Released all remaining pages.\n");
    
    // 尝试申请小的内存和大的内存（边缘情况）
    printf("=======Test 3=======\n");
    char *pages_6 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    printf("Allocated 1 page for pages_6, starting at 0x%x.\n", pages_6);
    char *pages_7 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 666);
    printf("Allocated 666 pages for pages_7, starting at 0x%x.\n", pages_7);

    memoryManager.releasePages(AddressPoolType::KERNEL, int(pages_6), 64); // 释放内存
    memoryManager.releasePages(AddressPoolType::KERNEL, int(pages_7), 666); 
    printf("Released all remaining pages.\n");
    
    asm_halt();
}

extern "C" void setup_kernel()
{
    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 内存管理器
    memoryManager.openPageMechanism();
    memoryManager.initialize();

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