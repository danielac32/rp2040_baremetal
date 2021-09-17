.syntax unified
.cpu cortex-m0plus
.thumb



.section .vectors, "ax"
.align 2

.global __vectors, __VECTOR_TABLE
__VECTOR_TABLE:
__vectors:
.word __StackTop
.word _reset_handler
.word isr_nmi
.word isr_hardfault
.word isr_invalid
.word isr_invalid
.word isr_invalid
.word isr_invalid
.word isr_invalid
.word isr_invalid
.word isr_invalid
.word isr_svcall
.word isr_invalid
.word isr_invalid
.word isr_pendsv
.word isr_systick
.word isr_irq0
.word isr_irq1
.word isr_irq2
.word isr_irq3
.word isr_irq4
.word isr_irq5
.word isr_irq6
.word isr_irq7
.word isr_irq8
.word isr_irq9
.word isr_irq10
.word isr_irq11
.word isr_irq12
.word isr_irq13
.word isr_irq14
.word isr_irq15
.word isr_irq16
.word isr_irq17
.word isr_irq18
.word isr_irq19
.word isr_irq20
.word isr_irq21
.word isr_irq22
.word isr_irq23
.word isr_irq24
.word isr_irq25
.word isr_irq26
.word isr_irq27
.word isr_irq28
.word isr_irq29
.word isr_irq30
.word isr_irq31





.macro decl_isr_bkpt name
.weak \name
.type \name,%function
.thumb_func
\name:
    bkpt #0
.endm


decl_isr_bkpt isr_invalid
decl_isr_bkpt isr_nmi
decl_isr_bkpt isr_hardfault
decl_isr_bkpt isr_svcall
decl_isr_bkpt isr_pendsv
decl_isr_bkpt isr_systick

.macro decl_isr name
.weak \name
.type \name,%function
.thumb_func
\name:
.endm

decl_isr isr_irq0
decl_isr isr_irq1
decl_isr isr_irq2
decl_isr isr_irq3
decl_isr isr_irq4
decl_isr isr_irq5
decl_isr isr_irq6
decl_isr isr_irq7
decl_isr isr_irq8
decl_isr isr_irq9
decl_isr isr_irq10
decl_isr isr_irq11
decl_isr isr_irq12
decl_isr isr_irq13
decl_isr isr_irq14
decl_isr isr_irq15
decl_isr isr_irq16
decl_isr isr_irq17
decl_isr isr_irq18
decl_isr isr_irq19
decl_isr isr_irq20
decl_isr isr_irq21
decl_isr isr_irq22
decl_isr isr_irq23
decl_isr isr_irq24
decl_isr isr_irq25
decl_isr isr_irq26
decl_isr isr_irq27
decl_isr isr_irq28
decl_isr isr_irq29
decl_isr isr_irq30
decl_isr isr_irq31


.global __unhandled_user_irq
.thumb_func
__unhandled_user_irq:
    bl __get_current_exception
    subs r0, #16
.global unhandled_user_irq_num_in_r0
unhandled_user_irq_num_in_r0:
    bkpt #0

.section .reset, "ax"


.type _entry_point,%function
.thumb_func
.global _entry_point
_entry_point:

    #movs r0, #0
   ldr r0, =__vectors

__vector_entry:
    ldr r1, =(0xe0000000 + 0x0000ed08)
    str r0, [r1]
    ldmia r0!, {r1, r2}
    msr msp, r1
    bx r2
 
.type _reset_handler,%function
.thumb_func
_reset_handler:

    ldr r0, =(0xd0000000 + 0x00000000)
    ldr r0, [r0]
    cmp r0, #0
    //bne wait_for_vector

    adr r4, data_cpy_table


1:
    ldmia r4!, {r1-r3}
    cmp r1, #0
    beq 2f
    bl data_cpy
    b 1b
2:


    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
    movs r0, #0
    b bss_fill_test
bss_fill_loop:
    stm r1!, {r0}
bss_fill_test:
    cmp r1, r2
    bne bss_fill_loop

platform_entry:

   
   // ldr r1, =runtime_init
   /// blx r1
    // runtime_init esta en la carpeta src/init
    ldr r1, =init
    blx r1
    ldr r1, =_exit
    blx r1


.weak _exit
.type _exit,%function
.thumb_func
_exit:
1:
    bkpt #0
    b 1b

data_cpy_loop:
    ldm r1!, {r0}
    stm r2!, {r0}
data_cpy:
    cmp r2, r3
    blo data_cpy_loop
    bx lr



.align 2
data_cpy_table:





.word __etext
.word __data_start__
.word __data_end__

.word __scratch_x_source__
.word __scratch_x_start__
.word __scratch_x_end__

.word __scratch_y_source__
.word __scratch_y_start__
.word __scratch_y_end__

.word 0





.weak runtime_init
.type runtime_init,%function
.thumb_func
runtime_init:
    bx lr



 

.global __get_current_exception
.thumb_func
__get_current_exception:
    mrs r0, ipsr
    uxtb r0, r0
    bx lr




.section .stack

.align 5
    .equ StackSize, 0x800u
.space StackSize

.section .heap
.align 2
    .equ HeapSize, 0x800
.space HeapSize

.global	disable
.global	restore
.global	enable

disable:
	mrs	r0, psr		/* Copy the CPSR into r0		*/
	cpsid	i		/* Disable interrupts			*/
	bx	lr		/* Return the CPSR			*/

 
restore:
	msr	psr_nzcvq, r0	/* Restore the CPSR			*/
	cpsie	i		
	bx 	lr		/* Return to caller			*/

 
enable:
	cpsie	i		/* Enable interrupts			*/
	bx	lr		/* Return				*/


.globl delay
.thumb_func
delay:
    subs r0,#1
    bne delay
    bx lr

.text
.align 4
.force_thumb
.global __aeabi_idiv
__aeabi_idiv:
	ldr r3, =0xD0000000
	str r0, [r3, #0x68]
	str r1, [r3, #0x6C]
	b 1f
1:	b 1f
1:	b 1f
1:	b 1f
1:	ldr r0, [r3, #0x70]
	bx lr


.text
.align 4
.force_thumb
.global __aeabi_idivmod
__aeabi_idivmod:
	ldr r3, =0xD0000000
	str r0, [r3, #0x68]
	str r1, [r3, #0x6C]
	b 1f
1:	b 1f
1:	b 1f
1:	b 1f
1:	ldr r0, [r3, #0x74]
	bx lr


.text
.align 4
.force_thumb
.global __aeabi_uidiv
__aeabi_uidiv:
	ldr r3, =0xD0000000
	str r0, [r3, #0x60]
	str r1, [r3, #0x64]
	b 1f
1:	b 1f
1:	b 1f
1:	b 1f
1:	ldr r0, [r3, #0x70]
	bx lr


.text
.align 4
.force_thumb
.global __aeabi_uidivmod
__aeabi_uidivmod:
	ldr r3, =0xD0000000
	str r0, [r3, #0x60]
	str r1, [r3, #0x64]
	b 1f
1:	b 1f
1:	b 1f
1:	b 1f
1:	ldr r0, [r3, #0x74]
	bx lr




.text
.align 0
.force_thumb
.syntax unified
.global __gnu_thumb1_case_uqi
.thumb_func
.type __gnu_thumb1_case_uqi,function
__gnu_thumb1_case_uqi:
push	{r1}
mov	r1, lr
lsrs	r1, r1, #1
lsls	r1, r1, #1
ldrb	r1, [r1, r0]
lsls	r1, r1, #1
add	lr, lr, r1
pop	{r1}
bx	lr
.size __gnu_thumb1_case_uqi, . - __gnu_thumb1_case_uqi

.text
.align 0
.force_thumb
.syntax unified
.global __gnu_thumb1_case_uhi
.thumb_func
.type __gnu_thumb1_case_uhi,function
__gnu_thumb1_case_uhi:
push	{r0, r1}
mov	r1, lr
lsrs	r1, r1, #1
lsls	r0, r0, #1
lsls	r1, r1, #1
ldrh	r1, [r1, r0]
lsls	r1, r1, #1
add	lr, lr, r1
pop	{r0, r1}
bx	lr
.size __gnu_thumb1_case_uhi, . - __gnu_thumb1_case_uhi
