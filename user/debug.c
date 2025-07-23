//
// Created by ROG on 2025/7/20.
//

#include "debug.h"

/* --------- 可调参数 --------- */
#define UART_TX_BUF_SIZE   128         /* FIFO 总字节数 (2^n 最快) */
#define UART_TX_TMP_LEN     96         /* 单次 sprintf 临时缓冲 */
#define UART_HANDLE         huart1     /* 修改这里可换串口 */
#define UART_DMA_IRQ_PRIO    3         /* 比控制环低，比主循环高 */

/* --------- 全局变量 --------- */
static uint8_t  tx_fifo[UART_TX_BUF_SIZE];
static uint16_t wr_idx = 0;            /* 写指针 */
static uint16_t rd_idx = 0;            /* 读指针 */
static volatile uint8_t dma_busy = 0;  /* 1 = DMA 正在发 */
/* ---------- RX FIFO) ---------- */
#define UART_RX_FIFO_SIZE   64

static uint8_t  rx_fifo[UART_RX_FIFO_SIZE];
static uint8_t  rx_w = 0, rx_r = 0;
static uint8_t  rx_byte;             /* 单字节缓冲 */

static char     line[40];            /* 单行最大 39 字节 */
static uint8_t  line_idx = 0;

void uart1_printf(const char *fmt, ...)
{
	char tmp[UART_TX_TMP_LEN];
	va_list ap;
	va_start(ap, fmt);
	int len = vsnprintf(tmp, sizeof(tmp), fmt, ap); /* 返回不含 '\0' 的字节数 */
	va_end(ap);
	if (len <= 0) return;              /* 格式化失败 */
	/* -------------- 环形 FIFO 写入 -------------- */
	for (int i = 0; i < len; i++) {
		tx_fifo[wr_idx++] = (uint8_t)tmp[i];
		if (wr_idx >= UART_TX_BUF_SIZE) wr_idx = 0;

		/* 简易溢出处理：覆盖旧数据（也可以阻塞等空间） */
		if (wr_idx == rd_idx) {        /* FIFO 满 */
			rd_idx++;                  /* 丢掉最老 1 字节 */
			if (rd_idx >= UART_TX_BUF_SIZE) rd_idx = 0;
		}
	}
	/* -------------- 如 DMA 空闲 → 启动一次 -------------- */
	if (!dma_busy) {
		uint16_t chunk = (wr_idx >= rd_idx) ?           /* 可连续发送的长度 */
		                 (wr_idx - rd_idx) :
		                 (UART_TX_BUF_SIZE - rd_idx);
		dma_busy = 1;
		HAL_UART_Transmit_DMA(&UART_HANDLE,
		                      &tx_fifo[rd_idx],
		                      chunk);
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &UART_HANDLE) {
		/* 写 FIFO，满则覆盖最旧 */
		rx_fifo[rx_w++] = rx_byte;
		if (rx_w >= UART_RX_FIFO_SIZE) rx_w = 0;
		if (rx_w == rx_r) { rx_r++; if (rx_r >= UART_RX_FIFO_SIZE) rx_r = 0; }

		HAL_UART_Receive_IT(&UART_HANDLE, &rx_byte, 1);   /* 继续收 */
	}
}
 void debug_poll(void)
{
	while (rx_r != rx_w) {
		char c = rx_fifo[rx_r++];
		if (rx_r >= UART_RX_FIFO_SIZE) rx_r = 0;

		if (c == '\r' || c == '\n') {  // 行结束
			if (line_idx > 0) {
				line[line_idx] = 0;  // 封尾

				/* 直接解析固定格式 */
				if(sscanf(line, "sp=%f", &PID.speed_kp)  == 1)
					LED1_TOG
				else if (sscanf(line, "si=%f", &PID.speed_ki)  == 1)
					LED1_TOG
				else if (sscanf(line, "tar=%d", &PID.target_speed) == 1)
					LED1_TOG
				else
					uart1_printf("err\r\n");

				line_idx = 0;
			}
		} else if (line_idx < sizeof(line) - 1) {
			line[line_idx++] = c;
		} else {
			line_idx = 0;  // 行太长，丢弃
		}
	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &UART_HANDLE) {
		/* 1. 推进读指针 */
		rd_idx += huart->TxXferSize;
		if (rd_idx >= UART_TX_BUF_SIZE) rd_idx = 0;

		/* 2. FIFO 还有数据？接着发 */
		if (rd_idx != wr_idx) {
			uint16_t chunk = (wr_idx > rd_idx) ?
			                 (wr_idx - rd_idx) :
			                 (UART_TX_BUF_SIZE - rd_idx);
			HAL_UART_Transmit_DMA(&UART_HANDLE,
			                      &tx_fifo[rd_idx],
			                      chunk);
		} else {
			dma_busy = 0;              /* 清忙标志 */
		}
	}
}
