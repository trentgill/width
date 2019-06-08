#pragma once

#include <stm32f3xx.h>

typedef struct{
    GPIO_TypeDef* gpio;
    uint16_t      pin;
    int           state;
} Dout;

Dout* dout_init( char gpio, int pin );
void dout_set( Dout* out, int state );
void dout_flip( Dout* out );
