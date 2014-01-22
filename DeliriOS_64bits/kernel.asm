%include "macros/asm_screen_utils.mac"
%include "macros/real_mode_macros.mac"

global kernel

;; GDT
extern GDT_DESC

;; IDT
extern IDT_DESC
extern idt_inicializar

;; STACK
extern kernelStackPtrBSP

;; PIC
extern deshabilitar_pic
extern resetear_pic
extern habilitar_pic

;;paginacion
extern krnPML4T
extern krnPDPT
extern krnPDT
extern krnPT

;; startup
extern startKernel64_BSPMODE
extern initialize_timer
extern multiprocessor_init
extern console_setYCursor
extern console_setXCursor

;;mergesort cosas
extern mergesort
extern array_global
extern start_point
extern done
extern mergesort_pm

;; Saltear seccion de datos(para que no se ejecute)
BITS 16
JMP kernel

;;
;; Seccion de datos
;; -------------------------------------------------------------------------- ;;
mensaje_inicioprot_msg:     db '[BSP]Starting up in protected mode...'
mensaje_inicioprot_len      equ $ - mensaje_inicioprot_msg

mensaje_inicio64_msg:     db '[BSP]Starting up in long mode(IA32e compatibility mode)...'
mensaje_inicio64_len      equ $ - mensaje_inicio64_msg

mensaje_inicio64real_msg:     db '[BSP]Starting up in full long mode...'
mensaje_inicio64real_len      equ $ - mensaje_inicio64real_msg

mensaje_64bitserr_msg:     db '[BSP]FAIL! 64 bits mode unavailable! -> Kernel Halted.'
mensaje_64bitserr_len      equ $ - mensaje_64bitserr_msg

mensaje_cpuiderr_msg:     db '[BSP]FAIL! CPUID unavailable! -> Kernel Halted.'
mensaje_cpuiderr_len        equ $ - mensaje_cpuiderr_msg

mensaje_paging4g_msg:             db '[BSP]Configuring PAE paging up to 4GB...'
mensaje_paging4g_len              equ $ - mensaje_paging4g_msg

mensaje_paging64g_msg:             db '[BSP]Extending paging up to 64GB...'
mensaje_paging64g_len              equ $ - mensaje_paging64g_msg

mensaje_interrupt_msg:             db '[BSP]Initializing interrupt handling...'
mensaje_interrupt_len              equ $ - mensaje_interrupt_msg

mensaje_timer_msg:             db '[BSP]Configuring timer...'
mensaje_timer_len              equ $ - mensaje_timer_msg

mensaje_multicore_msg:             db '[BSP]Starting up multicore...'
mensaje_multicore_len              equ $ - mensaje_multicore_msg

mensaje_ok_msg:             db 'OK!'
mensaje_ok_len              equ $ - mensaje_ok_msg

array_prueba:   db 'ndjeropaktjclqa'
array_len:  equ $ - array_prueba

;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;------------------------------------------------------------------------------------------------------
;------------------------------- comienzo modo real ---------------------------------------------------
;------------------------------------------------------------------------------------------------------
BITS 16
kernel:
    ; Deshabilitar interrupciones
    cli

    ; habilitar A20
    call habilitar_A20

    ;desaparecer cursor en pantalla
    mov BL, 0
    dec BL
    mov BH, 0
    dec BH
    set_cursor

    ; cargar la GDT
    lgdt [GDT_DESC]

    imprimir_texto_mr mensaje_inicioprot_msg, mensaje_inicioprot_len, 0x0F, 0, 320

    ; setear el bit PE del registro CR0
    mov EAX, CR0;levanto registro CR0 para pasar a modo protegido
    or EAX, 1;hago un or con una mascara de 0...1 para setear el bit de modo protegido
    mov CR0, EAX

    ; pasar a modo protegido
    jmp 00001000b:protected_mode; saltamos a modo protegido, modificamos el cs con un jump y la eip(program counter)
    ;{index:1 | gdt/ldt: 0 | rpl: 00} => 1000
    ;aca setie el selector de segmento cs al segmento de codigo del kernel

;------------------------------------------------------------------------------------------------------
;------------------------------- comienzo modo protegido ----------------------------------------------
;------------------------------------------------------------------------------------------------------

BITS 32
protected_mode:
    ;limpio los registros
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    xor esi, esi
    xor edi, edi
    xor ebp, ebp
    xor esp, esp

    ;cargo los selectores de segmento de modo protegido
    xor eax, eax
    mov ax, 00011000b;{index:3 | gdt/ldt: 0 | rpl: 00} segmento de datos de kernel
    mov ds, ax;cargo como selector de segmento de datos al descriptor del indice 2 que corresponde a los datos del kernel
    mov es, ax;cargo tambien estos selectores auxiliares con el descriptor de datos del kernel
    mov fs, ax;cargo tambien estos selectores auxiliares con el descriptor de datos del kernel
    mov gs, ax;cargo tambien estos selectores auxiliares con el descriptor de datos del kernel
    mov ss, ax;cargo el selector de pila en el segmento de datos del kernel
    mov esp, [kernelStackPtrBSP];la pila va a partir de kernelStackPtrBSP(expand down, OJO)
    mov ebp, esp;pongo base y tope juntos.

    imprimir_texto_mp mensaje_ok_msg, mensaje_ok_len, 0x02, 2, mensaje_inicioprot_len

    ; Chequeo de disponibilidad de uso de CPUID
    ; tomado de http://wiki.osdev.org/User:Stephanvanschaik/Setting_Up_Long_Mode#Detection_of_CPUID
    pushfd               ; Store the FLAGS-register.
    pop eax              ; Restore the A-register.
    mov ecx, eax         ; Set the C-register to the A-register.
    xor eax, 1 << 21     ; Flip the ID-bit, which is bit 21.
    push eax             ; Store the A-register.
    popfd                ; Restore the FLAGS-register.
    pushfd               ; Store the FLAGS-register.
    pop eax              ; Restore the A-register.
    push ecx             ; Store the C-register.
    popfd                ; Restore the FLAGS-register.
    xor eax, ecx         ; Do a XOR-operation on the A-register and the C-register.
    jz CPUIDNoDisponible; The zero flag is set, no CPUID.
    ; here CPUID is available for use.

    ;Deteccion de modo 64 bits y mensaje de error sino esta disponible halteamos.
    mov eax, 0x80000000    ; pasamos parametros 0x80000000.
    cpuid
    cmp eax, 0x80000001    ; 0x80000001 significa que esta habilitado.
    jb ModoLargoNoDisp    ; si es menor, modo largo no esta disponible

    ;aca tenemos certeza de que tenemos modo de 64 bits disponible

;inicializo paginacion
    imprimir_texto_mp mensaje_paging4g_msg, mensaje_paging4g_len, 0x0F, 3, 0

;    Estoy usando paginacion IA-32e => bits CR0.PG=1 + CR4.PAE=1 + EFER.LME=1
;    paginas de 2mb

;symbol containing krnPML4T pointer is krnPML4T
;symbol containing krnPDPT pointer is krnPDPT
;symbol containing krnPDT pointer is krnPDT

; Mapeo 4GB con paginas de 2MB
; la estructura PML4 esta en krnPML4T, creamos la primer entrada aca
    cld                     ;limpia el direction flag -> http://en.wikipedia.org/wiki/Direction_flag
    mov edi, [krnPML4T]
    mov eax, [krnPDPT]
    or eax, 0x7; attributes nibble
    stosd
    xor eax, eax
    stosd

    ;Nota http://en.wikipedia.org/wiki/X86_instruction_listings
    ;stosd es equivalente a *ES:EDI = EAX; => store string double word

; creo las entradas en PDP
; la estructura PDP esta en krnPDPT, creamos las entradas desde este lugar
    mov ecx, 64             ; hago 64 PDPE entries cada una mapea 1gb de memoria -> mas abajo las mapeo en x64 mode
    mov edi, [krnPDPT]
    mov eax, [krnPDT]
    or eax, 0x7; attributes nibble
crear_pdpentry:
    stosd
    push eax
    xor eax, eax
    stosd
    pop eax
    add eax, 0x00001000     ;avanzo 4k
    dec ecx
    cmp ecx, 0
    jne crear_pdpentry

; Crear las entradas en PD
    mov edi, [krnPDT]
    mov eax, 0x0000008F     ; para activar paginas de 2MB seteamos el bit 7
    xor ecx, ecx
pd_loop:
    stosd
    push eax
    xor eax, eax
    stosd
    pop eax
    add eax, 0x00200000     ;incremento 2 MB
    inc ecx
    cmp ecx, 2048
    jne pd_loop            ; Create 2048 2 MiB page maps.

    ; apunto cr3 al PML4
    mov eax, [krnPML4T]
    mov cr3, eax

    imprimir_texto_mp mensaje_ok_msg, mensaje_ok_len, 0x02, 3, mensaje_paging4g_len

    ;comienzo a inicializar 64 bits
    imprimir_texto_mp mensaje_inicio64_msg, mensaje_inicio64_len, 0x0F, 4, 0

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

    imprimir_texto_mp mensaje_ok_msg, mensaje_ok_len, 0x02, 4, mensaje_inicio64_len

    imprimir_texto_mp mensaje_inicio64real_msg, mensaje_inicio64real_len, 0x0F, 5, 0

    ;estamos en modo ia32e compatibilidad con 32 bits
    ;comienzo pasaje a 64 bits puro

    jmp 00010000b:long_mode; saltamos a modo largo, modificamos el cs con un jump y la eip(program counter)
    ;{index:2 | gdt/ldt: 0 | rpl: 00} => 00010000
    ;aca setie el selector de segmento cs al segmento de codigo del kernel

; Funciones auxiliares en 32 bits!
CPUIDNoDisponible:
imprimir_texto_mp mensaje_cpuiderr_msg, mensaje_cpuiderr_len, 0x0C, 3, mensaje_inicio64_len

    cli
    hlt
    jmp CPUIDNoDisponible

ModoLargoNoDisp:
    imprimir_texto_mp mensaje_64bitserr_msg, mensaje_64bitserr_len, 0x0C, 3, mensaje_inicio64_len

    cli
    hlt
    jmp ModoLargoNoDisp


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

    ;levanto segmentos con valores iniciales
    XOR rax, rax
    MOV ax, 00011000b;{index:3 | gdt/ldt: 0 | rpl: 00} segmento de datos de kernel
    MOV ds, ax;cargo como selector de segmento de datos al descriptor del indice 2 que corresponde a los datos del kernel
    MOV es, ax;cargo tambien estos selectores auxiliares con el descriptor de datos del kernel
    MOV fs, ax;cargo tambien estos selectores auxiliares con el descriptor de datos del kernel
    MOV gs, ax;cargo tambien estos selectores auxiliares con el descriptor de datos del kernel

    ;cargo el selector de pila en el segmento de datos del kernel
    MOV ss, ax

    ;setear la pila en para el kernel
    MOV rsp, [kernelStackPtrBSP];la pila va a partir de kernelStackPtrBSP(expand down, OJO)
    MOV rbp, rsp;pongo base y tope juntos.

    imprimir_texto_ml mensaje_ok_msg, mensaje_ok_len, 0x02, 5, mensaje_inicio64real_len

    ;levanto la IDT de 64 bits
    lidt [IDT_DESC]
    call idt_inicializar

    imprimir_texto_ml mensaje_paging64g_msg, mensaje_paging64g_len, 0x0F, 6, 0

    ;genero el resto de las estructuras de paginacion para mapear 64 gb
    ;lo tengo que hacer aca porque la direccion es de 64 bits, en modo protegido de 32 no podia
    mov rcx, 0x0000000000000000
    mov rax, 0x000000010000008F
    mov rdi, [krnPDT]
    add rdi, 0x4000;los primeros 4gb ya los tengo bien mapeados
loop_64g_structure:
    stosq
    add rax, 0x0000000000200000
    add rcx, 1
    cmp rcx, 30720         ; 32768 - 2048 (ya mapeamos 2048*2mb)
    jne loop_64g_structure

    imprimir_texto_ml mensaje_ok_msg, mensaje_ok_len, 0x02, 6, mensaje_paging64g_len

    imprimir_texto_ml mensaje_interrupt_msg, mensaje_interrupt_len, 0x0F, 7, 0

    ;configurar controlador de interrupciones
    CALL deshabilitar_pic
    CALL resetear_pic
    CALL habilitar_pic

    ;habilito las interrupciones! necesario para timer
    STI
    imprimir_texto_ml mensaje_ok_msg, mensaje_ok_len, 0x02, 7, mensaje_interrupt_len

    ;initialize_timer -> para multicore init es necesario por los core_sleep
    imprimir_texto_ml mensaje_timer_msg, mensaje_timer_len, 0x0F, 8, 0
    call initialize_timer
    imprimir_texto_ml mensaje_ok_msg, mensaje_ok_len, 0x02, 8, mensaje_timer_len

    ;inicializamos multicore
    imprimir_texto_ml mensaje_multicore_msg, mensaje_multicore_len, 0x0F, 9, 0
    call multiprocessor_init
    imprimir_texto_ml mensaje_ok_msg, mensaje_ok_len, 0x02, 9, mensaje_multicore_len

    ;llamo al entrypoint en kmain64
    call startKernel64_BSPMODE

    ;mov rdi, array_global
    ;mov rsi, 52

    ;call mergesort

    ;imprimir_texto_ml  array_global, 52, 0x02, 11, 0

    ;fin inicio kernel para BSP en 64 bits!

    call mergesort_pm


    haltBspCore:
        imprimir_texto_ml array_global, 52, 0x02, 12, 0
        jmp haltBspCore

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
