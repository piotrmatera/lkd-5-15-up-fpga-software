; file:    interrupts_control.asm
; author:  Piotr Romaniuk
; created: 2023-10-22
;

	.global __custom_disable_interrupts
	.global __custom_read_st1
	.global __custom_read_ipc_timer
	.ref _IpcRegs
		.text

__custom_disable_interrupts:
		PUSH	ST1					; push ST1 onto stack, SP += 1
		SETC    INTM		        ; disable interrupts
		MOV		AL, *--SP			; copy ST1 into AL, SP -= 1
		AND		AL, #0x1			; mask LSB
		LRETR

__custom_read_st1:
		PUSH	ST1					; push ST1 onto stack, SP += 1
		MOV		AL, *--SP			; copy ST1 into AL, SP -= 1
		LRETR

;####################################################################################################
;  odpowiednik funkcji ReadIpcTimer()
;
;  zwraca: Uint64 wartosc z licznika Ipc
;
; zabezpieczone przed wtraceniem ISR
; wylacza przerwania na 3 cykle CPU
; odtwarza stan przerwan po odczycie rejestrow
;
; C28x Embedded Application Binary Interface, Application Report
; SPRAC71A–March 2019–Revised February 2020
; @str. 22:
;  " 3.4 Return Values
;   The function return value is placed in the same register as the usual first argument register, based on its
;   type and size.
;   • 16-bit results are returned in the AL register.
;   • 32-bit results are returned in the ACC register.
;   • 64-bit results are returned in the ACC:P register pair."
;
; @str. 20:
;  "3.2.2 Callee-Saved Registers
; A called function is required to preserve the callee-saved registers so that they have the same value on
; return from a function as they had at the point of the call.
; Registers XAR1, XAR2, and XAR3 are callee-saved. If the target supports FPU, the R4H, R5H, R6H, and
; R7H registers are also callee-saved.
; All other general-purpose registers are caller-save; that is, they are not preserved across a call, so if their
; value is needed following the call, the caller is responsible for saving and restoring their contents."
;
;#####################################################################################################

__custom_read_ipc_timer:
		PUSH	ST1					; push ST1 onto stack, SP += 1
		MOV		AL, *--SP			; copy ST1 into AL, SP -= 1

		MOVW	DP,#_IpcRegs+12

		TBIT      AL,#0				;test ST1.INTM
   		B         _$L_intr_are_off,TC

   		SETC	INTM
   		 MOVL	P,@$BLOCKED(_IpcRegs)+12  ; IPCCOUNTERL
   		 MOVL	ACC,@$BLOCKED(_IpcRegs)+14 ; IPCCOUNTERH
   		CLRC	INTM

   		LRETR

_$L_intr_are_off:
   		MOVL	P,@$BLOCKED(_IpcRegs)+12  ; IPCCOUNTERL
   		MOVL	ACC,@$BLOCKED(_IpcRegs)+14 ; IPCCOUNTERH

   		LRETR





; end of file
