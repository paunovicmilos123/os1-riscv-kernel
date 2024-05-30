#include "../h/kernelPrinting.hpp"
#include "../h/kConsole.hpp"

void kprintString(char const *string)
{
    while (*string != '\0')
    {
        kConsole::putc(*string);
        string++;
    }
}

char hexdigits[] = "0123456789ABCDEF";

void kprintInt(int xx, int base, int sgn)
{
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if(sgn && xx < 0){
        neg = 1;
        x = -xx;
    } else {
        x = xx;
    }

    i = 0;
    do{
        buf[i++] = hexdigits[x % base];
    }while((x /= base) != 0);
    if(neg)
        buf[i++] = '-';

    while(--i >= 0)
        kConsole::putc(buf[i]);
}