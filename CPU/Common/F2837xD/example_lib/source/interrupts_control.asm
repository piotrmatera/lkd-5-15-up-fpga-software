
	.global __custom_disable_interrupts
	.global __custom_read_st1
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


; end of file
