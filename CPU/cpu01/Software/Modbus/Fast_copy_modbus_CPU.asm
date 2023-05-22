;Tomasz Œwiêchowicz swiechowicz.tomasz@gmail.com

; C prototype:
; void Fast_copy_modbus_CPUasm();
; x instructions without pre-jump

        .cdecls   CPP,LIST,"Modbus_Converter_memory.h"

		.if __TI_EABI__
		.asg Fast_copy_modbus_CPUasm, _Fast_copy_modbus_CPUasm
		.endif

        .global _Fast_copy_modbus_CPUasm
        .text

_Fast_copy_modbus_CPUasm:
		MOVL	XAR0, #_Modbus_Converter.input_registers.Grid_filter
		.asg 	0 , x
		.loop 	$sizeof(Grid_parameters_struct)/2
		MOV32	*XAR0++, *(#_Grid_filter.parameters + 2 * x)
		.eval 	x+1, x
		.endloop

		MOVW    DP, #_Conv.f_filter
		MOV32	*(#_Modbus_Converter.input_registers.frequency), @_Conv.f_filter


		.asg 	0 , y
		.loop 	3
		MOVL	XAR0, #(_Kalman_I_grid + Kalman_struct.rms_values + y*$sizeof(Kalman_struct))

		.asg 	0 , x
		.loop 	KALMAN_HARMONICS
		MOV32	*(#_Modbus_Converter.input_registers.harmonic_rms_values.I_grid_a + 2*(x + KALMAN_HARMONICS*y)), *XAR0++
		.eval 	x+1, x
		.endloop
		.asg 	0 , x
		.loop 	KALMAN_HARMONICS
		MOV32	*(#_Modbus_Converter.input_registers.harmonic_THD_individual.I_grid_a + 2*(x + KALMAN_HARMONICS*y)), *XAR0++
		.eval 	x+1, x
		.endloop

		.eval 	y+1, y
		.endloop

		.asg 	0 , y
		.loop 	3
		MOVL	XAR0, #(_Kalman_U_grid + Kalman_struct.rms_values + y*$sizeof(Kalman_struct))

		.asg 	0 , x
		.loop 	KALMAN_HARMONICS
		MOV32	*(#_Modbus_Converter.input_registers.harmonic_rms_values.U_grid_a + 2*(x + KALMAN_HARMONICS*y)), *XAR0++
		.eval 	x+1, x
		.endloop
		.asg 	0 , x
		.loop 	KALMAN_HARMONICS
		MOV32	*(#_Modbus_Converter.input_registers.harmonic_THD_individual.U_grid_a + 2*(x + KALMAN_HARMONICS*y)), *XAR0++
		.eval 	x+1, x
		.endloop

		.eval 	y+1, y
		.endloop

        LRETR                           ;Return

;end of function Fast_copy_modbus_CPUasm()
;*********************************************************************

       .end
;;#############################################################################
;;  End of File
;;#############################################################################
