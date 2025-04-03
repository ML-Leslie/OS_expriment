org 0x7c00
[bits 16]


mov ah,2    ;功能号
mov bh,0    ;第0页
mov dh,8    ;dh中放行号
mov dl,8    ;dl中放列号
int 10h

mov ah,3    ;功能号
mov bx,0    ;第0页

int 10h

xor ax, ax

; 初始化段寄存器, 段地址全部设为0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; 初始化栈指针
mov sp, 0x7c00
mov ax, 0xb800
mov gs, ax

mov ah, 0x70 ;白底黑字
mov al, '('
mov [gs:2 * 0], ax

mov al, dh
add al, '0'
mov [gs:2 * 1], ax

mov al, ','
mov [gs:2 * 2], ax

mov al, dl
add al, '0'
mov [gs:2 * 3], ax

mov al, ')'
mov [gs:2 * 4], ax

jmp $ ; 死循环

times 510 - ($ - $$) db 0
db 0x55, 0xaa