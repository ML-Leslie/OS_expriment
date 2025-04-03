org 0x7c00
[bits 16]


mov ah, 2 
mov bh, 0 
mov dh, 0 
mov dl, 0 
int 10h


PRINT:

    mov ah,0;功能号
    int 16h

    cmp al,0x1b
    je END_

    mov ah, 2 ;功能号2设置光标位置        
    mov bh, 0        
    int 10h

    ; 显示字符
    mov ah,9    ;功能号
    mov bl,7    ;颜色属性
    mov bh,0    ;第0页
    mov cx,1    ;设置字符重复个数
    int 10h

    add dl,1
    cmp dl,80
    jl PRINT ; 如果dl < 80, 则继续打印
    add dh,1
    mov dl,0
    cmp dh,25
    jl PRINT ; 如果dh < 25, 则继续打印
    mov dh,0
    mov dl,0
    jmp PRINT


END_:
    jmp $

times 510 - ($ - $$) db 0
db 0x55, 0xaa