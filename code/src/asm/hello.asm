  ;; hello.asm
  section .data
  msg db "Hello world!", 0xA ;string to printf
  len equ $ - msg               ;string len
  section .text                 ;code data section
  global _start
_start: 
  mov edx, len
  mov ecx, msg
  mov ebx, 1
  mov eax, 4                    ;sys_write
  int 0x80
  ;; exit
  mov ebx, 0
  mov eax, 1
  int 0x80                      ;system call
