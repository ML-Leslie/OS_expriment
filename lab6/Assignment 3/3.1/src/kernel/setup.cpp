#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"


STDIO stdio;
InterruptManager interruptManager;
ProgramManager programManager;


#define PHILOSOPHER_COUNT 5  // 哲学家数量
Semaphore chopsticks[PHILOSOPHER_COUNT];  // 每根筷子一个信号量

void Philosopher(void *arg)
{
    int id = *(int *)arg;  // 哲学家的编号
    // 哲学家就餐
    do
    {
        // 取左边的筷子
        printf("Philosopher %d is trying to pick up the chopsticks on the left\n", id);
        chopsticks[id].P();
        printf("Philosopher %d has picked up the chopsticks on the left\n", id);
        int delay = 0xfffffff;
        while (delay) --delay;
        // 取右边的筷子
        printf("Philosopher %d is trying to pick up the chopsticks on the right\n", id);
        chopsticks[(id + 1) % PHILOSOPHER_COUNT].P();
        printf("Philosopher %d has picked up the chopsticks on the right\n", id);
        // 吃饭
        printf("Philosopher %d is eating\n", id);
        // 放下左边的筷子
        chopsticks[id].V();
        // 放下右边的筷子
        chopsticks[(id + 1) % PHILOSOPHER_COUNT].V();
        // 思考
        printf("Philosopher %d is thinking\n", id);
    }
    while(1);
}

// 运行哲学家就餐问题
void first_thread(void *arg)
{
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    // 初始化信号量
    for (int i = 0; i < PHILOSOPHER_COUNT; ++i)
    {
        chopsticks[i].initialize(1);  // 每根筷子初始状态为可用
    }
   
    int philosopher_ids[PHILOSOPHER_COUNT] = {0, 1, 2, 3, 4};
    programManager.executeThread(Philosopher, &philosopher_ids[0], "Philosopher 0", 1);
    programManager.executeThread(Philosopher, &philosopher_ids[1], "Philosopher 1", 1);
    programManager.executeThread(Philosopher, &philosopher_ids[2], "Philosopher 2", 1);
    programManager.executeThread(Philosopher, &philosopher_ids[3], "Philosopher 3", 1);
    programManager.executeThread(Philosopher, &philosopher_ids[4], "Philosopher 4", 1);

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

