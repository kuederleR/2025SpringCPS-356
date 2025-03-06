#include "types.h"
#include "stat.h"
#include "user.h"

// Type for functions that can "output" a single character either to a 
// file descriptor or a character buffer of at least length characters.
typedef void (*putFunction_t)(int32_t fd, char *outbuffer, uint32_t length, uint32_t index, char c);

// Output c to fd
static void
s_putc(int32_t fd, char *outbuffer, uint32_t length, uint32_t index, char c) {
  write(fd, &c, 1);
}

// store c at index within outbuffer if index is less than length
void s_sputc(int32_t fd, char *outbuffer, uint32_t length, uint32_t index, char c)  {
  if(index < length)
  {
    outbuffer[index] = c;
  }
}

// This function stores a reverse text representation of xx expressed in 
// base [10 or 16] into outbuf. Reversed means that the most significant digit
// is last in the text representation instead of first. It is necessary to
// output the characters in outbuf from last to first to get the normal
// "most significant digit on teh left" representation. At most length
// characters are written to outbuf.
// \return the number of characters written to outbuf
static uint32_t 
s_getReverseDigits(char *outbuf, uint32_t length, int32_t xx, int32_t base, int32_t sgn) {
  static char digits[] = "0123456789ABCDEF";
  int32_t i, neg;
  uint32_t x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  while(i + 1 < length && x != 0) {
    outbuf[i++] = digits[x % base];
    x /= base;
  }

  if(0 == xx && i + 1 < length) {
    outbuf[i++] = digits[0];   
  }

  if(neg && i < length) {
    outbuf[i++] = '-';
  }

  return i;
}

static uint32_t 
s_printint(putFunction_t putcFunction, 
  int32_t fd, char *outbuf, uint32_t length, int32_t xx, int32_t base, int32_t sgn) {
  static const uint32_t localBufferLength = 16; 
  char localBuffer[localBufferLength];

  uint32_t result = 
  s_getReverseDigits(localBuffer, localBufferLength, xx, base, sgn);

  // Reverse the digits to put them in normal print order with most significant
  // to the left
  int32_t j = 0;
  int32_t i = result;
  while(--i >= 0 && j < length) 
  {
    putcFunction(fd, outbuf, length, j, localBuffer[i]);
    j++;
  }
  return result;
}

static uint32_t s_min(uint32_t a, uint32_t b) {
  return (a < b) ? a : b;
}

static 
int32_t s_printf(putFunction_t putcFunction, int32_t fd, char *outbuffer, int32_t n, const char *fmt, uint32_t* ap) {
  char *s;
  int32_t c, i, state;
  uint32_t outindex = 0;
  const int32_t length = n -1; // leave room for nul termination

  state = 0;
  for(i = 0; fmt[i] && outindex < length; i++) {
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%') {
        state = '%';
      } else {
        putcFunction(fd, outbuffer, length, outindex++, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        outindex += s_printint(putcFunction, fd, &outbuffer[outindex], length - outindex, *ap, 10, 1);
        ap++;
      } else if(c == 'x' || c == 'p'){
        outindex += s_printint(putcFunction, fd, &outbuffer[outindex], length - outindex, *ap, 16, 0);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putcFunction(fd, outbuffer, length, outindex++, *s);
          s++;
        }
      } else if(c == 'c'){
        putcFunction(fd, outbuffer, length, outindex++, *ap);
        ap++;
      } else if(c == '%'){
        putcFunction(fd, outbuffer, length, outindex++, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putcFunction(fd, outbuffer, length, outindex++, '%');
        putcFunction(fd, outbuffer, length, outindex++, c);
      }
      state = 0;
    }
  }
  
  return s_min(length, outindex);
}

// Print into outbuffer at most n characters. Only understands %d, %x, %p, %s.
// If n is greater than 0, a terminating nul is always stored in outbuffer.
// \return the number of characters written to outbuffer not counting the
// terminating nul.
int32_t snprintf(char *outbuffer, int32_t n, const char *fmt, ...) {
  uint32_t* ap = (uint32_t*)(void*)&fmt + 1;
  const uint32_t count = s_printf(s_sputc, -1, outbuffer, n, fmt, ap);
  if(count < n) {
    outbuffer[count] = 0; // Assure nul termination
  } 

  return count;
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
printf(int32_t fd, const char *fmt, ...) {
  static const uint32_t veryLarge = 1<<30;
  uint32_t* ap = (uint32_t*)(void*)&fmt + 1;
  s_printf(s_putc, fd, 0, veryLarge, fmt, ap);
}