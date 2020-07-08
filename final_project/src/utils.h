#ifndef UTILS_H_
#define UTILS_H_

// these functions are inside the asm file

extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);
extern void Delay1sWith4MHz();
extern void fpu_enable();

/**
 * show data on 7-seg via max7219_send
 *
 * input:
 *   data: decimal value
 *   num_digs: number of digits to show on 7-seg
 *
 * return:
 *   0: success
 *   -1: illegal data range (out of 8 digits)
 */
int display(int data, int num_digs){
	int i;
	if(data == -1 || num_digs == 0){
		for(i = 1; i <= 8; i ++){
				max7219_send(i, 15);
			}
		return 0;
	}

	for(i = num_digs+1; i <= 8; i ++){
		max7219_send(i, 15);
	}
	int neg = 0;
	int over = 0;
	if(data < 0){
		neg = 1;
	}
	if(data > 99999999 || data < -9999999){
		over = 1;
	}
	for (i = 1; i <= num_digs; i++){
		if (data % 10 < 0){
			max7219_send(i, -data % 10);
		}
		else{
			max7219_send(i, data % 10);
		}
		data /= 10;
	}
	if(neg == 1){
		max7219_send(num_digs, 10);	//means '-'
	}
	if(over == 1){
		for(i = 3; i <= 8; i++){
			max7219_send(i, 15);
		}
		max7219_send(2, 10);
		max7219_send(1, 1);
		return -1;
	}

	return 0;
}

#define GPIO_Pin_0  0b0000000000000001
#define GPIO_Pin_1  0b0000000000000010
#define GPIO_Pin_2  0b0000000000000100
#define GPIO_Pin_3  0b0000000000001000
#define GPIO_Pin_4  0b0000000000010000
#define GPIO_Pin_5  0b0000000000100000
#define GPIO_Pin_6  0b0000000001000000
#define GPIO_Pin_7  0b0000000010000000
#define GPIO_Pin_8  0b0000000100000000
#define GPIO_Pin_9  0b0000001000000000
#define GPIO_Pin_10 0b0000010000000000
#define GPIO_Pin_11 0b0000100000000000
#define GPIO_Pin_12 0b0001000000000000
#define GPIO_Pin_13 0b0010000000000000
#define GPIO_Pin_14 0b0100000000000000
#define GPIO_Pin_15 0b1000000000000000

int GPIO_ReadInputDataBit(GPIO_TypeDef *a, uint16_t b) {
	return a->IDR & b;
}

void PB_timer_init()
{
	RCC->AHB2ENR   |= 0b00000000000000000000000000000111;

	GPIOB->MODER   &= 0b11111111111010101010101010101010;
	//GPIOB->MODER &= 0;
	GPIOB->PUPDR   &= 0b11111111111111111111111111111111;
	GPIOB->PUPDR   |= 0b00000000000000000000000000000000;
	GPIOB->OSPEEDR &= 0b11111111111111111111111111111111;
	GPIOB->OSPEEDR |= 0b00000000000000000000000000000000;

}
void timer_init_pb0(TIM_TypeDef *timer)
{
	RCC->AHB2ENR |= 0x1 << 1;	/* enable AHB2 clock for port B */
	GPIOB->MODER |= GPIO_MODER_MODE0_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL0_1;	/* PB0: AF2 (TIM3_CH3) */
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

	timer->ARR = (uint32_t) 99;
	timer->CR1 |= TIM_CR1_DIR;
	//CH3
	timer->CCMR2 &= 0xFFFFFCFF;
	// select compare 2 (channel 2 is configured as output)
	timer->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1);
	// set output compare 2 mode to PWM mode 1
	timer->CCMR2 |= TIM_CCMR2_OC3PE;
	timer->CR1 |= TIM_CR1_ARPE;
	timer->CCER |= TIM_CCER_CC3E;
		// enable compare 2 output
	timer->EGR = TIM_EGR_UG;
	// re-initialize the counter and generates an update of the registers
}

void timer_init_pb1(TIM_TypeDef *timer)
{
	RCC->AHB2ENR |= 0x1 << 1;	/* enable AHB2 clock for port B */
	GPIOB->MODER |= GPIO_MODER_MODE1_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL1_1;	/* PB1: AF2 (TIM3_CH4) */
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	timer->ARR = (uint32_t) 99;

	timer->CR1 |= TIM_CR1_DIR;	// counter used as downcounter

	timer->CCMR2 &= 0xFFFFFCFF;
	// select compare 2 (channel 2 is configured as output)
	timer->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1);
	// set output compare 2 mode to PWM mode 1
	timer->CCMR2 |= TIM_CCMR2_OC4PE;
	// enable output compare 2 preload register on TIM2_CCR2
	timer->CR1 |= TIM_CR1_ARPE;
	timer->CCER |= TIM_CCER_CC4E;
		// enable compare 2 output
	timer->EGR = TIM_EGR_UG;
	// re-initialize the counter and generates an update of the registers

}

void timer_init_pb3(TIM_TypeDef *timer)
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	// enable TIM2 timer clock
	GPIOB->MODER |= GPIO_MODER_MODE3_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL3_0;
	// select AF1 for PB3 (PB3 is TIM2_CH2)
	timer->CR1 |= TIM_CR1_DIR;
	// counter used as downcounter
	timer->CR1 |= TIM_CR1_ARPE;
	// enable auto-reload preload (buffer TIM2_ARR)
	timer->ARR = (uint32_t) 99;
	// auto-reload prescaler value
	timer->CCMR1 &= 0xFFFFFCFF;
	// select compare 2 (channel 2 is configured as output)
	timer->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);
	// set output compare 2 mode to PWM mode 1
	timer->CCMR1 |= TIM_CCMR1_OC2PE;
	// enable output compare 2 preload register on TIM2_CCR2
	timer->CCER |= TIM_CCER_CC2E;
	// enable compare 2 output
	timer->EGR = TIM_EGR_UG;
	// re-initialize the counter and generates an update of the registers
}

void timer_init_pb4 (TIM_TypeDef *timer)
{
	RCC->AHB2ENR |= 0x1 << 1;	/* enable AHB2 clock for port B */
	GPIOB->MODER |= GPIO_MODER_MODE4_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL4_1;	/* PB4: AF2 (TIM3_CH1) */

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	timer->ARR = (uint32_t) 99;
	timer->CR1 |= TIM_CR1_DIR;
	/* CH1 */
	//timer->CCR1 = 50;
	timer->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
	timer->CCMR1 |= TIM_CCMR1_OC1PE;
	timer->CR1 |= TIM_CR1_ARPE;
	timer->EGR = TIM_EGR_UG;
	timer->CCER |= TIM_CCER_CC1E;	/* CH1 */
}

void timer_init_pb5 (TIM_TypeDef *timer)
{
	RCC->AHB2ENR |= 0x1 << 1;	/* enable AHB2 clock for port B */
	GPIOB->MODER |= GPIO_MODER_MODE5_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL5_1;	/* PB5: AF2 (TIM3_CH2) */

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	timer->ARR = (uint32_t) 99;
	timer->CR1 |= TIM_CR1_DIR;
	/* CH2 */
	//timer->CCR1 = 50;
	timer->CCMR1 &= 0xFFFFFCFF;
	timer->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
	timer->CCMR1 |= TIM_CCMR1_OC2PE;
	timer->CR1 |= TIM_CR1_ARPE;
	timer->EGR = TIM_EGR_UG;
	timer->CCER |= TIM_CCER_CC2E;	/* CH2 */
}

void timer_init_pb6 (TIM_TypeDef *timer)
{
	RCC->AHB2ENR |= 0x1 << 1;	/* enable AHB2 clock for port B */
	GPIOB->MODER |= GPIO_MODER_MODE6_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;	/* PB6: AF2 (TIM4_CH1) */

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	timer->ARR = (uint32_t) 99;
	timer->CR1 |= TIM_CR1_DIR;
	/* CH1 */
	//timer->CCR1 = 50;
	timer->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
	timer->CCMR1 |= TIM_CCMR1_OC1PE;
	timer->CR1 |= TIM_CR1_ARPE;
	timer->EGR = TIM_EGR_UG;
	timer->CCER |= TIM_CCER_CC1E;	/* CH1 */
}

void timer_init_pb7 (TIM_TypeDef *timer)
{
	RCC->AHB2ENR |= 0x1 << 1;	/* enable AHB2 clock for port B */
	GPIOB->MODER |= GPIO_MODER_MODE7_1;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_1;	/* PB7: AF2 (TIM4_CH2) */

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	timer->ARR = (uint32_t) 99;
	timer->CR1 |= TIM_CR1_DIR;
	/* CH2 */
	//timer->CCR1 = 50;
	timer->CCMR1 &= 0xFFFFFCFF;
	timer->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
	timer->CCMR1 |= TIM_CCMR1_OC2PE;
	timer->CR1 |= TIM_CR1_ARPE;
	timer->EGR = TIM_EGR_UG;
	timer->CCER |= TIM_CCER_CC2E;	/* CH2 */
}

void timer_init_pb8(TIM_TypeDef *timer)
{
	RCC->AHB2ENR |= 0x1 << 1;	/* enable AHB2 clock for port B */
	GPIOB->MODER |= GPIO_MODER_MODE8_1;
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL8_1;	/* PB8: AF2 (TIM4_CH3) */
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;	// select AF2 for PB10 (PB3 is TIM2_CH3)
	timer->CR1 |= TIM_CR1_DIR;	// counter used as downcounter
	timer->CR1 |= TIM_CR1_ARPE;
	// enable auto-reload preload (buffer TIM2_ARR)
	timer->ARR = (uint32_t) 99;
	// auto-reload prescaler value
	timer->CCMR2 &= 0xFFFFFCFF;
	// select compare 2 (channel 2 is configured as output)
	timer->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1);
	// set output compare 2 mode to PWM mode 1
	timer->CCMR2 |= TIM_CCMR2_OC3PE;
	// enable output compare 2 preload register on TIM2_CCR2
	timer->CCER |= TIM_CCER_CC3E;
		// enable compare 2 output
	timer->EGR = TIM_EGR_UG;
	// re-initialize the counter and generates an update of the registers
}

void timer_init_pb9(TIM_TypeDef *timer)
{
	RCC->AHB2ENR |= 0x1 << 1;	/* enable AHB2 clock for port B */
	GPIOB->MODER |= GPIO_MODER_MODE9_1;
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL9_1;	/* PB8: AF2 (TIM4_CH3) */
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;	// select AF2 for PB10 (PB3 is TIM2_CH3)
	timer->CR1 |= TIM_CR1_DIR;	// counter used as downcounter
	timer->CR1 |= TIM_CR1_ARPE;
	// enable auto-reload preload (buffer TIM2_ARR)
	timer->ARR = (uint32_t) 99;
	// auto-reload prescaler value
	timer->CCMR2 &= 0xFFFFFCFF;
	// select compare 2 (channel 2 is configured as output)
	timer->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1);
	// set output compare 2 mode to PWM mode 1
	timer->CCMR2 |= TIM_CCMR2_OC4PE;
	// enable output compare 2 preload register on TIM2_CCR2
	timer->CCER |= TIM_CCER_CC4E;
		// enable compare 2 output
	timer->EGR = TIM_EGR_UG;
	// re-initialize the counter and generates an update of the registers
}
void SystemClock_Config(){
	RCC->CFGR = 0x00000001;	//choose HSI
	RCC->CR |= RCC_CR_HSION;// turn on HSI16 oscillator
	while((RCC->CR & RCC_CR_HSIRDY) == 0);//check HSI16 ready
	RCC->CFGR &= 0xFFFFFF0F;	//set bit 7:4 clear
	RCC->CFGR |= 11<<4; //SYSCLK divide by 16. SYSCLK = 16MHz/16 = 1Mhz
}

void gpio_init(){
	RCC->AHB2ENR   |= 0x7;
	GPIOA->MODER   &= 0xFFFF03FF;	//set PA5,6,7
	GPIOA->MODER   |= 0x00005400;
	GPIOA->OTYPER  &= 0xFFFFFF1F;
	GPIOA->OSPEEDR &= 0xFFFF03FF;
	GPIOA->OSPEEDR |= 0x00005400;
}
int cal_len(int n)
{
	int sum = 0;
	if(n == 0){
		sum++;
		return sum;
	}

	while (n > 0)
	{
		n /= 10;
		sum++;
	}
	return sum;
}
#define X0 GPIO_Pin_0	//pc0,1,2,3 output
#define X1 GPIO_Pin_1
#define X2 GPIO_Pin_2
#define X3 GPIO_Pin_3
#define Y0 GPIO_Pin_8	//pa8,9,10,11 input
#define Y1 GPIO_Pin_9
#define Y2 GPIO_Pin_10
#define Y3 GPIO_Pin_11
void keypad_init()
{
	RCC->AHB2ENR   |= 0x7;

	GPIOC->MODER   &= 0xFFFFFF00;
	GPIOC->MODER   |= 0x00000055;	//output
	GPIOC->PUPDR   &= 0xFFFFFF00;
	GPIOC->PUPDR   |= 0x00000055;
	GPIOC->OSPEEDR &= 0xFFFFFF00;
	GPIOC->OSPEEDR |= 0x00000055;
	GPIOC->ODR     |= 0x000F;

	GPIOA->MODER   &= 0xFF00FFFF;
	GPIOA->MODER   |= 0x00000000;	//input
	GPIOA->PUPDR   &= 0xFF00FFFF;
	GPIOA->PUPDR   |= 0x00AA0000;
	GPIOA->OSPEEDR &= 0xFF00FFFF;
	GPIOA->OSPEEDR |= 0x00550000;
	GPIOA->IDR	   &= 0xF0FF;

}
signed char keypad_scan()
{
	GPIOC->ODR = X0;
	if (GPIOA->IDR & Y0)	return 1;
	if (GPIOA->IDR & Y1)	return 4;
	if (GPIOA->IDR & Y2)	return 7;
	if (GPIOA->IDR & Y3)	return 15;

	GPIOC->ODR = X1;
	if (GPIOA->IDR & Y0)	return 2;
	if (GPIOA->IDR & Y1)	return 5;
	if (GPIOA->IDR & Y2)	return 8;
	if (GPIOA->IDR & Y3)	return 0;

	GPIOC->ODR = X2;
	if (GPIOA->IDR & Y0)	return 3;
	if (GPIOA->IDR & Y1)	return 6;
	if (GPIOA->IDR & Y2)	return 9;
	if (GPIOA->IDR & Y3)	return 14;

	GPIOC->ODR = X3;
	if (GPIOA->IDR & Y0)	return 10;
	if (GPIOA->IDR & Y1)	return 11;
	if (GPIOA->IDR & Y2)	return 12;
	if (GPIOA->IDR & Y3)	return 13;

	return -1;
}

void ray_init(){
	GPIOC->MODER &= 0xFF00FFFF;	//pc 8,9,10,11
	//GPIOC->MODER |= 0x0000;
	GPIOC->PUPDR &= 0xFF00FFFF;
	GPIOC->PUPDR |= 0x550000;
	GPIOC->IDR   &= 0xCCFF;
}
void give_1st(float duty, int time){
	TIM_TypeDef	*timer = TIM3;
	timer->PSC = (uint32_t) (4000000/50/100);	//period = 0.02sec
	timer_init_pb4(timer);	// for pb4 tim3 ch1
	GPIOB->MODER &= GPIO_MODER_MODE4_1;	//disable others
	timer->CCR1 = duty/2;
	timer->CR1 |= TIM_CR1_CEN;
	int j = time;		//spin 360', one cycle
	while(j > 0){
		j--;
	}
	timer->CR1 &= ~TIM_CR1_CEN;
}
void give_2nd(float duty, int time){
	TIM_TypeDef	*timer = TIM3;
	timer->PSC = (uint32_t) (4000000/50/100);	//period = 0.02sec
	timer_init_pb5(timer);	// for pb5 tim3 ch2
	GPIOB->MODER &= GPIO_MODER_MODE5_1;	//disable others
	timer->CCR2 = duty/2;
	timer->CR1 |= TIM_CR1_CEN;
	int j = time;		//spin 360', one cycle
	while(j > 0){
		j--;
	}

	timer->CR1 &= ~TIM_CR1_CEN;
}
void give_3rd(float duty, int time){
	TIM_TypeDef	*timer = TIM4;
	timer->PSC = (uint32_t) (4000000/50/100);	//period = 0.02sec
	timer_init_pb6(timer);	// for pb6 tim4 ch1
	GPIOB->MODER &= GPIO_MODER_MODE6_1;	//disable others
	timer->CCR1 = duty/2;
	timer->CR1 |= TIM_CR1_CEN;
	int j = time;		//spin 360', one cycle
	while(j > 0){
		j--;
	}
	timer->CR1 &= ~TIM_CR1_CEN;
}
void back_10(float duty, int time){
	TIM_TypeDef	*timer = TIM4;
	timer->PSC = (uint32_t) (4000000/50/100);	//period = 0.02sec
	timer_init_pb7(timer);	// for pb7 tim4 ch2
	GPIOB->MODER &= GPIO_MODER_MODE7_1;	//disable others
	timer->CCR2 = duty/2;
	timer->CR1 |= TIM_CR1_CEN;
	int j = time;		//spin 360', one cycle
	while(j > 0){
		j--;
	}
	timer->CR1 &= ~TIM_CR1_CEN;
}
void back_5(float duty, int time){
	TIM_TypeDef	*timer = TIM3;
	timer->PSC = (uint32_t) (4000000/50/100);	//period = 0.02sec
	timer_init_pb1(timer);	// for pb1 tim3 ch4
	GPIOB->MODER &= GPIO_MODER_MODE1_1;	//disable others
	timer->CCR4 = duty/2;
	timer->CR1 |= TIM_CR1_CEN;
	int j = time;		//spin 360', one cycle
	while(j > 0){
		j--;
	}
	timer->CR1 &= ~TIM_CR1_CEN;
}
void back_1(float duty, int time){
	TIM_TypeDef	*timer = TIM4;
	timer->PSC = (uint32_t) (4000000/50/100);	//period = 0.02sec
	timer_init_pb9(timer);	// for pb9 tim4 ch4
	GPIOB->MODER &= GPIO_MODER_MODE9_1;	//disable others
	timer->CCR4 = duty/2;
	timer->CR1 |= TIM_CR1_CEN;
	int j = time;		//spin 360', one cycle
	while(j > 0){
		j--;
	}
	timer->CR1 &= ~TIM_CR1_CEN;
}




//void ray()
#endif /* UTILS_H_ */
