/*----------------------------------------------------------*
   sci1_printf.c for H8/3052F additional library v1.4
                                       Last Edit '03 11/26
   Copyright (c) 1999-2003 BestTechnology CO.,LTD.
 *----------------------------------------------------------*/

#include  "3052.h"
#include  <stdlib.h>
#include  <string.h>
#include  <stdarg.h>

#define rs_puts   SCI1_OUT_STRING
#define rs_putc   SCI1_OUT_DATA
#define rs_printf SCI1_PRINTF

// 簡易フォーマット出力
short rs_printf (const char *format, ...)
{
  short         i = 0;
  char          scratch[16], f, *ptr;
  unsigned long val = 0, base;
  char          ch, z, n, l, c, m;
  va_list       ap;

  va_start (ap, format);
  for (;;) {
    while ((f = *format++) != '%') {
      if (!f) {
        va_end (ap);
        return i;
      } else {
        rs_putc (f);
        if (f == '\n') rs_putc ('\r');
        i++;
      }
    }
    c = 0;
    // 0で埋める？
    if (!(z = (f = *format++) == '0')) format--;
    // 桁数指定あり？
    f = *format++;
    if (f >= '1' && f <= '9') n = f - '0'; else {n = 0; format--;}
    // 32bit？
    f = *format++;
    l = (f == 'l');
    if (!l) format--;
    m = 0;

    switch (f = *format++) {
      case 'c':
        f = va_arg (ap, int);
        i++;
      default:
        rs_putc (f);
        continue;
      case 's':
        ptr = va_arg (ap, char *);
        rs_puts (ptr);
        i += strlen(ptr);
        continue;
      case 'o':
        val = (long)(l ? va_arg (ap, long) : va_arg (ap, int));
        base = 8L;
        goto CONV_LOOP;
      case 'd':
        if (l) {
          val = va_arg (ap, long);
          if (((long)val) < 0L) {
            m = 1;
            val = - val;
          }
        } else {
          val = va_arg (ap, int);
          if ((short)val < 0) {
            m = 1;
            val = - val;
          }
        }
        base = 10L;
        goto CONV_LOOP;
      case 'b':
        val = (long)(l ? va_arg (ap, long) : va_arg (ap, int));
        base = 2L;
        goto CONV_LOOP;
      case 'u':
        val = (long)(l ? va_arg (ap, long) : va_arg (ap, int));
        base = 10L;
        goto CONV_LOOP;
      case 'X':
        c = 1;
      case 'x':
        base = 16L;
        val = (long)(l ? va_arg (ap, long) : va_arg (ap, int));

      CONV_LOOP:
        for (i = 0; i < 16; i++) scratch[i] = z ? '0' : ' ';
        ptr = scratch + 16;
        *--ptr = 0;
        do {
          ch = (char)(val % base) + '0';
          if (ch > '9') ch += (c ? 'A' : 'a')  - '9' - 1;
          *--ptr = ch;
          val /= base;
        } while (val);
        if (m) *--ptr = '-';
        if (n) ptr = scratch + 15 - n;
        rs_puts (ptr);
        i += strlen(ptr);
    }
  }
}
