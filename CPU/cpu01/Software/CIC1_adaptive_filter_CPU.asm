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
		MMOV32	  	@LFRAME + LFRAME_MR3, MR3 	; save MR3
		MMOV16 		@LFRAME + LFRAME_CIC_addr, MAR1
		MMOV16 		@LFRAME + LFRAME_CIC_global_addr, MAR0
		MMOV32		MR1, *MAR1+[CIC1_adaptive_struct.range_modifier]
		MMPYF32		MR0, MR0, MR1
||		MMOV32	  	MR1, *MAR1+[CIC1_adaptive_struct.integrator]
		MF32TOI32	MR0, MR0
		MADD32		MR1, MR1, MR0
		MMOV32	  	*MAR1+[CIC1_adaptive_struct.integrator], MR1

		MMOVZ16		MR0, @LFRAME + LFRAME_CIC_addr

		MMOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.cycle_enable]
		MMOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_memory]
		MADD32		MR0, MR0, MR2
		MMOV16		MAR0, MR0, #0
		MBCNDD 		_Skip, EQ
		MMOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_OSR_adaptive]
		MMOV16 		MAR0, @LFRAME + LFRAME_CIC_global_addr
		MMOV32	  	MR0, *MAR0+[CIC1_adaptive_struct.decimator_memory]

		MMOV32	  	*MAR0+[CIC1_adaptive_struct.decimator_memory], MR1

		MSUB32		MR0, MR1, MR0
		MI32TOF32 	MR0, MR0
		MMPYF32		MR0, MR0, MR2
||		MMOV32		MR2, *MAR1+[CIC1_adaptive_struct.div_range_modifier]
		MMPYF32		MR0, MR0, MR2
		MMOV32	  	*MAR1+[CIC1_adaptive_struct.out_temp], MR0

_Skip:
		MNOP
		MMOVZ16		MR0, @LFRAME + LFRAME_CIC_addr
		MMOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.cycle_enable + 2]
		MMOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_memory + 2]
		MADD32		MR0, MR0, MR2
		MMOV16		MAR0, MR0, #0
		MBCNDD 		_Skip2, EQ
		MMOV32	  	MR2, *MAR0+[CIC1_adaptive_global_struct.div_OSR_adaptive + 2]
		MMOV16 		MAR0, @LFRAME + LFRAME_CIC_global_addr
		MMOV32	  	MR0, *MAR0+[CIC1_adaptive_struct.decimator_memory+2*CIC_upsample2]

		MMOV32	  	*MAR0+[CIC1_adaptive_struct.decimator_memory+2*CIC_upsample2], MR1
		MMOV32	  	MR1, *MAR1+[CIC1_adaptive_struct.integrator]

		MSUB32		MR0, MR1, MR0
		MI32TOF32 	MR0, MR0
		MMPYF32		MR0, MR0, MR2
||		MMOV32		MR2, *MAR1+[CIC1_adaptive_struct.div_range_modifier]
		MMPYF32		MR0, MR0, MR2
		MMOV32	  	*MAR1+[CIC1_adaptive_struct.out_temp+2], MR0

_Skip2:
		MNOP
		MMOV32		MR3, @LFRAME + LFRAME_MR3	; restore MR3
		MMOV32		MR0, *MAR0+[CIC1_adaptive_global_struct.select_output]
		MRCNDD		UNC							; return call
		MMOV32	  	MR0, *MAR1+[CIC1_adaptive_struct.out_temp], EQ
		MMOV32	  	MR0, *MAR1+[CIC1_adaptive_struct.out_temp+2], NEQ
		MMOV32	  	*MAR1+[CIC1_adaptive_struct.out], MR0


		.unasg	LFRAME

; end of file
