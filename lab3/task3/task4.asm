org 0x7c00
[bits 16]

; 初始化光标位置(2,0)
mov ah, 2        
mov bh, 0        
mov dh, 2  
mov dl, 0     
int 10h

mov si,1;表示行号在加
mov di,1;表示列号在加

mov byte [seed], 42  ; 初始种子值

mov bl,66

PLAY:
    mov ah,0
    int 16h

    cmp al,0x1b
    je END_

    mov ah, 2 ;功能号2设置光标位置        
    mov bh, 0        
    int 10h

    ;生成随机颜色
    mov cl, [seed] 
    mov bl, cl  
    shl bl, 3       
    xor bl, cl       
    add bl, dh      
    add bl, dl   
    and bl, 0x7F    
    mov [seed], bl  
    
    ;显示字符
    mov ah,9    
    mov bh,0  
    mov cx,1   
    int 10h

    ;更新列
    cmp si,1;如果行号在加
    jne DEC_COL
    add dl,1
    cmp dl,80
    jl END_R_C
    mov si,0
    mov dl,78
    jmp END_R_C
    DEC_COL:
    sub dl,1
    cmp dl,0
    jge END_R_C
    mov si,1
    mov dl,1
    END_R_C:
     
    ;更新行
    cmp di,1;如果行号在加
    jne DEC_ROW
    add dh,1
    cmp dh,25
    jl END_R_R
    mov di,0
    mov dh,23
    jmp END_R_R
    DEC_ROW:
    sub dh,1
    cmp dh,0
    jge END_R_R
    mov di,1
    mov dh,1
    END_R_R:

    jmp PLAY

END_:
    jmp $

seed: db 42    ; 存储随机种子的变量

times 510 - ($ - $$) db 0
db 0x55, 0xaa