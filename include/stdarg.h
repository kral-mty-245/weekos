#ifndef WEEKOS_STDARG_H
#define WEEKOS_STDARG_H

typedef char* va_list;

#define va_start(ap, last) (ap = (char*)&last + sizeof(last))
#define va_end(ap) (ap = (char*)0)
#define va_arg(ap, type) (*(type*)((ap += sizeof(type)) - sizeof(type)))

#endif
