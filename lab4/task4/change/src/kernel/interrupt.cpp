#include "interrupt.h"
#include "os_type.h"
#include "os_constant.h"
#include "asm_utils.h"
#include "stdio.h"

extern STDIO stdio;

int times = 0;
int flag = -1;
InterruptManager::InterruptManager()
{
    initialize();
}

void InterruptManager::initialize()
{
    // 初始化中断计数变量
    times = 0;
    
    // 初始化IDT
    IDT = (uint32 *)IDT_START_ADDRESS;
    asm_lidt(IDT_START_ADDRESS, 256 * 8 - 1);

    for (uint i = 0; i < 256; ++i)
    {
        setInterruptDescriptor(i, (uint32)asm_unhandled_interrupt, 0);
    }

    // 初始化8259A芯片
    initialize8259A();
}

void InterruptManager::setInterruptDescriptor(uint32 index, uint32 address, byte DPL)
{
    IDT[index * 2] = (CODE_SELECTOR << 16) | (address & 0xffff);
    IDT[index * 2 + 1] = (address & 0xffff0000) | (0x1 << 15) | (DPL << 13) | (0xe << 8);
}

void InterruptManager::initialize8259A()
{
    // ICW 1
    asm_out_port(0x20, 0x11);
    asm_out_port(0xa0, 0x11);
    // ICW 2
    IRQ0_8259A_MASTER = 0x20;
    IRQ0_8259A_SLAVE = 0x28;
    asm_out_port(0x21, IRQ0_8259A_MASTER);
    asm_out_port(0xa1, IRQ0_8259A_SLAVE);
    // ICW 3
    asm_out_port(0x21, 4);
    asm_out_port(0xa1, 2);
    // ICW 4
    asm_out_port(0x21, 1);
    asm_out_port(0xa1, 1);

    // OCW 1 屏蔽主片所有中断，但主片的IRQ2需要开启
    asm_out_port(0x21, 0xfb);
    // OCW 1 屏蔽从片所有中断
    asm_out_port(0xa1, 0xff);
}

void InterruptManager::enableTimeInterrupt()
{
    uint8 value;
    // 读入主片OCW
    asm_in_port(0x21, &value);
    // 开启主片时钟中断，置0开启
    value = value & 0xfe;
    asm_out_port(0x21, value);
}

void InterruptManager::disableTimeInterrupt()
{
    uint8 value;
    asm_in_port(0x21, &value);
    // 关闭时钟中断，置1关闭
    value = value | 0x01;
    asm_out_port(0x21, value);
}

void InterruptManager::setTimeInterrupt(void *handler)
{
    setInterruptDescriptor(IRQ0_8259A_MASTER, (uint32)handler, 0); //setInterruptDescriptor(index, address, DPL); address指的就是中断处理函数的地址
}

// 中断处理函数
extern "C" void c_time_interrupt_handler()
{
    // 清空屏幕
    for (int i = 0; i < 80; ++i)
    {
        stdio.print(0, i, ' ', 0x07);
    }

    // 输出"Leslie:2336050"跑马灯效果
    ++times; //全局变量
    char str[] = "interrupt happend: ";
    char str_run[] = "Leslie:2336050";
    int len = sizeof(str_run) - 1;

    char _result[len + 1];

    //一个一个显示
    // for (int i = 0; i < len; ++i)
    // {
    //     if (i != ((int)(times/3) % len))
    //     {
    //         _result[i] = ' ';
    //     }
    //     else
    //     {
    //         _result[i] = str_run[i];
    //     }
    // }
    // _result[len] = '\0'; // 结束符

    //滚动显示
    int pos = (int)(times) % (2 * len);
    for (int i = 0; i < len; ++i)
    {
        if (pos < len) {
            // 消失的尾巴
            if (i < pos) {
                _result[i] = ' ';
            } else {
                _result[i] = str_run[i - pos];
            }
        } else {
            // 出现的头
            int actualPos = pos - len;
            if (i < actualPos) {
                _result[i] = str_run[len - actualPos + i];
            } else {
                _result[i] = ' ';
            }
        }
    }
    _result[len] = '\0'; // 结束符

    // 移动光标到(0,0)输出字符
    stdio.moveCursor(0);
    for(int i = 0; str[i]; ++i ) 
    {
        stdio.print(str[i]);
    }
    
    // 跑马灯打印字符串
    for(int i = 0; i < len; ++i ) 
    {
        stdio.print(_result[i]);
    }
}