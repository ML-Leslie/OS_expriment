;org 0x7c00
[bits 16]
xor ax, ax ; eax = 0
; 初始化段寄存器, 段地址全部设为0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00
mov ax, 1                ; 逻辑扇区号第0~15位
mov cx, 0                ; 逻辑扇区号第16~31位
mov bx, 0x7e00           ; bootloader的加载地址
load_bootloader:
    call asm_read_hard_disk  ; 读取硬盘
    add bx, 512         ; 增加缓冲区指针
    inc ax
    cmp ax, 5
    jle load_bootloader
jmp 0x0000:0x7e00        ; 跳转到bootloader

jmp $ ; 死循环

asm_read_hard_disk:                           
; 从硬盘读取一个逻辑扇区，使用CHS模式和BIOS中断13h

; 参数列表
; ax=逻辑扇区号0~15位
; cx=逻辑扇区号16~28位 (在当前实现中假设为0)
; ds:bx=读取出的数据放入地址

; 返回值
; bx=bx+512

    ;保存寄存器
    push ax
    push cx
    push dx
    
    ; 计算扇区号 (S) = (LBA % 63) + 1
    xor dx, dx
    mov cx, 63          ; 每磁道扇区数
    idiv cx              ; ax / cx，商在ax，余数在dx
    inc dx              ; 扇区号从1开始
    mov cl, dl          ; CL的低6位 = 扇区号(1-63)

    ; 计算磁头号(H) = (LBA / 63) % 18 和 柱面号(C) = (LBA / 63) / 18
    xor dx, dx
    mov di, 18          ; 每柱面磁头数
    idiv di             ; ax / cx，商在ax (柱面号)，余数在dx (磁头号)

    mov ch, al          ; CH = 柱面号的低8位
    mov dh, dl          ; DH = 磁头号
    mov dl, 80h         ; DL = 驱动器号 
    
    mov al,1        ;AL = 读取1个扇区
    mov ah, 2h       ; AH = 02h (读取扇区功能),
    int 13h             ; 调用BIOS中断


    ;恢复寄存器
    pop dx
    pop cx
    pop ax
    ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa