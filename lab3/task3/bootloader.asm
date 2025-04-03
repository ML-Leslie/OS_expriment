%include "boot.inc"
;org 0x7e00
[bits 16]
mov ax, 0xb800
mov gs, ax

;空描述符
mov dword [GDT_START_ADDRESS+0x00],0x00
mov dword [GDT_START_ADDRESS+0x04],0x00  

;创建描述符，这是一个数据段，对应0~4GB的线性地址空间
mov dword [GDT_START_ADDRESS+0x08],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x0c],0x00cf9200    ; 粒度为4KB，存储器段描述符 

;建立保护模式下的堆栈段描述符      
mov dword [GDT_START_ADDRESS+0x10],0x00000000    ; 基地址为0x00000000，界限0x0 
mov dword [GDT_START_ADDRESS+0x14],0x00409600    ; 粒度为1个字节

;建立保护模式下的显存描述符   
mov dword [GDT_START_ADDRESS+0x18],0x80007fff    ; 基地址为0x000B8000，界限0x07FFF 
mov dword [GDT_START_ADDRESS+0x1c],0x0040920b    ; 粒度为字节

;创建保护模式下平坦模式代码段描述符
mov dword [GDT_START_ADDRESS+0x20],0x0000ffff    ; 基地址为0，段界限为0xFFFFF
mov dword [GDT_START_ADDRESS+0x24],0x00cf9800    ; 粒度为4kb，代码段描述符 

;初始化描述符表寄存器GDTR
mov word [pgdt], 39      ;描述符表的界限   
lgdt [pgdt]
      
in al,0x92                         ;南桥芯片内的端口 
or al,0000_0010B
out 0x92,al                        ;打开A20

cli                                ;中断机制尚未工作
mov eax,cr0
or eax,1
mov cr0,eax                        ;设置PE位
      
;以下进入保护模式
jmp dword CODE_SELECTOR:protect_mode_begin

;16位的描述符选择子：32位偏移
;清流水线并串行化处理器
[bits 32]           
protect_mode_begin:                              

mov eax, DATA_SELECTOR                     ;加载数据段(0..4GB)选择子
mov ds, eax
mov es, eax
mov eax, STACK_SELECTOR
mov ss, eax
mov eax, VIDEO_SELECTOR
mov gs, eax

;这里开始
mov esi, 1       ; 表示行号在加
mov edi, 1       ; 表示列号在加
mov byte [seed], 42  ; 初始种子值

mov dl, 0        ; 列号
mov dh, 2        ; 行号
mov ah, 0x07     ; 颜色属性

PLAY:
    ; 计算屏幕偏移
    movzx ebx, dh
    imul ebx, 80  ; 行 * 80
    movzx ecx, dl
    add ebx, ecx  ; + 列
    shl ebx, 1    ; * 2 (每个字符占用2字节)

    ; 生成随机颜色和字符
    mov cl, [seed]
    
    ; 更新随机种子
    shl cl, 3    
    xor cl, [seed]   
    add cl, dh   
    add cl, dl   
    mov [seed], cl
    
    ; 随机字符 (A-Z)
    and cl, 0x1F    ; 保留低5位
    add cl, 'A'      ; 转换为字母
    cmp cl, 'Z'+1    ; 确保在A-Z范围内
    jl valid_char
    sub cl, 26       ; 如果超出范围，回到A-Z范围内
valid_char:
    mov al, cl   
    
    ; 随机颜色
    mov cl, [seed]
    mov ah, cl      
    and ah, 0x0F     ; 保留低4位
    
    rol cl, 4 ;旋转随机数
    and cl, 0x07 ;限制为0-7
    shl cl, 4 
    or ah, cl ;合并颜色

    ; 写入显存
    mov word [gs:ebx], ax

    ; 更新列位置
    cmp esi, 1    ; 如果列号在加
    jne DEC_COL
    add dl, 1
    cmp dl, 80
    jl END_R_C
    mov esi, 0
    mov dl, 78
    jmp END_R_C
DEC_COL:
    sub dl, 1
    cmp dl, 0
    jge END_R_C
    mov esi, 1
    mov dl, 1
END_R_C:

    ; 更新行位置
    cmp edi, 1    ; 如果行号在加
    jne DEC_ROW
    add dh, 1
    cmp dh, 25
    jl END_R_R
    mov edi, 0
    mov dh, 23
    jmp END_R_R
DEC_ROW:
    sub dh, 1
    cmp dh, 0
    jge END_R_R
    mov edi, 1
    mov dh, 1
END_R_R:

    ; 延迟
    mov ecx, 0x1000000  ; 调整这个值以控制速度
DELAY:
    loop DELAY

    jmp PLAY

seed: db 42    ; 存储随机种子的变量
;这里结束

jmp $ ; 死循环

pgdt dw 0
     dd GDT_START_ADDRESS

