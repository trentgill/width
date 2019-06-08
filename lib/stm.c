#include "stm.h"

#include <stm32f3xx_hal.h>

// private declarations
void SystemClock_Config(void);

// public defns
void STM_Init(void)
{
    HAL_Init();
    SystemClock_Config();
}

void STM_println( char** s ){ printf("%s\n", *s); }

void NMI_Handler(void){}
void HardFault_Handler(void){ while (1){} }
void MemManage_Handler(void){ while (1){} }
void BusFault_Handler(void){ while (1){} }
void UsageFault_Handler(void){ while (1){} }
void SVC_Handler(void){}
void DebugMon_Handler(void){}
void PendSV_Handler(void){}
void SysTick_Handler(void){ HAL_IncTick(); }

// private defns
void SystemClock_Config(void)
{
    // HSI Oscillator already ON after system reset, activate PLL with HSI as source
    RCC_OscInitTypeDef osc;
    osc.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    osc.PLL.PLLMUL = RCC_PLL_MUL12;
    if( HAL_RCC_OscConfig(&osc) != HAL_OK ){ while(1); }

    // Select PLL as system clock source
    // configure the HCLK, PCLK1 clocks dividers
    RCC_ClkInitTypeDef clk = { .ClockType = RCC_CLOCKTYPE_SYSCLK
                                          | RCC_CLOCKTYPE_HCLK
                                          | RCC_CLOCKTYPE_PCLK1
                             , .SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK
                             , .AHBCLKDivider  = RCC_SYSCLK_DIV1
                             , .APB1CLKDivider = RCC_HCLK_DIV1
                             };
    if( HAL_RCC_ClockConfig( &clk
                           , FLASH_LATENCY_1
                           ) != HAL_OK ){ while(1); }
}

int _write(int file, char *data, int len)
{
#ifndef RELEASE
    int count = len;
    while(count--){
        fputc((uint32_t)*data++, (FILE*)file);
    }
#endif // RELEASE
    return len;
}

int fputc(int ch, FILE *f)
{
    f = f;
    ITM_SendChar( (uint32_t)ch );
    return ch;
}
