//
// Created by ROG on 2025/7/10.
//
#include "key.h"
#define BTN1_READ()  (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET)
#define BTN2_READ()  (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET)

#define PRESS_CNT     6       // 按下计数门限：10ms
#define LONG_CNT      800      // 长按门限：800ms

typedef struct {
	uint8_t state;
	uint8_t hold;
	uint16_t cnt;
	ButtonEvent_t event;
} Btn_t;

static Btn_t btn1 = {0}, btn2 = {0};
void key_pro(void)
{
	if (btn1.event > 0)
	{
		if (btn1.event == BUTTON_SHORT)
		{

		}
		else{//长按

		}
		btn1.event = BUTTON_IDLE;
	}
	if (btn2.event > 0)
	{
		if (btn2.event == BUTTON_SHORT)
		{

		}
		else{//长按

		}
		btn2.event = BUTTON_IDLE;
	}
}
static void button_proc(Btn_t *btn, uint8_t pressed)
{
	switch (btn->state) {
		case 0: // 空闲态
			if (pressed) {
				btn->state = 1;
				btn->cnt = 0;
			}			break;
		case 1: // 按下态
			if (pressed) {
				btn->cnt++;
				if (btn->cnt >= LONG_CNT) {
					btn->event = BUTTON_LONG;
					btn->state = 2;
				}
			} else {
				if (btn->cnt >= PRESS_CNT && btn->cnt < LONG_CNT) {
					btn->event = BUTTON_SHORT;
				}
				btn->state = 0;
			}
			break;
		case 2: // 长按后保持
			if (!pressed) {
				btn->state = 0;
			}			break;
	}
}

void Button_Scan_10ms(void)
{
	button_proc(&btn1, BTN1_READ());
	button_proc(&btn2, BTN2_READ());
	key_pro();
}

//ButtonEvent_t Button1_Read(void)
//{
//	ButtonEvent_t evt = btn1.event;
//	btn1.event = BUTTON_IDLE;
//	return evt;
//}
//
//ButtonEvent_t Button2_Read(void)
//{
//	ButtonEvent_t evt = btn2.event;
//	btn2.event = BUTTON_IDLE;
//	return evt;
//}

