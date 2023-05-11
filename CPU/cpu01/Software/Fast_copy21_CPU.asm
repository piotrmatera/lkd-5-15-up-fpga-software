;Tomasz �wi�chowicz swiechowicz.tomasz@gmail.com

; C prototype:
; void Fast_copy21_CPUasm();
; x instructions without pre-jump

        .cdecls   C,LIST,"stdafx.h"

		.if __TI_EABI__
		.asg Fast_copy21_CPUasm, _Fast_copy21_CPUasm
		.endif

        .global _Fast_copy21_CPUasm
        .sect ".TI.ramfunc"

_Fast_copy21_CPUasm:
		MOVL	XAR0, #_CPU2toCPU1.CLA2toCLA1.Grid

		.asg 	0 , x
		.loop 	$sizeof(Grid_parameters_struct)/2
		MOV32	*(#_CLA2toCLA1.Grid + 2 * x), *XAR0++
		.eval 	x+1, x
		.endloop

		.asg 	0 , x
		.loop 	$sizeof(Grid_parameters_struct)/2
		MOV32	*(#_CLA2toCLA1.Grid_filter + 2 * x), *XAR0++
		.eval 	x+1, x
		.endloop

        LRETR                           ;Return

;end of function Fast_copy21_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
