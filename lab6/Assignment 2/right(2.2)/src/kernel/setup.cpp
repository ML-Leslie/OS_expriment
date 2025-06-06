#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"


STDIO stdio;
InterruptManager interruptManager;
ProgramManager programManager;

#define QUOTES_COUNT 4
char* quotes[QUOTES_COUNT] = {
    "I am Jacky Cheung, I will show you one of my songs",
    "I am Leslie Cheung, I will show you one of my songs",
    "I am Faye Wong, I will show you one of my songs",
    "I am George Lam, I will show you one of my songs"
};

// 互斥锁和计数器
Semaphore rwMutex;       // 控制读写互斥
Semaphore readCountMutex; // 保护readCount
int readCount = 0;       // 当前读者数量


// 读取第1条记录
void readFirstQuote(void *arg) {
    int delay = 0x1ffffff;
    while (delay) --delay;
    
    // 等待readCountMutex
    readCountMutex.P();
    readCount++;
    if (readCount == 1) {
        // 第一个读者需要获取rwMutex
        rwMutex.P();
    }
    readCountMutex.V();
    
    // 读取操作
    printf("Reader: Quote 1 - \"%s\"\n", quotes[0]);
    
    // 等待readCountMutex
    readCountMutex.P();
    readCount--;
    if (readCount == 0) {
        // 最后一个读者释放rwMutex
        rwMutex.V();
    }
    readCountMutex.V();
}

// 读取第2条记录
void readSecondQuote(void *arg) {
    int delay = 0x1ffffff;
    while (delay) --delay;
    
    // 等待readCountMutex
    readCountMutex.P();
    readCount++;
    if (readCount == 1) {
        // 第一个读者需要获取rwMutex
        rwMutex.P();
    }
    readCountMutex.V();
    
    // 读取操作
    printf("Reader: Quote 2 - \"%s\"\n", quotes[1]);
    
    // 等待readCountMutex
    readCountMutex.P();
    readCount--;
    if (readCount == 0) {
        // 最后一个读者释放rwMutex
        rwMutex.V();
    }
    readCountMutex.V();
}

// 读者函数 - 读取第3条记录
void readThirdQuote(void *arg) {
    int delay = 0x1ffffff;
    while (delay) --delay;
    
    // 等待readCountMutex
    readCountMutex.P();
    readCount++;
    if (readCount == 1) {
        // 第一个读者需要获取rwMutex
        rwMutex.P();
    }
    readCountMutex.V();
    
    // 读取操作
    printf("Reader: Quote 3 - \"%s\"\n", quotes[2]);
    
    // 等待readCountMutex
    readCountMutex.P();
    readCount--;
    if (readCount == 0) {
        // 最后一个读者释放rwMutex
        rwMutex.V();
    }
    readCountMutex.V();
}

// 读者函数 - 读取第4条记录
void readFourthQuote(void *arg) {
    int delay = 0x1ffffff;
    while (delay) --delay;
    

    // 等待readCountMutex
    readCountMutex.P();
    readCount++;
    if (readCount == 1) {
        // 第一个读者需要获取rwMutex
        rwMutex.P();
    }
    readCountMutex.V();
    
    // 读取操作
    printf("Reader: Quote 4 - \"%s\"\n", quotes[3]);
    
    // 等待readCountMutex
    readCountMutex.P();
    readCount--;
    if (readCount == 0) {
        // 最后一个读者释放rwMutex
        rwMutex.V();
    }
    readCountMutex.V();
    
}

// 写者函数 - 修改第2条记录
void writeSecondQuote(void *arg) {
    int delay = 0xfffffff;

    rwMutex.P();
    
    // 写入操作（耗时较长）
    printf("Writer: Updating Quote 2...\n");
    while (delay) --delay;
    
    quotes[1] = "I will sing 'The wind blows' for you.";
    printf("Writer: Quote 2 updated to \"%s\"\n", quotes[1]);
    
    rwMutex.V();
    
}

// 写者函数 - 修改第4条记录
void writeFourthQuote(void *arg) {
    int delay = 0xfffffff;
    
    rwMutex.P();
    
    // 写入操作（耗时较长）
    printf("Writer: Updating Quote 4...\n");
    while (delay) --delay;
    
    quotes[3] = "I will sing 'In The Middle of The Water' for you.";
    printf("Writer: Quote 4 updated to \"%s\"\n", quotes[3]);
    
    rwMutex.V();
    
}
// 正确版本演示
void reader_writer_correct(void *arg) {
    
    // 初始化信号量
    rwMutex.initialize(1);
    readCountMutex.initialize(1);
    readCount = 0;
    
    // 创建线程读第1-4条记录
    programManager.executeThread(readFirstQuote, nullptr, "second thread", 1);
    programManager.executeThread(readSecondQuote, nullptr, "third thread", 1);
    programManager.executeThread(readThirdQuote, nullptr, "fourth thread", 1);
    programManager.executeThread(readFourthQuote, nullptr, "fifth thread", 1);
    
    // 创建线程，修改第2条和第4条记录为较长内容
    programManager.executeThread(writeSecondQuote, nullptr, "sixth thread", 1);
    programManager.executeThread(writeFourthQuote, nullptr, "seventh thread", 1);
    
    // 创建线程读第2条和第4条记录
    programManager.executeThread(readSecondQuote, nullptr, "eighth thread", 1);
    programManager.executeThread(readFourthQuote, nullptr, "ninth thread", 1);
}


void first_thread(void *arg)
{

    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);
   
    programManager.executeThread(reader_writer_correct, nullptr, "demo_error", 1);
    
    asm_halt();
}

extern "C" void setup_kernel()
{

    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    stdio.initialize();
    programManager.initialize();

    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("Cannot execute thread\n");
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

