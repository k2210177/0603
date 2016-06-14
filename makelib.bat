@ECHO OFF
PATH=..\..\gcc\h8\bin;%PATH%
DEL libadd3052.a
h8300-elf-gcc -Wall -mh -O2 -c sci0.c sci1.c sci0_printf.c sci1_printf.c
@IF ERRORLEVEL 1 GOTO FAIL
h8300-elf-ar -rc libadd3052.a sci0.o sci1.o sci0_printf.o sci1_printf.o
@IF ERRORLEVEL 1 GOTO FAIL
@GOTO SUCCESS
:FAIL
@ECHO ERROR !!!
@GOTO END
:SUCCESS
@ECHO SUCCESS !!!
DEL sci0.o
DEL sci1.o
DEL sci0_printf.o
DEL sci1_printf.o
:END
PAUSE
