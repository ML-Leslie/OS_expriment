org 0x7c00
[bits 16]

mov ah, 2        ; 功能号：设置光标位置
mov bh, 0        ; 页号
mov dh, 8       ; 行号
mov dl, 8       ; 列号
int 10h

mov al,"2"
call print_char

call add_pos
mov al,"3"
call print_char

call add_pos
mov al,"3"
call print_char

call add_pos
mov al,"3"
call print_char


call add_pos
mov al,"6"
call print_char

call add_pos
mov al,"0"
call print_char

call add_pos
mov al,"5"
call print_char

call add_pos
mov al,"0"
call print_char


; 设置光标位置 (行8，列15)
add_pos:
    mov ah, 2        ; 功能号：设置光标位置
    mov bh, 0        ; 页号
    mov dh, 8       ; 行号
    add dl, 1     ; 列号+1
    int 10h
    ret

; 显示字符
print_char:
    mov ah,9    ;功能号
    mov bl, 9   ;颜色属性
    mov bh,0    ;第0页
    mov cx,1    ;设置字符重复个数
    int 10h
    ret
jmp $ ; 死循环

times 510 - ($ - $$) db 0
db 0x55, 0xaa