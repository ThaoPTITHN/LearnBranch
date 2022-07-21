#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stdio.h"
//Tich con IC HC595 : Dich bit : - SCK SDI RCK : SDI : Nhan dl . SCK : clock -> Ban pat clock 
// Clock Cap cu 1 bit truyen di( SDI nhan duoc) thi SCK : keo tu cao ve thap. RCK Chot dl : Cu du 8 bity
// RCK phai keo tu 1 -> 0 
#define  SDI 			GPIO_Pin_0
#define  SCK 		  GPIO_Pin_1
#define  RCK 			GPIO_Pin_2
//2 kenh ADC : 2 button 
#define  Button1  GPIO_Pin_13
#define  Button2  GPIO_Pin_12
//So hinh trang thai : He thong 
typedef enum{
	IDLE,//Ban dau chua button duoc nhan 
	ADC_Channel5,//Trang thai hien thi gt cua Channel 5 len 7 thanh 
	ADC_Channel6// 
}ADC_Channel;//CT struct 
ADC_Channel currentMode = IDLE;
//Rat chuan . De thuc hien . Ko can cau hinh nhieu . Nam cored ko anh huong toi bo nho 
void delay_ms(int time){
	while(time){
		SysTick -> LOAD  = 72000 - 1;
		SysTick -> VAL   = 0;//gan VAL  = 0  
		SysTick -> CTRL  = 5;// Clocksouce AHB . ENable Systick
		while(!(SysTick -> CTRL & (1 << 16))){
		}
		--time;
	}
}
void delay_us(unsigned int time){
	while(time){
		SysTick -> LOAD   = 72 - 1;
		SysTick -> VAL    = 0;
		SysTick -> CTRL   = 5;
		while(!(SysTick -> CTRL & (1 << 16))){
		}
		-- time;
	}
}
void Led7Seg_Configure(){
	RCC -> APB2ENR |= 0x04;//'Cap clock cho port 
	GPIOA -> CRL   &= 0xFFFFF000;//Reset Các chan A0 A1 A2 ve 0 
	GPIOA -> CRL   |= 0x00000333;
}
//Chot dl Sau khi nhan du 8 bit hC595 
void Out(){
    delay_us(1);
    GPIO_SetBits(GPIOA, RCK);
    delay_us(1);
    GPIO_ResetBits(GPIOA, RCK);
}
void shiftout(uint8_t data){
	for(uint8_t i = 0; i< 8; i++){
		if(((data << i) & 0x80 ) == 0x80){
			GPIO_SetBits(GPIOA, SDI);
		}
		else GPIO_ResetBits(GPIOA, SDI);
		delay_us(1);
		GPIO_SetBits(GPIOA, SCK);
		delay_us(1);
		GPIO_ResetBits(GPIOA, SCK);
	}
}
uint16_t Num[10] = {0xBE, 0xA0, 0x2F, 0xAB, 0xB1, 0x9B, 0x9F, 0xA8, 0xBF, 0xBB};
void Play(uint16_t val){
	 uint16_t Display[4] = {0xff, 0xff, 0xff, 0xff};
	 int index = 3;
	 while(val){
		 Display[index] = ~(Num[val % 10]);//1450
		 val = val / 10;
		 index --;
	 }
	 if(val < 0) Display[index] = 0xfe; //0xfe la dau tru
	 for(uint16_t i = 0; i <= 3; i++){
        shiftout(Display[i]);
   }
	 Out();
}
void GPIO_Configuration(){
		/*cap clock cho chan GPIO va bo ADC1*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	  GPIO_InitTypeDef GPIO_InitStructure;
		/*cau hinh chan Input cua bo ADC1 la chan PA0*/
		GPIO_InitStructure.GPIO_Pin 	=  GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/*cau hinh chan Input cua bo ADC1 la chan PA1*/
		GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AIN ;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
}
/*cau hinh cho thanh ghi ADC_Channel5*/
void ADC_Configure(){
	/*Cap clock hoat dong cho ADC in RCC_CFGR_DIV6 72M / 6 = 12 < 14MHz - */
	RCC -> CFGR   |= RCC_CFGR_ADCPRE_DIV6;
	/*cap clock cho APB1*/
	RCC -> APB2ENR|= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;
	/*cau hinh cho chan PA5 in che do INPUT pUSh pull*/
	GPIOA -> CRL  |= GPIO_CRL_CNF5_1;
	GPIOA -> CRL  &=~ GPIO_CRL_CNF5_0;
	/*cau hinh ngat cho ADC*/
	ADC1 -> CR1   |= ADC_CR1_EOCIE;
	NVIC_EnableIRQ(ADC1_2_IRQn);
	/*set sampling rt */
	ADC1 -> SMPR2 |= ADC_SMPR2_SMP5_0 | ADC_SMPR2_SMP5_1 | ADC_SMPR2_SMP5_2;
	/*channel ADC1 convert*/
	ADC1 -> SQR3  |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_2;
	/*Enable ADC firt time and set it continous (dât che do quyet lien tu)*/
	ADC1 -> CR2   |= ADC_CR2_ADON | ADC_CR2_CONT;
	delay_ms(1);
	/*tiep tuc enable cai ADC lan 2*/
	ADC1 -> CR2   |= ADC_CR2_ADON;
	delay_ms(1);
	/*Cho bit CAL bat len 1 bat dau chuyen doi */
	ADC1 -> CR2   |= ADC_CR2_CAL;
	delay_ms(2);
}
void ADC_ConfiguraChannel5(){
		/*cau hinh ADC1*/
	  ADC_InitTypeDef ADC_InitStructure;
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		/* Cau hinh chanel, rank, thoi gian lay mau */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_71Cycles5);
		ADC_Init(ADC1, &ADC_InitStructure);
			/* Cho phep bo ADC1 hoat dong */
		ADC_Cmd(ADC1, ENABLE);   
		/* Reset thanh ghi cablib  */   
		ADC_ResetCalibration(ADC1);
		/* Cho thanh ghi cablib reset xong */
		while(ADC_GetResetCalibrationStatus(ADC1));
		/* Khoi dong bo ADC */
		ADC_StartCalibration(ADC1);	
		/* Cho trang thai cablib duoc bat */
		while(ADC_GetCalibrationStatus(ADC1));
		/* Bat dau chuyen doi ADC */ 
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);	  
}
void ADC_ConfiguraChannel6(){
		/*cau hinh ADC1*/
	  ADC_InitTypeDef ADC_InitStructure;
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;
		/* Cau hinh chanel, rank, thoi gian lay mau */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_71Cycles5);
		ADC_Init(ADC1, &ADC_InitStructure);
			/* Cho phep bo ADC1 hoat dong */
		ADC_Cmd(ADC1, ENABLE);   
		/* Reset thanh ghi cablib  */   
		ADC_ResetCalibration(ADC1);
		/* Cho thanh ghi cablib reset xong */
		while(ADC_GetResetCalibrationStatus(ADC1));
		/* Khoi dong bo ADC */
		ADC_StartCalibration(ADC1);	
		/* Cho trang thai cablib duoc bat */
		while(ADC_GetCalibrationStatus(ADC1));
		/* Bat dau chuyen doi ADC */ 
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);	  
}
void Kiemtra_Button(){
    if(GPIO_ReadInputDataBit(GPIOB, Button1) == 0){
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, Button1) == 0);
        currentMode = ADC_Channel5;
    }
		if(GPIO_ReadInputDataBit(GPIOB, Button2) == 0){
        delay_ms(20);
        while(GPIO_ReadInputDataBit(GPIOB, Button2) == 0);
        currentMode = ADC_Channel6;
		}
}
void Button_Configure(){
	GPIO_InitTypeDef GPIO_Struct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Struct.GPIO_Pin =  Button1 | Button2;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_Struct);
}
int main(){
	Led7Seg_Configure();
	GPIO_Configuration();
	Button_Configure();
	while(1){
		//ADC_Configura();
		Kiemtra_Button();
		ADC_ConfiguraChannel5();
		uint16_t value1 = ADC_GetConversionValue(ADC1);
	  ADC_ConfiguraChannel6();
		uint16_t value2 = ADC_GetConversionValue(ADC1);
		switch(currentMode){
			case ADC_Channel5:
				Play(value1);
			  delay_ms(500);
			break;
			case ADC_Channel6:
				Play(value2);
			  delay_ms(500);
			break;
			default:
			break;
		}
	}
}