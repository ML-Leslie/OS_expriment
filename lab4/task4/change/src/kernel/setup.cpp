#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;


extern "C" void setup_kernel()
{
    // 中断处理部件
    interruptManager.initialize();
    // 屏幕IO处理部件
    stdio.initialize();
    interruptManager.enableTimeInterrupt(); // 开启时钟中断
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler); //设置时钟中断处理函数
    asm_enable_interrupt();   //为了CPU响应可屏蔽中断，需要这个函数
    asm_halt();
}

