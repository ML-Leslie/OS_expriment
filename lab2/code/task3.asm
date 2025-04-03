; If you meet compile error, try 'sudo apt install gcc-multilib g++-multilib' first

%include "head.include"
; you code here



your_if:
; put your implementation here
cmp dword [a1],12
jge less_then_24
mov eax,[a1]
mov ecx,2
idiv ecx
mov [if_flag],eax 
inc dword [if_flag]
jmp end_if

less_then_24:
    cmp dword [a1],24
    jge else_ 
    mov ecx,24
    sub ecx,[a1]    
    imul ecx,[a1]
    mov [if_flag], ecx
    jmp end_if

else_:
    mov eax,[a1]
    shl eax,4
    mov [if_flag], eax

end_if:

your_while:

    mov eax,[a2]
    cmp eax, 12
    jl your_while_end   

    call my_random       


    mov ecx, [a2]
    sub ecx, 12          
    mov edx, [while_flag]  ;[while_flag]是在获取这个指针变量存储的地址值，也就是它指向的内存位置，而不是指针变量本身的地址
    add edx, ecx;计算出地址放在edx中         

    mov byte [edx], al;一个字节  

    dec dword [a2]

    jmp your_while

your_while_end:


%include "end.include"


your_function:

    mov edx, [your_string]    
    mov ecx, 0              
    
loop_start:
    
    movzx eax, byte [edx + ecx]  
    cmp eax, 0 ;检查是否为 '\0'
    jz loop_end
    
    pushad
    
    push eax
    
    call print_a_char
    
    pop eax
    
    popad
    
    inc ecx
    
    jmp loop_start
    
loop_end:
    ret