#include <stm32f7xx.h>
#include "lib/debug_usart.h"

int _write(int file, char *data, int len)
{
#ifndef RELEASE
#ifdef TRACE
    int count = len;
    while(count--){
        fputc((uint32_t)*data++, (FILE*)file);
    }
#else // TRACE using USART to debug
    U_PrintLen(data,len); // enqueues a string
    //U_Print(data); // enqueues a string
#endif // TRACE
#endif // RELEASE
    return len;
}

// use an #ifdef to choose ITM or USART (CLI flag?)
int fputc(int ch, FILE *f)
{
    f = f;
    ITM_SendChar( (uint32_t)ch );
    return ch;
}
