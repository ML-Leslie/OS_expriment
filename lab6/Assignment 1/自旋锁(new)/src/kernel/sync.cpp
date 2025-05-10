#include "sync.h"
#include "asm_utils.h"
#include "stdio.h"
#include "os_modules.h"

SpinLock::SpinLock()
{
    initialize();
}

void SpinLock::initialize()
{
    bolt = 0;
}

void SpinLock::lock()
{
    
    while (asm_atomic_bts(&bolt))
    {
        // 自旋等待锁释放
    }
}

void SpinLock::unlock()
{
    bolt = 0;
}