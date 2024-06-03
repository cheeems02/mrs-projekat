;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------
			.ref	WriteLed				; reference the extern function
			.ref	disp1					; reference the global variable
			.ref	disp2					; reference the global variable

;-------------------------------------------------------------------------------
    .sect ".text"
    .global _button1_ISR
    .global _button2_ISR
    .global _button3_ISR
    .global _button4_ISR

button1_ISR: // P1.1 Interrupt Service Routine
    push.w  SR
    push.w  R12
    push.w  R13
    mov.w   #start, R12
    call    R12
    bic.w   #BIT1, &P1IFG
    pop.w   R13
    pop.w   R12
    pop.w   SR
    reti

button2_ISR: // P2.1 Interrupt Service Routine
    push.w  SR
    push.w  R12
    push.w  R13
    mov.w   #minimum, R12
    call    R12
    bic.w   #BIT1, &P2IFG
    pop.w   R13
    pop.w   R12
    pop.w   SR
    reti

button3_ISR: // P1.4 Interrupt Service Routine
    push.w  SR
    push.w  R12
    push.w  R13
    mov.w   #average, R12
    call    R12
    bic.w   #BIT4, &P1IFG
    pop.w   R13
    pop.w   R12
    pop.w   SR
    reti

button4_ISR: // P1.5 Interrupt Service Routine
    push.w  SR
    push.w  R12
    push.w  R13
    mov.w   #maximum, R12
    call    R12
    bic.w   #BIT5, &P1IFG
    pop.w   R13
    pop.w   R12
    pop.w   SR
    reti

            .sect   ".int49"
            .short  CCR0ISR

