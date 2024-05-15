#ifndef _KERNEL_PRINTING_HPP_
#define _KERNEL_PRINTING_HPP_

void kprintString(char const *string);

char* kgetString(char *buf, int max);

int kstringToInt(const char *s);

void kprintInt(int xx, int base=10, int sgn=0);

#endif // _KERNEL_PRINTING_HPP_

