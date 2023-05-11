;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

; C prototype:
; void Fast_copy21_CPUasm();
; x instructions without pre-jump

        .cdecls   C,LIST,"stdafx.h"

		.if __TI_EABI__
		.asg Fast_copy21_CPUasm, _Fast_copy21_CPUasm
		.endif

        .global _Fast_copy21_CPUasm
        .text

_Fast_copy21_CPUasm:
		MOVL	XAR0, #_CPU2toCPU1

		.asg 	0 , x
		.loop 	24
		MOV32	*XAR0++, *(#_Grid.input_P_p + 2 * x)
		.eval 	x+1, x
		.endloop

		.asg 	0 , x
		.loop 	$sizeof(Grid_parameters_struct)/2
		MOV32	*XAR0++, *(#_Grid.parameters + 2 * x)
		.eval 	x+1, x
		.endloop

		.asg 	0 , x
		.loop 	$sizeof(Grid_parameters_struct)/2
		MOV32	*XAR0++, *(#_Grid_filter.parameters + 2 * x)
		.eval 	x+1, x
		.endloop

        LRETR                           ;Return

;end of function Fast_copy21_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
