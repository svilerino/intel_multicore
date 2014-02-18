%include "../macros/asm_screen_utils.mac"

global go64

;; GDT

extern GDT_DESC

;; IDT
extern IDT_DESC

;; STACK
extern core_stack_ptrs

;; Sorting
extern sort_ap

%macro get_lapic_id 0  ; para distinguir los procesadores entre si
    ;xor rax, rax ; por si las moscas
    ;mov eax, 0xb ; Manual de intel capitulo 8.4.5 Read 32-bit APIC ID from CPUID leaf 0BH
    ;cpuid
    ;mov eax, edx ; esta version describe topologia, capaz varia con una maquina real
    xor rax, rax
    mov eax, 0xfee00020
    mov eax, [eax]
    shr eax, 24
    and eax, 0xFF
%endmacro

%define breakpoint xchg bx, bx
%define sleep 0x20000d ;; definido en defines.h tambien

BITS 32
go64:
    lgdt [GDT_DESC]
    mov ax, 3<<3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ;prender el bit 5(6th bit) para activar PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080          ; Seleccionamos EFER MSR poniendo 0xC0000080 en ECX
    rdmsr                        ; Leemos el registro en EDX:EAX.
    or eax, 1 << 8               ; Seteamos el bit de modo largo que es el noveno bit (contando desde 0) osea el bit 8.
    wrmsr                        ; Escribimos nuevamente al registro.

    mov eax, cr0                 ; Obtenemos el registro de control 0 actual.
    or eax, 1 << 31              ; Habilitamos el bit de Paginacion que es el 32vo bit (contando desde 0) osea el bit 31
    mov cr0, eax                 ; escribimos el nuevo valor sobre el registro de control
    jmp (2<<3):long_mode

mensaje_ap_started_msg: db "AP en 64 :D"
mensaje_ap_started_len: equ $ - mensaje_ap_started_msg

;------------------------------------------------------------------------------------------------------
;------------------------------- comienzo modo largo --------------------------------------------------
;------------------------------------------------------------------------------------------------------


BITS 64

long_mode:
    ;limpio los registros
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor rbp, rbp
    xor rsp, rsp
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

    ;consigo id del core para saber q stack le corresponde
    get_lapic_id
    ;setear la pila en para el kernel
    mov esp, [core_stack_ptrs + eax * 8];la pila va a partir de kernelStackPtrBSP(expand down, OJO)

    MOV rbp, rsp;pongo base y tope juntos.

    ;levanto la IDT de 64 bits, es unica para todos los cores
    lidt [IDT_DESC]

    ;la IDT esta inicializada por el BSP

    ;el controlador de interrupciones ya esta inicializado por el BSP

    ;imprimir mensaje en pantalla
    imprimir_texto_ml mensaje_ap_started_msg, mensaje_ap_started_len, 0x0F, 10, 0

    cli

    ;llamo al entrypoint en kmain64
    ;call startKernel64_APMODE
    do_sort:
        call sort_ap
        cmp byte [sleep], 1
        jne do_sort

    sleep_ap:
        hlt
        jmp sleep_ap

; -------------------------------------------------------------------------- ;;