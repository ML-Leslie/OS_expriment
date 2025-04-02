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
    inc ax
    cmp ax, 5
    jle load_bootloader
jmp 0x0000:0x7e00        ; 跳转到bootloader

jmp $ ; 死循环

; 定义常量
SECTORS_PER_TRACK equ 63   ; 每磁道扇区数
HEADS_PER_CYLINDER equ 18  ; 每柱面磁头数

asm_read_hard_disk:                           
; 从硬盘读取一个逻辑扇区

; 参数列表
; ax=逻辑扇区号0~15位
; cx=逻辑扇区号16~28位
; ds:bx=读取出的数据放入地址

; 返回值
; bx=bx+512

    ; 保存输入参数
    push ax
    push cx
    push dx
    
    ; 计算CHS地址 - 保存逻辑扇区号在dx:ax中
    mov dx, cx
    
    ; 计算扇区号(S) = (LBA % SPT) + 1
    xor cx, cx     ; 先清零cx
    mov cx, SECTORS_PER_TRACK
    idiv cx         ; dx:ax / cx, 商在ax, 余数在dx
    inc dx         ; 扇区号从1开始
    push dx        ; 保存扇区号
    
    ; 计算磁头号(H)和柱面号(C)
    xor dx, dx     ; 清除dx以准备下一次除法
    mov cx, HEADS_PER_CYLINDER
    idiv cx         ; ax / cx, 商在ax(柱面号C), 余数在dx(磁头号H)
    
    ; 设置磁头号和驱动器
    mov dh, dl     ; 磁头号到dh
    mov dl, 0x80   ; 驱动器号(第一个硬盘)
    
    ; 设置柱面号
    mov ch, al     ; 柱面号低8位到ch
    
    ; 设置扇区号和柱面号高2位
    pop ax         ; 取出之前保存的扇区号到al
    mov cl, al
    
    ; 设置读取的扇区数
    mov al, 1      ; 读取1个扇区
    
    ; 设置读取命令
    mov ah, 0x02   ; INT 13H AH=02H: 读扇区
    mov dl, 0x80
    
    ; 执行BIOS中断来读取硬盘
    int 0x13
    jc .error      ; 如果出错(CF=1)，跳转到错误处理

    ; 恢复寄存器并返回
    pop dx
    pop cx
    pop ax
    add bx, 512
    ret

.error:
    ; 简单的错误处理 - 重试
    pop dx
    pop cx
    pop ax
    jmp asm_read_hard_disk  ; 重试读取

times 510 - ($ - $$) db 0
db 0x55, 0xaa