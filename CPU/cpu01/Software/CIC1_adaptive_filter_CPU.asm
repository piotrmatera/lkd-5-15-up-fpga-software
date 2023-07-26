;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

		.cdecls   C,LIST,"Controllers.h"

		.if __TI_EABI__
		.asg	CIC1_adaptive_filter_CPUasm, _CIC1_adaptive_filter_CPUasm
		.endif

		.global _CIC1_adaptive_filter_CPUasm
        .sect ".TI.ramfunc"

; C prototype:
; float CIC1_adaptive_filter_CPUasm(struct CIC1_adaptive_global_struct *CIC_global, volatile struct CIC1_adaptive_struct *CIC, float input);
; 50

_CIC1_adaptive_filter_CPUasm:
;		MDEBUGSTOP
		MOV32		R1H, *+XAR5[CIC1_adaptive_struct.range_modifier]
		MPYF32		R0H, R0H, R1H
        MOVL		XAR6, XAR4
		F32TOI32	R0H, R0H
        ADDB		XAR6, #CIC1_adaptive_global_struct.select_output
		MOVL		XAR0, #CIC1_adaptive_struct.integrator
		MOV32	  	P, R0H
		ADDUL	  	P, *+XAR5[AR0]
		MOVL	  	*+XAR5[AR0], P

		MOVL	  	ACC, *+XAR4[CIC1_adaptive_global_struct.cycle_enable]
		BF	 		_Skip, EQ

		MOVL	  	XAR0, *+XAR4[CIC1_adaptive_global_struct.div_memory]
		ADDB		XAR0, #CIC1_adaptive_struct.decimator_memory
		MOVL	  	ACC, *+XAR5[AR0]
		MOVL	  	*+XAR5[AR0], P

		SUBL		P, ACC
		MOV32	  	R0H, P
		MOVL		XAR0, #CIC1_adaptive_global_struct.div_OSR_adaptive
		MOV32	  	R2H, *+XAR4[AR0]
		MOVL		XAR0, #CIC1_adaptive_struct.div_range_modifier
		MOV32		R3H, *+XAR5[AR0]
		I32TOF32 	R0H, R0H
		MPYF32		R2H, R2H, R3H
		NOP
		MPYF32		R0H, R0H, R2H
		MOVL		XAR0, #CIC1_adaptive_struct.out_temp
		MOV32	  	*+XAR5[AR0], R0H

_Skip:
		MOVL	  	ACC, *+XAR4[CIC1_adaptive_global_struct.cycle_enable + 2]
		BF	 		_Skip2, EQ

		MOVL	  	XAR0, *+XAR4[CIC1_adaptive_global_struct.div_memory + 2]
		MOV			ACC, #CIC1_adaptive_struct.decimator_memory+2*CIC_upsample2
		ADDL		XAR0, ACC
		MOVL	  	ACC, *+XAR5[AR0]
		MOVL	  	*+XAR5[AR0], P

		SUBL		P, ACC
		MOV32	  	R0H, P
		MOVL		XAR0, #CIC1_adaptive_global_struct.div_OSR_adaptive + 2
		MOV32	  	R2H, *+XAR4[AR0]
		MOVL		XAR0, #CIC1_adaptive_struct.div_range_modifier + 2
		MOV32		R3H, *+XAR5[AR0]
		I32TOF32 	R0H, R0H
		MPYF32		R2H, R2H, R3H
		NOP
		MPYF32		R0H, R0H, R2H
		MOVL		XAR0, #CIC1_adaptive_struct.out_temp + 2
		MOV32	  	*+XAR5[AR0], R0H
_Skip2:
		MOVL		ACC, *XAR6
		MOV32	  	R0H, *+XAR5[CIC1_adaptive_struct.out_temp], EQ
		MOV32	  	R0H, *+XAR5[CIC1_adaptive_struct.out_temp+2], NEQ
		MOV32	  	*+XAR5[CIC1_adaptive_struct.out], R0H

;Finish up
        LRETR                           ;Return

;end of function CIC1_adaptive_filter_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################

