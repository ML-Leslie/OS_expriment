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
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);
    //asm_enable_interrupt();
    printf("print percentage: %%\n"
           "print char \"N\": %c\n"
           "print string \"Hello World!\": %s\n"
           "print decimal: \"-1234\": %7d\n"
           "print hexadecimal \"0x7abcdef0\": %x\n"
           "print float \"2.3336050000\": %9.3f\n"
           "print octal \"23336050\": %o\n"
           "print scientific \"2.3336050000\": %.2e\n",
           'N', "Hello World!", -1234, 0x7abcdef0, 2.3336050000, 23336050 ,2.3336050000);
    //uint a = 1 / 0;
    asm_halt();
}
