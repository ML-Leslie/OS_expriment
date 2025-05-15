#include "bitmap.h"
#include "stdlib.h"
#include "stdio.h"

BitMap::BitMap()
{
}

void BitMap::initialize(char *bitmap, const int length)
{
    this->bitmap = bitmap;
    this->length = length;

    int bytes = ceil(length, 8);

    for (int i = 0; i < bytes; ++i)
    {
        bitmap[i] = 0;
    }
}

bool BitMap::get(const int index) const
{
    int pos = index / 8;
    int offset = index % 8;

    return (bitmap[pos] & (1 << offset));
}

void BitMap::set(const int index, const bool status)
{
    int pos = index / 8;
    int offset = index % 8;

    // 清0
    bitmap[pos] = bitmap[pos] & (~(1 << offset));

    // 置1
    if (status)
    {
        bitmap[pos] = bitmap[pos] | (1 << offset);
    }
}

int BitMap::allocate(const int count)
{
    if (count <= 0)
        return -1;

    int index, empty, start;
    
    int bestStart = -1;      // 最佳匹配的起始位置
    int bestSize = length+1; // 最佳匹配的大小（初始值设置为比位图长度还大）
    
    index = 0;
    while (index < length)
    {
        // 越过已经分配的资源
        while (index < length && get(index))
            ++index;
            
        // 不存在连续的count个资源
        if (index == length)
            return -1;
            
        // 找到未分配的资源，计算大小
        empty = 0;
        start = index;
        while (index < length && !get(index))
        {
            ++empty;
            ++index;
        }
        
        // 更新最佳匹配
        if (empty >= count && empty < bestSize)
        {
            bestStart = start;
            bestSize = empty;
        }
    }
    
   
    if (bestStart != -1)
    {
        // 分配资源
        for (int i = 0; i < count; ++i)
        {
            set(bestStart + i, true);
        }
        return bestStart;
    }
    
    // 没有找到足够大的空间
    return -1;
}

void BitMap::release(const int index, const int count)
{
    for (int i = 0; i < count; ++i)
    {
        set(index + i, false);
    }
}

char *BitMap::getBitmap()
{
    return (char *)bitmap;
}

int BitMap::size() const
{
    return length;
}