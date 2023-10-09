//
// formatted console output -- printf, panic.
//

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

volatile int panicked = 0;

// lock to avoid interleaving concurrent printf's.
static struct {
  struct spinlock lock;
  int locking;
} pr;

static char digits[] = "0123456789abcdef";

static int
printint(int xx, int base, int sign)
{
  char buf[16];
  int i;
  int count = 0;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while((x /= base) != 0);

  if(sign) {
    buf[i++] = '-';
  }

  while(--i >= 0)
    count += consputc(buf[i]);

  return count;
}

static int
printptr(uint64 x)
{
  int i;
  int count = 0;
  count += consputc('0');
  count += consputc('x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    count += consputc(digits[x >> (sizeof(uint64) * 8 - 4)]);

  return count;
}

// Print to the console. only understands %d, %x, %p, %s.
int
printf(const char *fmt, ...)
{
  va_list ap;
  int i, c, locking;
  char *s;
  int count = 0;

  locking = pr.locking;
  if(locking)
    acquire(&pr.lock);

  if (fmt == 0)
    panic("null fmt");

  va_start(ap, fmt);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      count += consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      count += printint(va_arg(ap, int), 10, 1);
      break;
    case 'x':
      count += printint(va_arg(ap, int), 16, 1);
      break;
    case 'p':
      count += printptr(va_arg(ap, uint64));
      break;
    case 's':
      if((s = va_arg(ap, char*)) == 0) {
        s = "(null)";
        count += 5;
      }
      for(; *s; s++)
        count += consputc(*s);
      break;
    case '%':
      count += consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      count += consputc('%');
      count += consputc(c);
      break;
    }
  }
  va_end(ap);

  if(locking)
    release(&pr.lock);

  return count;
}

void
panic(char *s)
{
  pr.locking = 0;
  printf("panic: ");
  printf(s);
  printf("\n");
  panicked = 1; // freeze uart output from other CPUs
  for(;;)
    ;
}

void
printfinit(void)
{
  initlock(&pr.lock, "pr");
  pr.locking = 1;
}
