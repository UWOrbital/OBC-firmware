/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */

MEMORY
{
    VECTORS    (X)   : origin=0x00000000 length=0x00000020      vfill = 0xffffffff
    FLASH0     (RX)  : origin=0x00000020 length=0x00140000-0x20 vfill = 0xffffffff
    SRAM       (RW)  : origin=0x08002000 length=0x0002D000
    STACK      (RW)  : origin=0x08000000 length=0x00002000
    ECC_VEC  (R) : origin=(0xf0400000 + (start(VECTORS) >> 3))
                   length=(size(VECTORS) >> 3)
                   ECC={algorithm=algoL2R4F021, input_range=VECTORS}

    ECC_FLA0 (R) : origin=(0xf0400000 + (start(FLASH0)  >> 3))
                   length=(size(FLASH0)  >> 3)
                   ECC={algorithm=algoL2R4F021, input_range=FLASH0 }
}

ECC
{
    algoL2R4F021 : address_mask = 0xfffffff8 /* Address Bits 31:3 */
                   hamming_mask = R4         /* Use R4/R5 build in Mask */
                   parity_mask  = 0x0c       /* Set which ECC bits are Even and Odd parity */
                   mirroring    = F021       /* RM57Lx and TMS570LCx are build in F021 */
}

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
  .intvecs : {} > VECTORS

  flashAPI:
  {
    .\source\Fapi_UserDefinedFunctions.obj (.text)
    .\source\bl_flash.obj (.text)
    --library= "c:\ti\Hercules\F021 Flash API\02.01.01\F021_API_CortexR4_LE.lib" (.text)
  } palign=8 load = FLASH0, run = SRAM, LOAD_START(apiLoadStart), RUN_START(apiRunStart), SIZE(apiLoadSize)

  .text  : {} > FLASH0 /*Initialized executable code and constants*/
  .const : {} palign=8 load=FLASH0, run = SRAM, LOAD_START(constLoadStart), RUN_START(constRunStart), SIZE(constLoadSize)

  .cinit : {} > FLASH0 /*Initialized global and static variables*/
  .pinit : {} > FLASH0
  .data  : {} > SRAM
  .bss   : {} > SRAM   /*Uninitialized Global and static variables */
  .sysmem  : {} > SRAM
  KEEP(*(.intvecs))
}
