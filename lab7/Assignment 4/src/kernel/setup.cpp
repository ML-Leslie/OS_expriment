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

int search_and_repalce(char* page, char *FIFO_list[])
{
    int i;
    for (i = 0; i < 3; i++)
    {
        if (FIFO_list[i] == page)
        {
            printf("Hit! Page 0x%x is already in the frame!\n", page);
            return 1;
        }
    }
    FIFO_list[0] = FIFO_list[1];
    FIFO_list[1] = FIFO_list[2];
    FIFO_list[2] = page;
    printf("Miss! Page 0x%x is added to the frame\n", page);
    return 0;
}


void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    // FIFO
    printf("======Test FIFO======\n");

    char *page_1 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    //printf("Allocated page_1, starting at 0x%x.\n", page_1);
    char *page_2 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    //printf("Allocated page_2, starting at 0x%x.\n", page_2);
    char *page_3 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    //printf("Allocated page_3, starting at 0x%x.\n", page_3);
    char *page_4 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    //printf("Allocated page_4, starting at 0x%x.\n", page_4);
    char *page_5 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    //printf("Allocated page_5, starting at 0x%x.\n", page_5);
    char *page_6 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    //printf("Allocated page_6, starting at 0x%x.\n", page_6);
    char *page_7 = (char *)memoryManager.allocatePages(AddressPoolType::KERNEL, 1);
    // printf("Allocated page_7, starting at 0x%x.\n", page_7);
    printf("Allocated 7 pages in total: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", page_1, page_2, page_3, page_4, page_5, page_6, page_7);

    char * pagelist[10] = {page_1,page_1,page_2,page_3,page_2,page_4,page_5,page_3,page_6,page_7};
    
    char* FIFO_list[3] = {nullptr,nullptr,nullptr};

    for(int i =0;i < 10;i++)
    {
        search_and_repalce(pagelist[i], FIFO_list);
        printf("Now what is in the frame? :");
        for(int i = 0; i < 3; i++)
        {
            printf("page: 0x%x ", FIFO_list[i]);
        }
        printf("\n");
    }

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
