;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

		.cdecls   C,LIST,"Controllers.h"

		.if __TI_EABI__
		.asg	CIC1_adaptive_filter_CPUasm, _CIC1_adaptive_filter_CPUasm
		.endif

		.global _CIC1_adaptive_filter_CPUasm
        .text

; C prototype:
; float CIC1_adaptive_filter_CPUasm(struct CIC1_adaptive_global_struct *CIC_global, volatile struct CIC1_adaptive_struct *CIC, float input);
; 47

_CIC1_adaptive_filter_CPUasm:
;		MDEBUGSTOP
		MOV32	  	@LFRAME + LFRAME_MR3, MR3 	; save MR3
		MOV16 		@LFRAME + LFRAME_CIC_addr, MAR1
		MOV16 		@LFRAME + LFRAME_CIC_global_addr, MAR0
		MOV32		MR1, *MAR1+[CIC1_adaptive_struct.range_modifier]
		MPYF32		MR0, MR0, MR1
||		MOV32	  	MR1, *MAR1+[CIC1_adaptive_struct.integrator]
		F32TOI32	MR0, MR0
		ADD32		MR1, MR1, MR0
		MOV32	  	*MAR1+[CIC1_adaptive_struct.integrator], MR1

		MOVZ16		MR0, @LFRAME + LFRAME_CIC_addr

		MOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.cycle_enable]
		MOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_memory]
		ADD32		MR0, MR0, MR2
		MOV16		MAR0, MR0, #0
		BF	 		_Skip, EQ
		MOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_OSR_adaptive]
		MOV16 		MAR0, @LFRAME + LFRAME_CIC_global_addr
		MOV32	  	MR0, *MAR0+[CIC1_adaptive_struct.decimator_memory]

		MOV32	  	*MAR0+[CIC1_adaptive_struct.decimator_memory], MR1

		SUB32		MR0, MR1, MR0
		I32TOF32 	MR0, MR0
		MPYF32		MR0, MR0, MR2
||		MOV32		MR2, *MAR1+[CIC1_adaptive_struct.div_range_modifier]
		MPYF32		MR0, MR0, MR2
		MOV32	  	*MAR1+[CIC1_adaptive_struct.out_temp], MR0

_Skip:
		NOP
		MOVZ16		MR0, @LFRAME + LFRAME_CIC_addr
		MOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.cycle_enable + 2]
		MOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_memory + 2]
		ADD32		MR0, MR0, MR2
		MOV16		MAR0, MR0, #0
		BF	 		_Skip2, EQ
		MOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_OSR_adaptive + 2]
		MOV16 		MAR0, @LFRAME + LFRAME_CIC_global_addr
		MOV32	  	MR0, *MAR0+[CIC1_adaptive_struct.decimator_memory+2*CIC_upsample2]

		MOV32	  	*MAR0+[CIC1_adaptive_struct.decimator_memory+2*CIC_upsample2], MR1
		MOV32	  	MR1, *MAR1+[CIC1_adaptive_struct.integrator]

		SUB32		MR0, MR1, MR0
		I32TOF32 	MR0, MR0
		MPYF32		MR0, MR0, MR2
||		MOV32		MR2, *MAR1+[CIC1_adaptive_struct.div_range_modifier]
		MPYF32		MR0, MR0, MR2
		MOV32	  	*MAR1+[CIC1_adaptive_struct.out_temp+2], MR0

_Skip2:
		NOP
		MOV32		MR3, @LFRAME + LFRAME_MR3	; restore MR3
		MOV32		MR0, *MAR0+[CIC1_adaptive_global_struct.select_output]
		MOV32	  	MR0, *MAR1+[CIC1_adaptive_struct.out_temp], EQ
		MOV32	  	MR0, *MAR1+[CIC1_adaptive_struct.out_temp+2], NEQ
		MOV32	  	*MAR1+[CIC1_adaptive_struct.out], MR0


;Finish up
        LRETR                           ;Return

;end of function CIC1_adaptive_filter_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################

