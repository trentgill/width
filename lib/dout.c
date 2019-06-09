#include "dout.h"

#include <stm32f7xx.h>
#include <stdio.h>
#include <stdlib.h>

// private declarations
static inline void dout_write( Dout* o );

// public defns
Dout* dout_init( char gpio, int pin )
{
    Dout* d = malloc( sizeof( Dout ));
    if( !d ){ printf("dout malloc!\n"); return NULL; }

    switch(gpio){
        case 'a': case 'A': __HAL_RCC_GPIOA_CLK_ENABLE(); d->gpio = GPIOA; break;
        case 'b': case 'B': __HAL_RCC_GPIOB_CLK_ENABLE(); d->gpio = GPIOB; break;
        case 'c': case 'C': __HAL_RCC_GPIOC_CLK_ENABLE(); d->gpio = GPIOC; break;
        case 'd': case 'D': __HAL_RCC_GPIOD_CLK_ENABLE(); d->gpio = GPIOD; break;
        case 'e': case 'E': __HAL_RCC_GPIOE_CLK_ENABLE(); d->gpio = GPIOE; break;
        case 'f': case 'F': __HAL_RCC_GPIOF_CLK_ENABLE(); d->gpio = GPIOF; break;
        case 'g': case 'G': __HAL_RCC_GPIOG_CLK_ENABLE(); d->gpio = GPIOG; break;
        default: printf("dout pin not found\n"); return NULL;
    }
    d->pin   = 1 << pin;
    d->state = 0;

    GPIO_InitTypeDef g = { .Mode = GPIO_MODE_OUTPUT_PP
                         , .Pull = GPIO_NOPULL
                         , .Pin  = d->pin
                         };
    HAL_GPIO_Init(d->gpio, &g);

    return d;
}

void dout_set( Dout* out, int state )
{
    out->state = (state) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    dout_write( out );
}

void dout_flip( Dout* out )
{
    out->state ^= 1;
    dout_write( out );
}

// private defns
static inline void dout_write( Dout* o ){
    HAL_GPIO_WritePin( o->gpio
                     , o->pin
                     , o->state
                     );
}
