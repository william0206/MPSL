#include "stm32l476xx.h"
#include "utils.h"

int flag[4] = {0};
int money = 0;
float time = 660000;
float duty = 1;

void IRQ_Init(){
	//PC8-11
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[2] &= 0x0000;
	SYSCFG->EXTICR[2] |= 0x2222;

	EXTI->RTSR1 |=(15<<8);
	EXTI->FTSR1 &=~(15<<8);

	EXTI->PR1|=(15<<8);
	EXTI->IMR1|=(15<<8);

	//NVIC_SetPriority(EXTI15_10_IRQn,5);
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);

}
void EXTI9_5_IRQHandler(void){ //user button

	if((GPIOC->IDR&(1<<8)) && flag[0] == 0){	//can add money
		money += 1;
		flag[0] = 1;
	}
	else if((GPIOC->IDR&(1<<9)) && flag[1] == 0){
		money += 4;
		flag[1] = 1;
	}
	EXTI->PR1|=(1<<8);
	EXTI->PR1|=(1<<9);
}
void EXTI15_10_IRQHandler(void){
	if((GPIOC->IDR&(1<<10)) && flag[2] == 0){
		money += 5;
		flag[2] = 1;
	}
	else if((GPIOC->IDR&(1<<11)) && flag[3] == 0){
		money += 40;
		flag[3] = 1;
	}
	EXTI->PR1|=(1<<10);
	EXTI->PR1|=(1<<11);
}
int main()
{
	A:
	money = 0;
	gpio_init();
	max7219_init();

	keypad_init();
	fpu_enable();
	PB_timer_init();
	display(0, 1);

	ray_init();

	IRQ_Init();
	coin:
	while(1){
		if(money > 50) money = 50;
		display(money, cal_len(money));
		if(keypad_scan() != -1) break;
		if(flag[0]||flag[1]||flag[2]||flag[3]){	//delay
			int i = 0;
			for(i = 0; i < 20000; i++){
				if(keypad_scan() != -1) break;
				//do nothing
			}
			for(i = 0; i< 4; i++){
				flag[i] = 0;
			}
		}
	}

	int choose = keypad_scan();
	int rem = 0;
	if(choose == 1){
		if(money < 5) {
			display(87, 2);
			goto coin;
		}
		rem = money - 5;
		display(rem, cal_len(rem));
	}
	else if(choose == 2){
		if(money < 20) {
			display(87, 2);
			goto coin;
		}
		rem = money - 20;
		display(rem, cal_len(rem));
	}
	else if(choose == 3){
		if(money < 30) {
			display(87, 2);
			goto coin;
		}
		rem = money - 30;
		display(rem, cal_len(rem));
	}

	if(choose == 1){
		give_1st(duty, time*11/10);	//first candy, spin is in "utils.h" pb4
	}

	else if(choose == 2){		//second candy
		give_2nd(duty, time*13/10);	//pb5
	}
	else if(choose == 3){
		give_3rd(duty, time*125/100);	//pb6
	}

	while(rem >= 10){
		rem -= 10;
		back_10(duty, time);	//give 10 dollars back //pb7
		display(rem, cal_len(rem));
	}
	while(rem >= 5){
		rem -= 5;
		back_5(duty, time);	//pb1
		display(rem, cal_len(rem));
	}
	while(rem >= 1){
		rem -= 1;
		back_1(duty, time);	//pb9
		display(rem, cal_len(rem));
	}

	goto A;
	//timer_init_pb3(timer);		//for pb3 tim2 ch2

	//timer_init_pb4(timer);		// for pb4 tim3 ch1

	//timer_init_pb5(timer);		// for pb5 tim3 ch2

	//timer_init_pb6(timer);		// for pb6 tim4 ch1

	//timer_init_pb7(timer);		// for pb7 tim4 ch2

	//timer_init_pb8(timer);		// for pb8 tim4 ch3

	//timer_init_pb9(timer);		// for pb9 tim4 ch4

		// ** for pb3 tim2 ch2
		//timer->CCR2 = duty;
		//***********************************************

		// ** for pb4 tim3 ch1
		//timer->CCR1 = duty;
		//***********************************************

		// ** for pb5 tim3 ch2
		//timer->CCR2 = duty;
		//*********************************************

		// ** for pb6 tim4 ch1
		//timer->CCR1 = duty;
		//**********************************************

		// ** for pb7 tim4 ch2
		//timer->CCR2 = duty;
		//*********************************************

		// ** for pb8 tim4 ch3
		//timer->CCR3 = duty;
		//*********************************************

		// ** for pb9 tim4 ch3
		//timer->CCR4 = duty;
		//*********************************************

		//timer->CR1 |= TIM_CR1_CEN;






}
