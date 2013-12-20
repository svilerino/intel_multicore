#include "idt.h"
#include "isr.h"

idt_entry idt[IDT_ENTRIES_COUNT] = { };

idt_descriptor IDT_DESC = {
    .idt_length = sizeof(idt) - 1,
    .idt_addr = (uint64_t) &idt
};

#define IDT_ENTRY(numero, attrsShort)                                                                               \
    idt[numero].segsel = (uint16_t) 0x10;/*64 bits code segment*/                                                   \
    idt[numero].attr = (uint16_t) attrsShort;                                                                       \
    idt[numero].offset_0_15 = (uint16_t) ((uint64_t)(&_isr ## numero) & (uint64_t) 0xFFFF);                         \
    idt[numero].offset_16_31 = (uint16_t) ((uint64_t)(&_isr ## numero) >> 16 & (uint64_t) 0xFFFF);                  \
    idt[numero].offset_32_63 = (uint32_t) ((uint64_t)(&_isr ## numero) >> 32 & (uint64_t) 0xFFFFFFFF);              \
    idt[numero].zeroPadd = 0x0;

void idt_inicializar() {
    IDT_ENTRY(0, KERNEL_TRAP_GATE_TYPE);//division por cero(instr div, idiv)
    //IDT_ENTRY(1, *);//reservada para intel
    IDT_ENTRY(2, KERNEL_TRAP_GATE_TYPE);//NMI interrupt
    IDT_ENTRY(3, KERNEL_TRAP_GATE_TYPE);//breakpoint (origen int 3)
    IDT_ENTRY(4, KERNEL_TRAP_GATE_TYPE);//overflow(INTO instruction)
    IDT_ENTRY(5, KERNEL_TRAP_GATE_TYPE);//bound range exceeded(solo con instruccion bound)
    IDT_ENTRY(6, KERNEL_TRAP_GATE_TYPE);//invalid opcode
    IDT_ENTRY(7, KERNEL_TRAP_GATE_TYPE);//device not available(math processor)
    IDT_ENTRY(8, KERNEL_TRAP_GATE_TYPE);//double fault ==================================>>>>>>>>>>>>>>>>> pushea error code 0
    IDT_ENTRY(9, KERNEL_TRAP_GATE_TYPE);//coprocessor segment overrun(reserved) ==> desp del intel 386 no se genera esta excepcion
    IDT_ENTRY(10, KERNEL_TRAP_GATE_TYPE);//invalid tss ==================================>>>>>>>>>>>>>>>>> pushea error code
    IDT_ENTRY(11, KERNEL_TRAP_GATE_TYPE);//segment not present ==================================>>>>>>>>>>>>>>>>> pushea error code
    IDT_ENTRY(12, KERNEL_TRAP_GATE_TYPE);//stack segment fault ==================================>>>>>>>>>>>>>>>>> pushea error code
    IDT_ENTRY(13, KERNEL_TRAP_GATE_TYPE);//general protection ==================================>>>>>>>>>>>>>>>>> pushea error code
    IDT_ENTRY(14, KERNEL_TRAP_GATE_TYPE);//page fault ==================================>>>>>>>>>>>>>>>>> pushea error code
    //IDT_ENTRY(15, *);//intel reserved DO NOT USE
    IDT_ENTRY(16, KERNEL_TRAP_GATE_TYPE);//x87 FPU ERROR.(math fault)
    IDT_ENTRY(17, KERNEL_TRAP_GATE_TYPE);//memory alingment check ==================================>>>>>>>>>>>>>>>>> pushea error code 0
    IDT_ENTRY(18, KERNEL_TRAP_GATE_TYPE);//machine check
    IDT_ENTRY(19, KERNEL_TRAP_GATE_TYPE);//SIMD excepcion
    //IDT_ENTRY(20, *);//Reserved -> intel use only
    //...
    //IDT_ENTRY(31, *);//Reserved -> intel use only
    //----------------------------------------------------------------------------------------------------------------
    //...user defined interrupts
    IDT_ENTRY(32, KERNEL_INT_GATE_TYPE);//clock
    IDT_ENTRY(33, KERNEL_INT_GATE_TYPE);//keyboard


    //;;;;;;;;;;;;;;;;;;;;;;;;;
    //;;;;;Syscall examples;;;;
    //;;;;;;;;;;;;;;;;;;;;;;;;;

    //IDT_ENTRY(80/*0x50*/, SERVICE_INT_GATE_TYPE);
    //IDT_ENTRY(102/*0x66*/, SERVICE_INT_GATE_TYPE);

}
