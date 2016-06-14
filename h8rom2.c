#include <3052.h>

void H8_BUS_INIT (void) asm ("_H8_BUS_INIT");

void H8_BUS_INIT (void)
{
  //--------------------------------------
  // 外部RAMバス設定
  //--------------------------------------
  P1.DDR = 0xff;         // A0-A7
  P2.DDR = 0xff;         // A8-A15
  P5.DDR = 0xf1;         // A16
//  BSC.ASTCR.BYTE = 0x00; // 外部RAM アクセスステート
}

__asm__ (
 "        .h8300h\n"
 "        .section .text\n"
 "        .global _start\n"
 "        .global _exit\n"
 "\n"
 "        .extern _H8_BUS_INIT\n"
 "        .extern _main\n"
 "        .extern _data\n"
 "        .extern _mdata\n"
 "        .extern _edata\n"
 "        .extern _bss\n"
 "        .extern _ebss\n"
 "        .extern _stack\n"
 "\n"
 "_start:\n"
 "        mov.l   #_stack,sp\n"
 "        jsr     @_H8_BUS_INIT\n"
 "        mov.l   #_stack,sp\n"
 "\n"
 "        ;; copy DATA from ROM\n"
 "        mov.l   #_mdata,er0\n"
 "        mov.l   #_data,er1\n"
 "        mov.l   #_edata,er2\n"
 "data_init_loop:\n"
 "        cmp     er2,er1\n"
 "        bge     fin_data_init\n"
 "        mov.b   @er0+,r3l\n"
 "        mov.b   r3l,@er1\n"
 "        adds    #1,er1\n"
 "        bra     data_init_loop\n"
 "fin_data_init:\n"
 "\n"
 "        ;; clear BSS\n"
 "        mov.l   #_bss,er0\n"
 "        mov.l   #_ebss,er1\n"
 "        mov.b   #0,r2l\n"
 "bss_init_loop:\n"
 "        cmp     er1,er0\n"
 "        bge     fin_bss_init\n"
 "        mov.b   r2l,@er0\n"
 "        adds    #1,er0\n"
 "        bra     bss_init_loop\n"
 "fin_bss_init:\n"
 "\n"
 "        jsr     @_main\n"
 "\n"
 "_exit:\n"
 "        sleep\n"
 "        bra     _exit\n"
);
