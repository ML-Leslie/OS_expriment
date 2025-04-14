#include "asm_utils.h"
#include "interrupt.h"

// 中断管理器
InterruptManager interruptManager;

extern "C" void setup_kernel()
{
    // 中断处理部件
    interruptManager.initialize();

    // 尝试触发除0错误
    // int a = 1 / 0;

    //数组越界错误
    int a[5];
    a[10] = 1;

    // 死循环
    asm_halt();
}