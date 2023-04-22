*********************************************************************************************
Hercules™ ARM® SafetyMCU Bootloader      
v00.30.00                                        
Build Date:  2019-12-03
*********************************************************************************************

---------------------------------------------------------------------------------------------
Introduction
---------------------------------------------------------------------------------------------
This version bootloader suppports LC43x, LS31x, LS12x, LS07x, LS04x, RM42, RM44, RM46, RM48, 
and RM57 families, supports the latest F021 Flash API V2.01.01, and uses the HALCoGen 4.07.01
generated drivers for device and peripherals' initialization. The bootloader and application 
are tested on TI HDKs.

Note: Bootloader uses F021 Flash API which is installed at c:\ti\Hercules\F021 Flash API\02.01.01 
      by default. If you installed F021 Flash API to other location, please change the project 
	  and linker cmd file to use the API in new location.
      
---------------------------------------------------------------------------------------------
New features
---------------------------------------------------------------------------------------------
1. Supports F021 Flash API 2.01.01
2. Generates flash ECC using Linker CMD file to avoid speculative fetch ECC error.
3. TMS570LC43x, RM57Lx, TMS570LS07x and RM44Lx devices are supported
4. Changed APPLICATION START ADDRESS to 0x10020, The 0x10000~0x10020 is left for
   application update status, code rev number(.30.00), and year (2019).
5. Copy the const (used by flash erase/program functions)to SRAM
6. Use HALCoGen driver for initialization
7. Separate flash bank/sector initialization and activiation from flash erase/program 
   functions. It is called only one time.

---------------------------------------------------------------------------------------------
Importing Bootloader Project
---------------------------------------------------------------------------------------------
Notes: please install the new compiler: TI v18.12.1.LTS or later version 

1. Download the zip file: SafetyMCU_Bootloder.zip
2. Extract the files to your workspace, e.g. c:/workspace
3. Within CCS, click "Project" in CCS top-menu
4. Click "Import CCS Project" to open the "Import CCS Eclipse Projects" window
5. Click "Browse" to select the project. For example, c:/workspace/SafetyMCU_Bootloader/
       TMS570LC43x/lc43_can_boot to select lc43_can_boot
5. Click "Finish" of "Import CCS Eclipse Projects" window
   
---------------------------------------------------------------------------------------------
Interrupt vector table: sys_intvecs.asm
---------------------------------------------------------------------------------------------
APPLICATION START ADDRESS is changed to 0x00010020 in bl_config.h. If you like to use the 
exception ISRs defined in APPLICATION, the branch address for SVC/UNDF/PABT/DABT should be 
changed to 0x00010020-0x8.

---------------------------------------------------------------------------------------------
Linker CMD file to generate Flash ECC
---------------------------------------------------------------------------------------------
The Cortex-R4/R5 CPU may generate speculative fetches to any location within the ATCM memory 
space. A speculative fetch to a location with invalid ECC, which is subsequently not used, 
will not create an abort, but will set the ESM flags for a correctable or uncorrectable error. 
An uncorrectable error will unconditionally cause the nERROR pin to toggle low. Therefore care 
must be taken to generate the correct ECC for the entire ATCM space including the holes between 
sections and any unused or blank flash areas. The easiest way to achieve this is to use the 
Linker to generate ECC data rather than the loader.

Changes made to linker cmd file (bl_link.cmd):
1. Add a 'vfill = 0xFFFFFFFF' directive to the end of each line that maps to Flash in the 
   Memory{} section of the command file.
2. Add memory regions corresponding to the ECC area of the flash bank to the Memory{} section
3. Add an ECC{} directive describing the algorithm that matches the device

Changes to CCS Flash Loader Settings:
To use the linker command file to generate ECC for the bootloader, it is necessary to change 
the loader settings so that the loader doesn't also try to generate ECC. Also verification during 
programming needs to be skipped because the data areas and ECC areas will now be programmed in 
separate steps.   

Do following changes under Flash Settings. CCS Project properties --> Debug -->Flash Settings:
1. Check "System Reset on Connect"
2. Uncheck "Auto ECC Generation"
3. Check "Align program segments to 64-bit memory regions"
4. Select "None" for "Flash Verification Settings"
5. Uncheck "Perform Blank Check before Program Load"
6. Press "APPLY" to save your changes!

---------------------------------------------------------------------------------------------
Application Image used for bootloader
---------------------------------------------------------------------------------------------
1. The application image has to be in binary format. to convert the executable (.out) to binary 
format (.bin), the following command can be used as post-build step in CCS project property:
CCS Project Property --> Build -->Steps -->Post-build steps:

"${CCS_INSTALL_ROOT}/utils/tiobj2bin/tiobj2bin.bat" "${BuildArtifactFileName}" 
"${BuildArtifactFileBaseName}.bin" "${CG_TOOL_ROOT}/bin/ofd470" 
"${CG_TOOL_ROOT}/bin/hex470" "${CCS_INSTALL_ROOT}/utils/tiobj2bin/mkhex4bin"


2. The application image will be programmed and executed at APP_START_ADDRESS defined in bl_config.h. 
   The memory map in cmd file of application project has to be modified.
   For example:
 
 /* Memory Map                 */
MEMORY{
    VECTORS (X)  : origin=0x00010020 length=0x00000020  //APP_START_ADDRESS = 0x10020
    FLASH0  (RX) : origin=0x00010040 length=0x0007FFC0
    STACKS  (RW) : origin=0x08000000 length=0x00001500
    RAM     (RW) : origin=0x08001500 length=0x0003EB00
}

3. Linker ECC generation for Application Image
The flash memory is only erasable by the sector or bank. Before progamming the application code to
the flash, the bootloader calls flash API to erase a number of sectors based on the size and star 
address of the application image. Those erased flash sectors are full of ECC errors. After programming 
the application code to those sectors, the unused the flash and the holes will still contain ECC errors.
This may cause the correctable error when the CPU performs speculative pre-fetches. The simple solution
is to fill the sectors with 0xFFFFFFFF, so the content of the unused flash (smaller than 1 sector) will
be part of the application image.  


---------------------------------------------------------------------------------------------
Bootloader Test Tools
---------------------------------------------------------------------------------------------
CAN Bootlaoder:  Visual C project is available. NI USB-8473 adaptor and its driver is 
required for VC project.