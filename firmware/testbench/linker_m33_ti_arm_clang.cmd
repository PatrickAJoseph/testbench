-uinterruptVectors
--stack_size=0x1000
--heap_size=0x1000

MEMORY
{
    // AM13E23019
    RAM_S           : ORIGIN = 0x20000000 , LENGTH = 0x18000    // RAM Optimized for data access
    RAM_C           : ORIGIN = 0x00C18000 , LENGTH = 0x8000     // RAM Optimized for code execution. SRAM3 is mapped to C-BUS by default.
    FLASH_BANK0     : ORIGIN = 0x00000000 , LENGTH = 0x40000
    FLASH_BANK1     : ORIGIN = 0x00040000 , LENGTH = 0x40000
    EPI_SDRAM_CODE  : ORIGIN = 0x80000000 , LENGTH = 0x10000  // EPI-mapped SDRAM code execution region (64 KB)
    EPI_SDRAM_DATA  : ORIGIN = 0x80020000 , LENGTH = 0x1000   // EPI-mapped SDRAM uninitialized data region (4 KB)


    // AM13E23018
    // RAM_S       : ORIGIN = 0x20000000 , LENGTH = 0x18000    // RAM Optimized for data access
    // RAM_C       : ORIGIN = 0x00C18000 , LENGTH = 0x8000     // RAM Optimized for code execution. SRAM3 is mapped to C-BUS by default.
    // FLASH_BANK0 : ORIGIN = 0x00000000 , LENGTH = 0x20000
    // FLASH_BANK1 : ORIGIN = 0x00040000 , LENGTH = 0x20000


    // AM13E23017
    // RAM_S       : ORIGIN = 0x20000000 , LENGTH = 0x10000    // RAM Optimized for data access
    // FLASH_BANK0 : ORIGIN = 0x00000000 , LENGTH = 0x10000
    // FLASH_BANK1 : ORIGIN = 0x00040000 , LENGTH = 0x10000


    // NONMAIN Flash
    BCR_CONFIG : origin = 0x60100800, length = 0x00000150
    BSL_CONFIG : origin = 0x60100C00, length = 0x00000050
}

SECTIONS
{

    .intvecs      : > 0, RUN_START(__INT_VECS_START)
    .text         : > FLASH_BANK0 | FLASH_BANK1, palign(8)
    .cinit        : > FLASH_BANK0, palign(8)
    .rodata       : > FLASH_BANK0, palign(8)
    .data         : > RAM_S
    .bss          : > RAM_S
    .sysmem       : > RAM_S
    .stack        : > RAM_S
    .vtable       : > RAM_S
    .binit        : > FLASH_BANK0, palign(8)
    .TI.ramfunc   : load = FLASH_BANK0, palign(8), run=RAM_C, table(BINIT)

    /*
     * .sdram_code — code that runs from EPI-mapped SDRAM.
     * LOAD address is in Flash (copied by memcpy in main after SDRAM init).
     * RUN  address is in EPI_SDRAM_CODE .
     * Linker symbols used by memcpy() in main.c:
     *   sdram_code_loadstart : Flash address of section bytes
     *   sdram_code_loadsize  : Size in bytes
     *   sdram_code_runstart  : SDRAM run address
     *   sdram_code_runsize   : Size at run time
     */
    .sdram_code   : LOAD = FLASH_BANK0, palign(8),
                    RUN  = EPI_SDRAM_CODE,
                    LOAD_START(sdram_code_loadstart),
                    LOAD_SIZE(sdram_code_loadsize),
                    RUN_START(sdram_code_runstart),
                    RUN_SIZE(sdram_code_runsize)

    /*
     * .sdram_data — uninitialized buffer in SDRAM .
     * type = NOLOAD, means any global variables in this regions
     * are not initialized by the C run-time and application needs
     * to initialize in code before using these globals
     */
    .sdram_data   : type = NOLOAD, > EPI_SDRAM_DATA

    /* LLVM code coverage profiling counter sections */
    GROUP :
    {
        __llvm_prf_cnts
        __llvm_prf_bits
    } > RAM_S

    /* Boot Configuration structures in NONMAIN Flash */
    .BCRConfig  : > BCR_CONFIG
    .BSLConfig  : > BSL_CONFIG
}
