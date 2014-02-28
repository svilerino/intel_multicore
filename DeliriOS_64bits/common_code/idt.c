#include <idt.h>
#include <isr.h>

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
    IDT_ENTRY(1, KERNEL_TRAP_GATE_TYPE);//reservada para intel
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
    IDT_ENTRY(15, KERNEL_TRAP_GATE_TYPE);//intel reserved DO NOT USE
    IDT_ENTRY(16, KERNEL_TRAP_GATE_TYPE);//x87 FPU ERROR.(math fault)
    IDT_ENTRY(17, KERNEL_TRAP_GATE_TYPE);//memory alingment check ==================================>>>>>>>>>>>>>>>>> pushea error code 0
    IDT_ENTRY(18, KERNEL_TRAP_GATE_TYPE);//machine check
    IDT_ENTRY(19, KERNEL_TRAP_GATE_TYPE);//SIMD excepcion

    IDT_ENTRY(20, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(21, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(22, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(23, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(24, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(25, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(26, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(27, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(28, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(29, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(30, KERNEL_TRAP_GATE_TYPE);
    IDT_ENTRY(31, KERNEL_TRAP_GATE_TYPE);

    //IDT_ENTRY(20, *);//Reserved -> intel use only
    //...
    //IDT_ENTRY(31, *);//Reserved -> intel use only
    //----------------------------------------------------------------------------------------------------------------
    //...user defined interrupts(IRQs)
    IDT_ENTRY(32, KERNEL_INT_GATE_TYPE);//clock
    IDT_ENTRY(33, KERNEL_INT_GATE_TYPE);//keyboard

    IDT_ENTRY(34, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(35, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(36, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(37, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(38, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(39, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(40, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(41, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(42, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(43, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(44, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(45, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(46, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(47, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(48, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(49, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(50, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(51, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(52, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(53, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(54, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(55, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(56, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(57, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(58, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(59, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(60, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(61, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(62, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(63, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(64, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(65, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(66, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(67, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(68, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(69, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(70, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(71, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(72, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(73, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(74, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(75, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(76, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(77, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(78, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(79, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(80, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(81, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(82, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(83, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(84, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(85, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(86, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(87, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(88, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(89, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(90, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(91, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(92, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(93, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(94, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(95, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(96, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(97, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(98, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(99, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(100, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(101, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(102, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(103, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(104, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(105, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(106, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(107, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(108, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(109, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(110, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(111, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(112, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(113, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(114, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(115, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(116, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(117, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(118, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(119, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(120, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(121, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(122, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(123, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(124, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(125, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(126, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(127, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(128, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(129, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(130, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(131, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(132, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(133, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(134, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(135, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(136, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(137, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(138, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(139, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(140, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(141, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(142, KERNEL_INT_GATE_TYPE);

    //;;;;;;;;;;;;;;;;;;;;;;;;;
    //;;;;;Syscall examples;;;;
    //;;;;;;;;;;;;;;;;;;;;;;;;;

    //IDT_ENTRY(80/*0x50*/, SERVICE_INT_GATE_TYPE);
    //IDT_ENTRY(102/*0x66*/, SERVICE_INT_GATE_TYPE);

    //Evitar interrupciones spurias usando el spurious intr number. Esto se
    //usa para el APIC en el vector.
    idt[SPURIOUS_INTR_NUM].segsel = (uint16_t) 0x10;/*64 bits code segment*/
    idt[SPURIOUS_INTR_NUM].attr = (uint16_t) KERNEL_INT_GATE_TYPE;
    idt[SPURIOUS_INTR_NUM].offset_0_15 = (uint16_t) ((uint64_t)(&_isr_spurious) & (uint64_t) 0xFFFF);
    idt[SPURIOUS_INTR_NUM].offset_16_31 = (uint16_t) ((uint64_t)(&_isr_spurious) >> 16 & (uint64_t) 0xFFFF);
    idt[SPURIOUS_INTR_NUM].offset_32_63 = (uint32_t) ((uint64_t)(&_isr_spurious) >> 32 & (uint64_t) 0xFFFFFFFF);
    idt[SPURIOUS_INTR_NUM].zeroPadd = 0x0;

    IDT_ENTRY(144, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(145, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(146, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(147, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(148, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(149, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(150, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(151, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(152, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(153, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(154, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(155, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(156, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(157, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(158, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(159, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(160, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(161, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(162, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(163, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(164, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(165, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(166, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(167, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(168, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(169, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(170, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(171, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(172, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(173, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(174, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(175, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(176, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(177, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(178, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(179, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(180, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(181, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(182, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(183, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(184, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(185, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(186, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(187, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(188, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(189, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(190, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(191, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(192, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(193, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(194, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(195, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(196, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(197, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(198, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(199, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(200, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(201, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(202, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(203, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(204, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(205, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(206, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(207, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(208, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(209, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(210, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(211, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(212, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(213, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(214, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(215, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(216, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(217, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(218, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(219, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(220, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(221, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(222, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(223, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(224, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(225, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(226, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(227, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(228, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(229, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(230, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(231, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(232, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(233, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(234, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(235, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(236, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(237, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(238, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(239, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(240, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(241, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(242, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(243, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(244, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(245, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(246, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(247, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(248, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(249, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(250, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(251, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(252, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(253, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(254, KERNEL_INT_GATE_TYPE);
    IDT_ENTRY(255, KERNEL_INT_GATE_TYPE);

}
