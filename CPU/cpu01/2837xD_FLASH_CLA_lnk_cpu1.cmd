#define _BOOTLOADER
#define CLA_SCRATCHPAD_SIZE 0x100
--undef_sym=__cla_scratchpad_end
--undef_sym=__cla_scratchpad_start

MEMORY
{
PAGE 0 :
   RAMM0_1           	: origin = 0x000122,   length = 0x0006DE
   //RAMM1            : origin = 0x000400,   length = 0x000400
   RAMD0_1           	: origin = 0x00B000,   length = 0x001000
   //RAMD1           	: origin = 0x00B800,   length = 0x000800

   RAMLS0_2          	: origin = 0x008000,   length = 0x001800
   //RAMLS1          	: origin = 0x008800,   length = 0x000800
   //RAMLS2      		: origin = 0x009000,   length = 0x000800
   RAMLS3_5      		: origin = 0x009800,   length = 0x001800
   //RAMLS4      	    : origin = 0x00A000,   length = 0x000800
   //RAMLS5           : origin = 0x00A800,   length = 0x000800

   RAMGS0_3           : origin = 0x00C000,   length = 0x004000
   RAMGS4_7           : origin = 0x010000,   length = 0x004000
   /*
   RAMGS1           : origin = 0x00D000,   length = 0x001000
   RAMGS2           : origin = 0x00E000,   length = 0x001000
   RAMGS3           : origin = 0x00F000,   length = 0x001000
   RAMGS4           : origin = 0x010000,   length = 0x001000
   RAMGS5           : origin = 0x011000,   length = 0x001000
   RAMGS6           : origin = 0x012000,   length = 0x001000
   RAMGS7  			: origin = 0x013000,   length = 0x001000
   	*/
   RAMGS8_15  			: origin = 0x014000,   length = 0x008000
   /*
   RAMGS8  			: origin = 0x014000,   length = 0x001000
   RAMGS9  			: origin = 0x015000,   length = 0x001000
   RAMGS10  		: origin = 0x016000,   length = 0x001000
   RAMGS11	 		: origin = 0x017000,   length = 0x001000
   RAMGS12  		: origin = 0x018000,   length = 0x001000
   RAMGS13	 		: origin = 0x019000,   length = 0x001000
   RAMGS14  		: origin = 0x01A000,   length = 0x001000
   RAMGS15	 		: origin = 0x01B000,   length = 0x001000
	*/

   /* Flash sectors */
	#ifdef _BOOTLOADER
   BEGIN		    : origin = 0x086000,   length = 0x000002
   //FLASHC           : origin = 0x084002,   length = 0x001FFE
	#else
   BEGIN     		: origin = 0x080000,   length = 0x000002
   FLASHA           : origin = 0x080002,   length = 0x001FFE
   FLASHB           : origin = 0x082000,   length = 0x002000
   FLASHC           : origin = 0x084000,   length = 0x002000
	#endif
   FLASHD           : origin = 0x086002,   length = 0x001FFE
   FLASHE           : origin = 0x088000,   length = 0x008000
   FLASHF           : origin = 0x090000,   length = 0x008000
   FLASHG           : origin = 0x098000,   length = 0x008000
   FLASHH           : origin = 0x0A0000,   length = 0x008000
   FLASHI           : origin = 0x0A8000,   length = 0x008000
   FLASHJ           : origin = 0x0B0000,   length = 0x008000
   FLASHK           : origin = 0x0B8000,   length = 0x002000
   FLASHL           : origin = 0x0BA000,   length = 0x002000
   FLASHM           : origin = 0x0BC000,   length = 0x002000
   FLASHN           : origin = 0x0BE000,   length = 0x002000

   EMIF1_CS0n       : origin = 0x80000000, length = 0x10000000
   EMIF1_CS2n       : origin = 0x00100000, length = 0x00200000
   EMIF1_CS3n       : origin = 0x00300000, length = 0x00080000
   EMIF1_CS4n       : origin = 0x00380000, length = 0x00060000
   EMIF2_CS0n       : origin = 0x90000000, length = 0x10000000
   EMIF2_CS2n       : origin = 0x00002000, length = 0x00001000

   CLA1_MSGRAMLOW   : origin = 0x001480,   length = 0x000080
   CLA1_MSGRAMHIGH  : origin = 0x001500,   length = 0x000080

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400

   RESET           : origin = 0x3FFFC0, length = 0x000002
}


SECTIONS
{

   codestart: 	>  BEGIN: {_start_code=.;}
   /* Allocate program areas: */
   .binit:   	>  FLASHI
   .cinit:   	>  FLASHI
   .pinit:   	>  FLASHI,   PAGE = 0
   .text:    	>>  FLASHE |FLASHF|FLASHG
   .TI.ramfunc:      LOAD = FLASHI
                     RUN = RAMD0_1,
                     table(BINIT)*/

   .TI.ramfunc_unsecure: LOAD = FLASHD,   PAGE = 0
                     RUN  = RAMGS8_15,   PAGE = 0,
                     table(BINIT)

   /* Initalized sections go in Flash */
   .switch:  	>  FLASHI,   PAGE = 0
   .econst:  	>  FLASHI

   .files_ro:   >  FLASHJ

   /* Allocate uninitalized data sections: */

   .stack: 		>  RAMGS0_3,   PAGE = 0

   .ebss:		>>  RAMM0_1 | RAMGS0_3 | RAMGS4_7,   PAGE = 0
   .esysmem: 	>  RAMM0_1 | RAMGS0_3 | RAMGS4_7,   PAGE = 0
   .cio: 		>>  RAMM0_1 | RAMGS0_3 | RAMGS4_7,   PAGE = 0

   Grid: 		>  RAMGS4_7,   PAGE = 0
   Scope: 		>  RAMGS8_15,   PAGE = 0
   EMIF_mem: 	>  EMIF1_CS2n, PAGE = 0
   CPUTOCLA: 	>  CLA1_MSGRAMHIGH, PAGE = 0

   RAM_16BIT_ADDR: >  RAMLS3_5,   PAGE = 0

///////////////////////////////////////////////////////////////////////////////

    /* CLA specific sections */
   CLAData:  	>  RAMLS3_5, PAGE = 0

   Cla1Prog: 	   LOAD = FLASHH, PAGE = 0
                   RUN  = RAMLS0_2, PAGE = 0
                   table(BINIT)

   .const_cla: 	   LOAD = FLASHI, PAGE = 0
                   RUN  = RAMLS3_5, PAGE = 0
                   table(BINIT)

   .scratchpad: >  RAMLS3_5,   PAGE = 0
   .bss_cla:    >  RAMLS3_5,   PAGE = 0

///////////////////////////////////////////////////////////////////////////////
   .reset: 		>  RESET,	  PAGE = 0, TYPE = DSECT

    /* The following section definitions are required when using the IPC API Drivers */
    GROUP : > CPU1TOCPU2RAM, PAGE = 0
    {
    	CPU1TOCPU2
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU2TOCPU1RAM, PAGE = 0
    {
    	CPU2TOCPU1
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
