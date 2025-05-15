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
    // 清屏
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    printf("Test starts...\n\n");
    
    // 申请内核物理页
    printf("Testing kernel physical memory allocation :\n");
    int kernelPages = 3; // 申请3页用户内存
    char *kernelAddr1 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, kernelPages);
    char *kernelAddr2 = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::KERNEL, kernelPages);

    
    if (kernelAddr1 != (char*)-1) {
        printf("Successfully applied for %d pages, start: 0x%x\n",kernelPages ,kernelAddr1);
        // 释放内存
        printf("\nReleasing kernel physical pages...\n");
        memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, (int)kernelAddr1, kernelPages);
        printf("Release successfully!\n");
    } else {
        printf("Kernel physical page request failed!\n");
    }

    
    if (kernelAddr2 != (char*)-1) {
        printf("Successfully applied for %d pages, start: 0x%x\n",kernelPages ,kernelAddr2);
        // 释放内存
        printf("\nReleasing kernel physical pages...\n");
        memoryManager.releasePhysicalPages(AddressPoolType::KERNEL, (int)kernelAddr2, kernelPages);
        printf("Release successfully!\n");
    } else {
        printf("Kernel physical page request failed!\n");
    }
    
    // 申请用户物理页
    printf("\nTesting user physical memory allocation ::\n");
    int userPages = 3; // 申请3页用户内存
    char *userAddr = (char *)memoryManager.allocatePhysicalPages(AddressPoolType::USER, userPages);
    
    if (userAddr) {
        printf("Successfully applied for %d pages, start: 0x%x\n", userPages, userAddr);
        // 释放内存
        printf("\nReleasing user physical pages...\n");
        memoryManager.releasePhysicalPages(AddressPoolType::USER, (int)userAddr, userPages);
        printf("Release successfully!\n");
    } else {
        printf("User physical page request failed!\n");
    }
    // 线程不可返回
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
