/*----------------------------------------------------------*
   sci0.c for H8/3052F additional library v1.4
                                       Last Edit '03 11/26
   Copyright (c) 1999-2003 BestTechnology CO.,LTD.
 *----------------------------------------------------------*/

#include  "3052.h"

#define rs_init     SCI0_INIT
#define rs_rx_buff  SCI0_IN_DATA_CHECK
#define rs_tx_buff  SCI0_OUT_DATA_CHECK
#define rs_rx_purge SCI0_IN_DATA_CLEAR
#define rs_tx_purge SCI0_OUT_DATA_CLEAR
#define rs_putc     SCI0_OUT_DATA
#define rs_puts     SCI0_OUT_STRING
#define rs_putsb    SCI0_OUT_STRINGB
#define rs_getc     SCI0_IN_DATA
#define rs_gets     SCI0_IN_STRING
#define rs_txi      int_txi0
#define rs_rxi      int_rxi0
#define rs_eri      int_eri0

#define FIFO        sci0fifo
#define SCI         SCI0

typedef struct {
  struct {
    char  *buf;
    short wp;
    short rp;
    short maxlen;
  } tx;
  struct {
    char  *buf;
    short wp;
    short rp;
    short maxlen;
  } rx;
} SCIFIFO;

volatile SCIFIFO FIFO;

// SCI������
void rs_init (TBaudRate b, char *tbuf, short txl, char *rbuf, short rxl)
{
  volatile int i;

  SCI.SCR.BYTE = SCI.SMR.BYTE = 0;

  FIFO.tx.buf = tbuf;
  FIFO.rx.buf = rbuf;
  FIFO.tx.maxlen = txl;
  FIFO.rx.maxlen = rxl;

  // ��M�f�[�^�p�ϐ�������
  FIFO.tx.wp = FIFO.tx.rp = 0;
  FIFO.rx.wp = FIFO.rx.rp = 0;

  SCI.BRR = b;                    // �{�[���[�g
  for (i = 0; i < 3000; i++) ;    // �҂�
  SCI.SCR.BYTE = (b == br115200_EXT) ? 0xf2 : 0xf0;   // ����M���荞�݋���
  SCI.SCR.BYTE = 0xf0;            // ����M����
  SCI.SSR.BYTE;
  SCI.SSR.BYTE = 0;               // �t���O�N���A
}

// ���M�o�b�t�@�`�F�b�N
short rs_tx_buff (void)
{
  short s;
  if ((s = (FIFO.tx.rp - FIFO.tx.wp)) <= 0)
    s += FIFO.tx.maxlen;
  return s - 1;
}

// ��M�o�b�t�@�`�F�b�N
short rs_rx_buff (void)
{
  short s;
  if ((s = (FIFO.rx.wp - FIFO.rx.rp)) < 0) s += FIFO.rx.maxlen;
  return s;
}

// ��M�o�b�t�@�N���A
void rs_rx_purge (void)
{
  SCI.SCR.BIT.RIE = 0;
  FIFO.rx.wp = FIFO.rx.rp = 0;
  SCI.SCR.BIT.RIE = 1;
}

// ���M�o�b�t�@�N���A
void rs_tx_purge (void)
{
  SCI.SCR.BIT.TIE = 0;
  FIFO.tx.wp = FIFO.tx.rp = 0;
}

// 1�������M
void rs_putc (char c)
{
  volatile short t;

  t = (FIFO.tx.wp + 1) % FIFO.tx.maxlen;
  while (t == FIFO.tx.rp) ;
  FIFO.tx.buf[FIFO.tx.wp] = c;
  FIFO.tx.wp = t;
  SCI.SCR.BIT.TIE = 1;
}

// �����񑗐M
void rs_puts (char *st)
{
  while (*st) {
    rs_putc (*st);
    if (*st++ == '\n') rs_putc ('\r');
  }
}

// �o�C�i���񑗐M
void rs_putsb (char *st, short l)
{
  short i;

  for (i = 0; i < l; i++) rs_putc (*st++);
}

// 1������M
char rs_getc (void)
{
  char c;

  while (FIFO.rx.rp == FIFO.rx.wp) ;
  c = FIFO.rx.buf[FIFO.rx.rp++];
  FIFO.rx.rp %= FIFO.rx.maxlen;
  return c;
}

// �������M
short rs_gets (char *s, short len)
{
  short i = 0;
  char c;
  while (1) {
    c = rs_getc ();
    if ((c == '\n') || (c == '\r')) {
      s[i] = '\0'; if (i != 0) i++;
      rs_putc ('\n');
      break;
    } else if ((i > 0) && (c == '\b')) {
      rs_puts ("\b \b");
      i--;
    } else if ((i < len) && (i >= 0) && (c >= ' ') && (c <= '~')) {
      s[i++] = (char)c;
      rs_putc (c);
    } else rs_putc ('\a');
  }
  return i;
}

// ��M���荞��
void rs_rxi (void)
{
  volatile short t;
  _BYTE c = SCI.RDR;

  SCI.SSR.BIT.RDRF = 0;
  t = (FIFO.rx.wp + 1) % FIFO.rx.maxlen;
  FIFO.rx.buf[FIFO.rx.wp] = c;
  if (t != FIFO.rx.rp) FIFO.rx.wp = t;
}

// ���M���荞��
void rs_txi (void)
{
  if (FIFO.tx.wp != FIFO.tx.rp) {
    SCI.TDR = FIFO.tx.buf[FIFO.tx.rp++];
    FIFO.tx.rp %= FIFO.tx.maxlen;
    SCI.SSR.BIT.TDRE = 0;
  } else
    SCI.SCR.BIT.TIE = 0;
}

// ��M�G���[���荞��
void rs_eri (void)
{
  SCI.SSR.BYTE &= ~0x38;
}
