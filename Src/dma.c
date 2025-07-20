#include "dma.h"

void MX_DMA_Init(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}
