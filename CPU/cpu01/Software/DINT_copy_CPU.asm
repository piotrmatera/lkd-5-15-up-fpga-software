;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

; C prototype:
; void DINT_copy_CPUasm(Uint16 *dst, Uint16 *src, Uint16 size);
; x instructions without pre-jump

        .cdecls   C,LIST,"stdafx.h"

		.if __TI_EABI__
		.asg DINT_copy_CPUasm, _DINT_copy_CPUasm
		.endif

        .global _DINT_copy_CPUasm
        .sect ".TI.ramfunc"

_DINT_copy_CPUasm:
		DEC		AL
		MOVL	XAR7, XAR5
		RPT 	AL
		||PREAD *XAR4++,*XAR7
        LRETR                           ;Return

;end of function DINT_copy_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
